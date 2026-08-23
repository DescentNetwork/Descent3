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

class posix_istream {
private:
    std::ifstream m_file;
    int m_error_code;
public:
    posix_istream() : m_error_code(0) {}
    
    bool open(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in) {
        errno = 0;
        m_file.open(filename, mode | std::ios_base::binary);
        m_error_code = errno;
        return m_file.is_open();
    }
    
    void close() {
        if (m_file.is_open()) {
            m_file.close();
        }
    }
    
    bool good() const {
        return m_file.good();
    }
    
    int error() const {
        return m_error_code;
    }
    
    posix_istream& operator>>(class hog2::archive_t& archive);
};

class posix_ostream {
private:
    std::ofstream m_file;
    int m_error_code;
    size_t m_bytes_written;
public:
    posix_ostream() : m_error_code(0), m_bytes_written(0) {}
    
    bool open(const std::string& filename, std::ios_base::openmode mode = std::ios_base::out) {
        errno = 0;
        m_file.open(filename, mode | std::ios_base::binary);
        m_error_code = errno;
        return m_file.is_open();
    }
    
    void close() {
        if (m_file.is_open()) {
            m_file.close();
        }
    }
    
    bool good() const {
        return m_file.good();
    }
    
    int error() const {
        return m_error_code;
    }
    
    size_t bytes_written() const {
        return m_bytes_written;
    }
    
    posix_ostream& operator<<(const class hog2::archive_t& archive);
};

#endif // POSIX_STREAM_H
