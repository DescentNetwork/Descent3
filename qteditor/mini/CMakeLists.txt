cmake_minimum_required(VERSION 3.16)
project(qteditor_mini VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEDITOR")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Try Qt6 first, fall back to Qt5
if(NOT FORCE_QT5_EDITOR)
  find_package(Qt6 COMPONENTS Widgets)
endif()

if(Qt6_FOUND)
  set(QT_VERSION_MAJOR ${Qt6Core_VERSION_MAJOR})
  set(QT_VERSION_MINOR ${Qt6Core_VERSION_MINOR})
  message(STATUS "Using Qt6")
else()
  find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets OpenGL)
  set(QT_VERSION_MAJOR ${Qt5Core_VERSION_MAJOR})
  set(QT_VERSION_MINOR ${Qt5Core_VERSION_MINOR})
  message(STATUS "Using Qt5")
endif()

# Find required packages
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/editor
    ${CMAKE_CURRENT_SOURCE_DIR}/game
    ${CMAKE_CURRENT_SOURCE_DIR}/fix
    ${CMAKE_CURRENT_SOURCE_DIR}/logger
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5OpenGL_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
)

# Collect all source files from root and subdirectories
file(GLOB QTEDITOR_SOURCES 
    "*.cpp"
    "lib/*.cpp"
    "editor/*.cpp"
    "game/*.cpp"
    "2dlib/*.cpp"
    "mem/*.cpp"
    "sndlib/*.cpp"
)
file(GLOB QTEDITOR_HEADERS 
    "*.h"
    "lib/*.h"
    "editor/*.h"
    "game/*.h"
    "2dlib/*.h"
    "mem/*.h"
    "sndlib/*.h"
)
file(GLOB QTEDITOR_UI "ui/*.ui")

# Resources
set(QTEDITOR_RESOURCES
    resources.qrc
)

# Set UIC search path for UI files
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

# Create executable
add_executable(qteditor_mini
    ${QTEDITOR_SOURCES}
    ${QTEDITOR_HEADERS}
    ${QTEDITOR_UI}
    ${QTEDITOR_RESOURCES}
)

# Link libraries
if(Qt6_FOUND)
  target_link_libraries(qteditor_mini PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    OpenGL::GL
    SDL2::SDL2
  )
  target_compile_definitions(qteditor_mini PRIVATE
    QT_CORE_LIB
    QT_GUI_LIB
    QT_WIDGETS_LIB
  )
else()
  target_link_libraries(qteditor_mini PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::OpenGL
    OpenGL::GL
    SDL2::SDL2
  )
  target_compile_definitions(qteditor_mini PRIVATE
    QT_CORE_LIB
    QT_GUI_LIB
    QT_WIDGETS_LIB
    QT_OPENGL_LIB
  )
endif()
