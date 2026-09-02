/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral port of the Win32 editor's TableManage.cpp
 * (GenericPageNode / GenericPageList).  I/O uses the mini byte streams
 * (posix_istream/posix_ostream) and std::filesystem instead of CFILE/CString.
 */

#include "table_manage.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <vector>

#include <posix_stream.h>
#include "posix_helpers.h"
#include "string_helpers.h"
#include "mem.h"

namespace {

// Case-insensitive ASCII comparison (replaces the original stricmp).
bool CiStrLess(const std::string &a, const std::string &b) {
  auto ia = a.begin();
  auto ib = b.begin();
  for (; ia != a.end() && ib != b.end(); ++ia, ++ib) {
    unsigned char ca = std::tolower(static_cast<unsigned char>(*ia));
    unsigned char cb = std::tolower(static_cast<unsigned char>(*ib));
    if (ca != cb)
      return ca < cb;
  }
  return (ia == a.end()) && (ib != b.end());
}

// Reads the [len][data] payload of a non-generic page.  If outfile is
// non-null, re-emits the page verbatim (pagetype byte + length + payload,
// the analogue of the original mng_ReadWriteDummyPage).
bool CopyPagePayload(byte_istream &infile, uint8_t pagetype, byte_ostream *outfile) {
  int32_t len = 0;
  infile >> len;
  if (len < 0)
    return false;

  if (outfile) {
    outfile->put(pagetype);
    *outfile << len;
  }

  std::vector<uint8_t> buffer(static_cast<size_t>(std::min<int32_t>(len, 0x4000)));
  size_t passed = 0;
  while (passed < static_cast<size_t>(len)) {
    const size_t chunk = std::min<size_t>(static_cast<size_t>(len) - passed, buffer.size());
    infile.read(buffer.data(), chunk);
    if (outfile)
      outfile->write(buffer.data(), chunk);
    passed += chunk;
  }
  return true;
}

} // namespace

////////////////////////////////////////////////////////////
// Generic Page Node Class Members
////////////////////////////////////////////////////////////
GenericPageNode::GenericPageNode() {
  prev = nullptr;
  next = nullptr;
  page_id = 0;
  genericpage.objinfo_struct.description = nullptr;
}

GenericPageNode::~GenericPageNode() {
  // Free up the object description memory
  if (genericpage.objinfo_struct.description != nullptr) {
    mem_free(genericpage.objinfo_struct.description);
    genericpage.objinfo_struct.description = nullptr;
  }
}

bool GenericPageNode::operator<(const GenericPageNode &node) const {
  return CiStrLess(genericpage.objinfo_struct.name, node.genericpage.objinfo_struct.name);
}

std::string GenericPageNode::description() const {
  const char *d = genericpage.objinfo_struct.description;
  return d != nullptr ? std::string(d) : std::string();
}

void GenericPageNode::setDescription(const std::string &text) {
  char *old = genericpage.objinfo_struct.description;
  char *next = nullptr;
  if (!text.empty()) {
    next = mem_rmalloc<char>(text.size() + 1);
    std::memcpy(next, text.data(), text.size() + 1);
  }
  genericpage.objinfo_struct.description = next;
  if (old != nullptr)
    mem_free(old);
}

////////////////////////////////////////////////////////////
// Generic Page List Class Members
////////////////////////////////////////////////////////////
GenericPageList::GenericPageList() {
  m_head = nullptr;
  m_tail = nullptr;
  m_size = 0;
  m_TableFilename.clear();
  m_TableLoaded = false;
  m_TableModified = false;
  m_SelectedNode = nullptr;
  m_SelectedIndex = 0;
}

GenericPageList::~GenericPageList() { ClearList(); }

void GenericPageList::ClearList(void) {
  GenericPageNode *node = m_head;
  while (node != nullptr) {
    GenericPageNode *temp = node;
    node = node->next;
    delete temp;
  }
  m_size = 0;
  m_head = nullptr;
  m_tail = nullptr;
  m_TableFilename.clear();
  m_TableLoaded = false;
  m_TableModified = false;
  m_SelectedNode = nullptr;
  m_SelectedIndex = 0;
}

// Adds the node to the list in ascending alphabetical order
// according to the page names of the nodes
bool GenericPageList::AddToList(GenericPageNode *new_node) {
  if (new_node == nullptr)
    return false;

  // Handle empty list case
  if (m_head == nullptr) {
    m_head = new_node;
    m_tail = new_node;
    new_node->next = nullptr;
    new_node->prev = nullptr;
    m_size++;
    return true;
  }

  // Handle (new node < list head) case
  if ((*new_node) < (*m_head)) {
    new_node->next = m_head;
    new_node->prev = nullptr;
    m_head->prev = new_node;
    m_head = new_node;
    m_size++;
    return true;
  }

  // Search through list until end of list or until the new node is less
  // than the next node, then add the new node after the current node
  GenericPageNode *node = m_head;
  while (node->next != nullptr && (*(node->next)) < (*new_node))
    node = node->next;

  new_node->next = node->next;
  new_node->prev = node;

  if (node->next == nullptr)
    m_tail = new_node;
  else
    node->next->prev = new_node;

  node->next = new_node;

  m_size++;

  return true;
}

