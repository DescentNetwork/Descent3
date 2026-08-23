cmake_minimum_required(VERSION 3.16)
project(qteditor_mini VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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

# Source files from qteditor directory
set(QTEDITOR_SOURCES
    main.cpp
    main_window.cpp
    d3_editor_init.cpp
    d3_editor_state.cpp
    editor_file_dialogs.cpp
    editor_lighting.cpp
    editor_object_lighting.cpp
    editor_settings.cpp
    editor_view.cpp
    level_io.cpp
    obj_move_manager.cpp
    object_ops.cpp
    room_ops.cpp
    rad_cast.cpp
    rad_hemicube.cpp
    rad_init.cpp
    rad_stubs.cpp
    terrain_keypad.cpp
    # Add more source files as needed
)

# Header files
set(QTEDITOR_HEADERS
    d3edit.h
    main_window.h
    d3_editor_init.h
    editor_lighting.h
    editor_room_state.h
    editor_settings.h
    editor_view.h
    level_io.h
    obj_move_manager.h
    object_ops.h
    radiosity.h
    # Add more header files as needed
)

# UI files
set(QTEDITOR_UI
    # Add .ui files here if any
)

# Resources
set(QTEDITOR_RESOURCES
    resources.qrc
)

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
