#ifndef QTEDITOR_TOOLS_D3L_CHUNKS_H
#define QTEDITOR_TOOLS_D3L_CHUNKS_H

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

// One entry in a .d3l file's top-level chunk table.
struct d3l_chunk_t
{
  std::string tag;    // 4-character ASCII chunk id (e.g. "LIFE")
  uint32_t size;      // raw size field: counts its own four bytes
  uint64_t offset;    // byte offset of the tag within the file
  uint32_t body_size; // payload bytes (size - 4)
};

// Reads the D3LV header and the complete top-level chunk table of a Descent 3
// level.  Returns false (leaving *error set) when the magic is wrong or the
// chunk table is truncated.  Nothing outside this function's arguments is
// touched, so it can be used to inspect a level without loading it.
bool D3LListChunks(const std::filesystem::path& path,
                   std::vector<d3l_chunk_t>& chunks,
                   uint32_t* version = nullptr,
                   std::string* error = nullptr);

#endif // QTEDITOR_TOOLS_D3L_CHUNKS_H
