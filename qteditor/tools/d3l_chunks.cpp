#include "d3l_chunks.h"

#include <posix_stream.h>

#include <cstring>
#include <utility>

namespace
{
// "D3LV" magic followed by a uint32 version.
constexpr uint64_t header_size = 8;
// A chunk is [tag:4][size:4][body]; `size` counts the size field itself.
constexpr uint64_t chunk_header_size = 8;
} // namespace

bool D3LListChunks(const std::filesystem::path& path,
                   std::vector<d3l_chunk_t>& chunks,
                   uint32_t* version,
                   std::string* error)
{
  chunks.clear();

  auto fail = [error](std::string message) {
    if (error)
      *error = std::move(message);
    return false;
  };

  posix_istream in;
  if (!in.open(path, std::ios_base::in | std::ios_base::binary))
    return fail("cannot open " + path.string());

  const uint64_t file_size = in.size();
  if (file_size < header_size)
    return fail("too small to hold a D3LV header");

  char magic[4] = {};
  in.read(magic, sizeof(magic));
  if (std::memcmp(magic, "D3LV", 4) != 0)
    return fail("not a D3LV level (bad magic)");

  uint32_t file_version = 0;
  in >> file_version;
  if (version)
    *version = file_version;

  uint64_t offset = header_size;
  while (offset + chunk_header_size <= file_size)
  {
    if (!in.seek(static_cast<off_t>(offset)))
      return fail("seek to chunk failed");

    char tag[4] = {};
    in.read(tag, sizeof(tag));

    uint32_t size = 0;
    in >> size;
    if (size < 4)
      return fail("chunk " + std::string(tag, 4) + " has an invalid size");

    const uint64_t next = offset + 4 + size;
    if (next > file_size)
      return fail("chunk " + std::string(tag, 4) + " overruns the file");

    chunks.push_back(d3l_chunk_t{std::string(tag, 4), size, offset, size - 4});
    offset = next;
  }

  if (offset != file_size)
    return fail("trailing bytes after the last chunk");

  in.close();
  return true;
}
