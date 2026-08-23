// Stub header for Windows-specific types needed for standalone editor build
#ifndef WIN32_TYPES_STUB_H
#define WIN32_TYPES_STUB_H

#include <cstdint>

// Windows type stubs for editor-only build
typedef int BOOL;
typedef intptr_t SOCKET;
typedef void* HANDLE;
typedef uint32_t DWORD;

// Socket address stub
struct SOCKADDR {
    int sa_family;
    char sa_data[14];
};

#endif // WIN32_TYPES_STUB_H
