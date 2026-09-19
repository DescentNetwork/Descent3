/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral data model for the table-file editor: port of the Win32
 * editor's TableManage.cpp GenericPageNode/GenericPageList classes, rewritten
 * to use std::string / std::filesystem::path instead of MFC CString and
 * char arrays.  The dialog glue (combo box / edit box population, message
 * boxes) lives in the Qt dialog port, not here.
 */

#ifndef TABLE_MANAGE_H
#define TABLE_MANAGE_H

#include <cstdint>
#include <string>

#include "genericpage.h"

// Display text shown when a page has no description string.
inline const std::string NO_DESCRIPTION_STRING = "<None>";

// Name used for the temp table file during a SaveTable (raised over the real
// file once the write completes).
inline const std::string TEMP_TABLE_FILENAME = "TempTable.loc";

// Window title prefix for the table file editor.
inline const std::string TITLE_NAME = "Table File Text Editor v1.0";

// Generic Page Node class
class GenericPageNode {
  friend class GenericPageList;

private:
  GenericPageNode *prev;
  GenericPageNode *next;

  mngs_generic_page genericpage; // Generic Page Data
  uint32_t page_id;              // Page's file index (i.e. the page_id'th page in file)

public:
  GenericPageNode();
  ~GenericPageNode();

  bool operator<(const GenericPageNode &node) const;

  // Accessors for the page data.  The description is stored as a C-string
  // (owned) inside the page struct; it is kept behind these accessors so all
  // new code talks to it in std::string terms only.
  std::string name() const { return genericpage.objinfo_struct.name; }
  std::string description() const;
  void setDescription(const std::string &text);

  uint32_t id() const { return page_id; }
};

// Generic Page List class (maintains linked list of pages)
class GenericPageList {
private:
  GenericPageNode *m_head;
  GenericPageNode *m_tail;
  uint32_t m_size;

  std::string m_TableFilename;
  bool m_TableLoaded;

  bool m_TableModified;

  GenericPageNode *m_SelectedNode;
  uint32_t m_SelectedIndex;

public:
  GenericPageList();
  ~GenericPageList();

  void SetModified(bool mMod) { m_TableModified = mMod; }
  bool IsModified() const { return m_TableModified; }
  bool IsLoaded() const { return m_TableLoaded; }

  // TableManage.php is a disk filename (a .loc table file on disk; never one
  // inside a HOG), so it is carried as a plain std::string.
  const std::string &TableFilename() const { return m_TableFilename; }
  uint32_t size() const { return m_size; }

  void ClearList();
  // Adds the node to the list in ascending alphabetical order (case
  // insensitive) according to the page names.  Takes ownership of the node.
  bool AddToList(GenericPageNode *node);
  bool LoadTable(const std::string &table_filename);
  bool SaveTable(const std::string &table_filename);

  GenericPageNode *FindNode(uint32_t page_id);

  void SelectNext();
  void SelectPrev();
  void SelectNode(uint32_t list_index);

  GenericPageNode *Selected() const { return m_SelectedNode; }
  uint32_t SelectedIndex() const { return m_SelectedIndex; }

  // Mirrors the original SaveSelected(CEdit*): replaces the selected page's
  // description with the given text (an empty string or "<None>" clears it).
  // Returns true if the list got marked modified.
  bool SaveSelectedDescription(const std::string &new_description);

  // Returns the title bar string for the editor dialog, e.g.
  // "Table File Text Editor v1.0 - [level.loc*]".
  std::string TitleString() const;
};

#endif /* TABLE_MANAGE_H */