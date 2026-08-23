#ifndef HOG2_FORMAT_H
#define HOG2_FORMAT_H

// Stub header for hog2_format.h - minimal definitions for qteditor
#include <cstdint>
#include <cstring>

namespace hog2 {
  struct header_t {
    char magic[4];
    uint32_t entry_count;
    uint32_t file_data_offset;
    char reserved[56];
  };
  
  struct entry_t {
    char name[36];
    uint32_t flags;
    uint32_t len;
    uint32_t timestamp;
  };
}

#endif