bool GenericPageList::LoadTable(const std::string &table_filename) {
  posix_istream infile(table_filename, std::ios_base::in);
  if (!infile.good())
    return false;

  // Wipe out the list first
  ClearList();

  uint32_t page_id = 0;
  while (!infile.eof()) {
    uint8_t pagetype = 0;
    infile >> pagetype;
    if (infile.eof())
      break;

    // If not a generic page, just read it in and ignore it
    if (pagetype != PAGETYPE_GENERIC) {
      if (!CopyPagePayload(infile, pagetype, nullptr))
        return false;
      page_id++;
      continue;
    }

    // Skip over the page-frame length field; mng_ReadNewGenericPage starts at
    // the payload (the version byte), mirroring gamedata_loader.
    {
      int32_t framelen = 0;
      infile >> framelen;
      if (framelen < 0)
        return false;
    }

    GenericPageNode *node = new GenericPageNode;
    if (!mng_ReadNewGenericPage(infile, &node->genericpage)) {
      delete node;
      return false;
    }
    node->page_id = page_id;
    AddToList(node);

    page_id++;
  }

  // Set loaded table data
  m_TableFilename = table_filename;
  m_TableLoaded = true;
  m_TableModified = false;

  return true;
}

bool GenericPageList::SaveTable(const std::string &table_filename) {
  // If no table is loaded, get outta here
  if (!m_TableLoaded)
    return false;

  // The edits are always read from the currently-loaded table and written to a
  // temp file; the finished temp file then takes the place of `target` (the
  // passed-in "Save As" path, or the loaded file when none is given) -- the
  // same flow as the original SwitcherooFiles-based SaveTable.
  const std::string target = table_filename.empty() ? m_TableFilename : table_filename;
  // The temp file lives next to the target so the final rename never crosses a
  // filesystem boundary (the original relied on the temp being in the same dir
  // as the table file).
  const std::string temp = (std::filesystem::path(target).parent_path() / TEMP_TABLE_FILENAME).string();

  posix_istream infile(m_TableFilename, std::ios_base::in);
  if (!infile.good())
    return false;

  posix_ostream outfile(temp, std::ios_base::out);
  if (!outfile.good())
    return false;

  // Read through the entire page file and replace every page with the one at
  // the same list position.  If a page can't be found, just write the
  // original page back out.
  uint32_t page_id = 0;
  while (!infile.eof()) {
    uint8_t pagetype = 0;
    infile >> pagetype;
    if (infile.eof())
      break;

    // If not a generic page, copy it through unchanged
    if (pagetype != PAGETYPE_GENERIC) {
      if (!CopyPagePayload(infile, pagetype, &outfile))
        return false;
      page_id++;
      continue;
    }

    // Skip over the page-frame length field; mng_ReadNewGenericPage starts at
    // the payload (the version byte), mirroring gamedata_loader.
    {
      int32_t framelen = 0;
      infile >> framelen;
      if (framelen < 0)
        return false;
    }

    mngs_generic_page genericpage{};
    if (!mng_ReadNewGenericPage(infile, &genericpage))
      return false;

    // Look for the generic page with the current page position in list
    GenericPageNode *node = FindNode(page_id);
    if (node != nullptr)
      mng_WriteNewGenericPage(outfile, &node->genericpage);
    else
      mng_WriteNewGenericPage(outfile, &genericpage);

    // Free the temporarily-read page's description memory (it is not adopted
    // by the list)
    if (genericpage.objinfo_struct.description != nullptr) {
      mem_free(genericpage.objinfo_struct.description);
      genericpage.objinfo_struct.description = nullptr;
    }

    page_id++;
  }

  infile.close();
  outfile.close();

  // Perform the switcheroo: delete the real table file and rename the temp
  // table file to be the new "real" table file.
  std::error_code ec;
  std::filesystem::remove(target, ec);
  std::filesystem::rename(temp, target, ec);
  if (ec)
    return false;

  m_TableModified = false;
  m_TableFilename = target;

  return true; // successful!
}

GenericPageNode *GenericPageList::FindNode(uint32_t page_id) {
  GenericPageNode *node = m_head;
  while (node != nullptr) {
    if (node->page_id == page_id)
      return node;
    node = node->next;
  }
  return nullptr;
}

void GenericPageList::SelectNext(void) {
  if (m_SelectedNode == nullptr)
    return;

  if (m_SelectedNode->next == nullptr) {
    m_SelectedNode = m_head;
    m_SelectedIndex = 0;
  } else {
    m_SelectedNode = m_SelectedNode->next;
    m_SelectedIndex++;
  }
}

void GenericPageList::SelectPrev(void) {
  if (m_SelectedNode == nullptr)
    return;

  if (m_SelectedNode->prev == nullptr) {
    m_SelectedNode = m_tail;
    m_SelectedIndex = (m_size - 1);
  } else {
    m_SelectedNode = m_SelectedNode->prev;
    m_SelectedIndex--;
  }
}

void GenericPageList::SelectNode(uint32_t list_index) {
  if (m_size == 0 || list_index > (m_size - 1))
    return;

  GenericPageNode *node = m_head;
  for (uint32_t j = 0; j < list_index; j++)
    if (node != nullptr)
      node = node->next;

  m_SelectedNode = node;
  m_SelectedIndex = list_index;
}

bool GenericPageList::SaveSelectedDescription(const std::string &new_description) {
  if (m_SelectedNode == nullptr)
    return false;

  std::string new_text = new_description;
  if (new_text.empty() || match(new_text, NO_DESCRIPTION_STRING))
    new_text.clear();

  const std::string old_text = m_SelectedNode->description();

  // If both new and old are empty, get outta here
  if (old_text.empty() && new_text.empty())
    return false;

  // If both have descriptions and they match, get outta here
  if (!old_text.empty() && !new_text.empty() && old_text == new_text)
    return false;

  m_SelectedNode->setDescription(new_text);
  m_TableModified = true;

  return true;
}

std::string GenericPageList::TitleString() const {
  std::string title = TITLE_NAME;
  if (m_TableLoaded)
    title += " - [" + m_TableFilename + (m_TableModified ? "*" : "") + "]";
  return title;
}
