set(CMAKE_FOLDER "third_party")

add_subdirectory(libacm)
add_subdirectory(stb)

if (NOT DEFINED USING_VCPKG) # VCPKG has ports for SDL3 and plog
  message("USING BUNDLED SDL")
  if(USE_EXTERNAL_PLOG)
    find_package(plog REQUIRED)
  else()
    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/plog/CMakeLists.txt)
      message(FATAL_ERROR "plog third-party directory could not be found. "
        "Please run 'submodule update --init --recursive' in the source directory.")
    endif()
    add_subdirectory(plog)
  endif()
endif()
