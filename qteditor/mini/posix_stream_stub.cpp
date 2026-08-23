#include "lib/posix_stream.h"
#include <cstring>

// Stub implementations for hog2 archive stream operators
namespace {
    const uint32_t HOG2_MAGIC = 0x484F4732; // "HOG2"
}

posix_istream& posix_istream::operator>>(hog2::archive_t& archive) {
    // Read header
    uint32_t magic;
    uint32_t entry_count;
    uint32_t file_data_offset;
    
    read(reinterpret_cast<char*>(&magic), sizeof(magic));
    read(reinterpret_cast<char*>(&entry_count), sizeof(entry_count));
    read(reinterpret_cast<char*>(&file_data_offset), sizeof(file_data_offset));
    
    // Skip reserved bytes
    char reserved[56];
    read(reserved, sizeof(reserved));
    
    // Read entries
    for (uint32_t i = 0; i < entry_count; ++i) {
        hog2::archive_entry_t entry;
        char name[36];
        uint32_t flags, len, timestamp;
        
        read(name, 36);
        read(reinterpret_cast<char*>(&flags), sizeof(flags));
        read(reinterpret_cast<char*>(&len), sizeof(len));
        read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
        
        entry.name = std::string(name);
        entry.offset = file_data_offset; // Simplified
        entry.length = len;
        entry.timestamp = timestamp;
        entry.flags = flags;
        
        archive.entries.push_back(entry);
        file_data_offset += len; // Simplified
    }
    
    return *this;
}

posix_ostream& posix_ostream::operator<<(const hog2::archive_t& archive) {
    // Write header
    uint32_t magic = HOG2_MAGIC;
    uint32_t entry_count = static_cast<uint32_t>(archive.entries.size());
    uint32_t file_data_offset = 116; // Header + entry table
    
    write(reinterpret_cast<const char*>(&magic), sizeof(magic));
    write(reinterpret_cast<const char*>(&entry_count), sizeof(entry_count));
    write(reinterpret_cast<const char*>(&file_data_offset), sizeof(file_data_offset));
    
    // Write reserved bytes
    char reserved[56] = {0};
    write(reserved, sizeof(reserved));
    
    // Write entries
    for (const auto& entry : archive.entries) {
        char name[36] = {0};
        std::strncpy(name, entry.name.c_str(), 35);
        uint32_t flags = entry.flags;
        uint32_t len = entry.length;
        uint32_t timestamp = entry.timestamp;
        
        write(name, 36);
        write(reinterpret_cast<const char*>(&flags), sizeof(flags));
        write(reinterpret_cast<const char*>(&len), sizeof(len));
        write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
    }
    
    return *this;
}
