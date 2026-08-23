/*
 * Stub posix_stream.h for qteditor mini - descar/ directory excluded
 */
#ifndef POSIX_STREAM_H
#define POSIX_STREAM_H

#include <fstream>
#include <string>
#include <cstdint>
#include <cstring>
#include <cerrno>
#include <vector>

namespace hog2 {
  struct entry_t {
    char name[36];
    uint32_t flags;
    uint32_t len;
    uint32_t timestamp;
  };
  
  struct archive_entry_t {
    std::string name;
    uint32_t offset;
    uint32_t length;
    uint32_t timestamp;
    uint32_t flags;
  };

  struct archive_t {
    std::vector<archive_entry_t> entries;

    void addEntry(const entry_t& entry) {
      archive_entry_t ae;
      ae.name = std::string(entry.name);
      ae.offset = 0;
      ae.length = entry.len;
      ae.timestamp = entry.timestamp;
      ae.flags = entry.flags;
      entries.push_back(ae);
    }
    
    void addEntry(const archive_entry_t& entry) {
      entries.push_back(entry);
    }

    const archive_entry_t* getEntry(const std::string& name) const {
      for (const auto& e : entries) {
        if (e.name == name) return &e;
      }
      return nullptr;
    }

    size_t entryCount() const { return entries.size(); }
    
    std::vector<archive_entry_t>::iterator begin() { return entries.begin(); }
    std::vector<archive_entry_t>::iterator end() { return entries.end(); }
    std::vector<archive_entry_t>::const_iterator begin() const { return entries.begin(); }
    std::vector<archive_entry_t>::const_iterator end() const { return entries.end(); }
  };
}

class posix_istream {
private:
    std::ifstream m_file;
    int m_error_code;
public:
    posix_istream() : m_error_code(0) {}

    bool open(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in) {
        errno = 0;
        m_file.open(filename, mode);
        if (!m_file.is_open()) {
            m_error_code = errno;
            return false;
        }
        return true;
    }

    void close() { m_file.close(); }
    bool is_open() const { return m_file.is_open(); }
    int error() const { return m_error_code; }
    bool good() const { return m_file.good(); }
    
    posix_istream& read(char* buf, std::streamsize size) {
        m_file.read(buf, size);
        if (!m_file) m_error_code = errno;
        return *this;
    }
    
    posix_istream& operator>>(class hog2::archive_t& archive);
};

class posix_ostream {
private:
    std::ofstream m_file;
    int m_error_code;
public:
    posix_ostream() : m_error_code(0) {}

    bool open(const std::string& filename, std::ios_base::openmode mode = std::ios_base::out) {
        errno = 0;
        m_file.open(filename, mode);
        if (!m_file.is_open()) {
            m_error_code = errno;
            return false;
        }
        return true;
    }

    void close() { m_file.close(); }
    bool is_open() const { return m_file.is_open(); }
    int error() const { return m_error_code; }
    bool good() const { return m_file.good(); }
    
    posix_ostream& write(const char* buf, std::streamsize size) {
        m_file.write(buf, size);
        if (!m_file) m_error_code = errno;
        return *this;
    }
    
    posix_ostream& operator<<(const class hog2::archive_t& archive);
};

#endif
