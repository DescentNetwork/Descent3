cmake_minimum_required(VERSION 3.16)
project(qteditor_mini VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEDITOR")
# Mini-specific build flag: marks code/tests that only make sense in the
# decoupled mini build (gated with #ifdef MINI_EDITOR in the shared sources so
# the original qteditor build omits them). Set as a target compile definition
# (not just CMAKE_CXX_FLAGS) so AUTOMOC's moc invocation also sees it and
# registers the gated Q_OBJECT test slots.
add_compile_definitions(MINI_EDITOR)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Try Qt6 first, fall back to Qt5
if(NOT FORCE_QT5_EDITOR)
  find_package(Qt6 COMPONENTS Widgets OpenGLWidgets)
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

# Find required packages (SDL2 removed - using Qt for windowing/input)
find_package(OpenGL REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/editor
    ${CMAKE_CURRENT_SOURCE_DIR}/game
    ${CMAKE_CURRENT_SOURCE_DIR}/fix
    ${CMAKE_CURRENT_SOURCE_DIR}/logger
    ${CMAKE_CURRENT_SOURCE_DIR}/mem
    ${CMAKE_CURRENT_SOURCE_DIR}/sndlib
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/2dlib
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux
    ${CMAKE_CURRENT_SOURCE_DIR}/bitmap
    ${CMAKE_CURRENT_SOURCE_DIR}/grtext
    ${CMAKE_CURRENT_SOURCE_DIR}/module
    ${CMAKE_CURRENT_SOURCE_DIR}/cfile
    ${CMAKE_CURRENT_SOURCE_DIR}/manage
    ${CMAKE_CURRENT_SOURCE_DIR}/ddebug
    ${CMAKE_CURRENT_SOURCE_DIR}/misc
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts
    ${CMAKE_CURRENT_SOURCE_DIR}/unzip
    ${PROJECT_SOURCE_DIR}/../../descar/src
    ${PROJECT_SOURCE_DIR}/../../descar/utils
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5OpenGL_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
)

# ---------------------------------------------------------------------------
# Source collection.
#
# The mini build compiles the ORIGINAL qteditor root sources (parent dir) so we
# don't keep duplicate copies in mini/.  The only root sources that remain in
# mini/ are:
#   - the decoupled overrides (modified to build against the mini engine
#     replacement instead of SDL / Descent3Core), and
#   - mini-only sources with no parent counterpart (stubs.cpp, level_loader.cpp).
# The mini engine-replacement subdirs (lib/, editor/, game/, cfile/, manage/,
# bitmap/, mem/, etc.) still come from mini/.
# ---------------------------------------------------------------------------
set(ORIGINAL_QTEDITOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Root .cpp files that were decoupled in mini/ and therefore must NOT be taken
# from the original qteditor dir; the mini copies take precedence.
set(DECOUPLED_OVERRIDES
)

# All original root .cpp sources from the parent qteditor dir...
file(GLOB QTEDITOR_ORIGINAL_SOURCES "${ORIGINAL_QTEDITOR_DIR}/*.cpp")
# ...minus the decoupled ones (they are supplied from mini below).

# Mini root overrides (decoupled files) + mini-only root sources.
set(QTEDITOR_MINI_ROOT_SOURCES "")
foreach(f IN LISTS DECOUPLED_OVERRIDES)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${f}")
    list(APPEND QTEDITOR_MINI_ROOT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${f}")
  endif()
endforeach()
foreach(f stubs.cpp level_loader.cpp brief_model.cpp)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${f}")
    list(APPEND QTEDITOR_MINI_ROOT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${f}")
  endif()
endforeach()

# Mini engine-replacement subdir sources.  CONFIGURE_DEPENDS re-globs this list
# when new .cpp files (e.g. manage/*page.cpp readers) are added, so they are
# picked up without a manual source-list edit.
file(GLOB QTEDITOR_SUBDIR_SOURCES CONFIGURE_DEPENDS
    "lib/*.cpp"
    "editor/*.cpp"
    "game/*.cpp"
    "2dlib/*.cpp"
    "mem/*.cpp"
    "sndlib/*.cpp"
    "md5/*.cpp"
    "cfile/*.cpp"
    "manage/*.cpp"
    "bitmap/*.cpp"
)

set(QTEDITOR_SOURCES
    ${QTEDITOR_ORIGINAL_SOURCES}
    ${QTEDITOR_MINI_ROOT_SOURCES}
    ${QTEDITOR_SUBDIR_SOURCES}
)
# Exclude test files from main build
list(FILTER QTEDITOR_SOURCES EXCLUDE REGEX "editor_test\\.cpp$")
list(FILTER QTEDITOR_SOURCES EXCLUDE REGEX "dialog_test\\.cpp$")
# Copy for the test target, then drop main.cpp (which owns the GUI entry point;
# editor_test.cpp supplies its own main()).
set(QTEDITOR_TEST_SOURCES ${QTEDITOR_SOURCES})
list(FILTER QTEDITOR_TEST_SOURCES EXCLUDE REGEX "(^|/)main\\.cpp$")
# descar sources (used for hog2 archive support)
set(DESCAR_SOURCES
    ${PROJECT_SOURCE_DIR}/../../descar/src/hog2_format.cpp
    ${PROJECT_SOURCE_DIR}/../../descar/utils/posix_stream.cpp
)
file(GLOB QTEDITOR_HEADERS
    "${ORIGINAL_QTEDITOR_DIR}/*.h"
    "*.h"
    "lib/*.h"
    "editor/*.h"
    "game/*.h"
    "2dlib/*.h"
    "mem/*.h"
    "sndlib/*.h"
    "bitmap/*.h"
    "cfile/*.h"
    "ddebug/*.h"
    "ddio/*.h"
    "fix/*.h"
    "logger/*.h"
    "manage/*.h"
    "md5/*.h"
    "misc/*.h"
    "music/*.h"
    "physics/*.h"
    "renderer/*.h"
    "scripts/*.h"
    "unzip/*.h"
    "ui/*.h"
    "${PROJECT_SOURCE_DIR}/../../descar/src/*.h"
    "${PROJECT_SOURCE_DIR}/../../descar/utils/*.h"
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
    ${DESCAR_SOURCES}
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
    Qt6::OpenGLWidgets
    OpenGL::GL
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
  )
  target_compile_definitions(qteditor_mini PRIVATE
    QT_CORE_LIB
    QT_GUI_LIB
    QT_WIDGETS_LIB
    QT_OPENGL_LIB
  )
endif()

# ---------------------------------------------------------------------------
# Standalone QTest executable (Phase 5): exercises every dialog/widget and
# the editor core WITHOUT linking Descent3Core. The test sources already live
# in mini/ (editor_test.cpp is a QTest suite with its own main(); dialog_test.cpp
# provides runDialogTest()). Guarded so builds that don't carry the Qt Test
# module can disable it.
# ---------------------------------------------------------------------------
option(BUILD_EDITOR_TESTS "Build the QTest editor test executable" ON)

if(BUILD_EDITOR_TESTS)
  if(Qt6_FOUND)
    find_package(Qt6 COMPONENTS Test)
  else()
    find_package(Qt5 REQUIRED COMPONENTS Test)
  endif()

  add_executable(qteditor_mini_tests
    ${ORIGINAL_QTEDITOR_DIR}/editor_test.cpp
    ${ORIGINAL_QTEDITOR_DIR}/dialog_test.cpp
    ${ORIGINAL_QTEDITOR_DIR}/dialog_test.h
    ${QTEDITOR_TEST_SOURCES}
    ${DESCAR_SOURCES}
    ${QTEDITOR_HEADERS}
    ${QTEDITOR_UI}
    ${QTEDITOR_RESOURCES}
  )

  if(Qt6_FOUND)
    target_link_libraries(qteditor_mini_tests PRIVATE
      Qt6::Core
      Qt6::Gui
      Qt6::Widgets
      Qt6::OpenGLWidgets
      Qt6::Test
      OpenGL::GL
    )
    target_compile_definitions(qteditor_mini_tests PRIVATE
      QT_CORE_LIB
      QT_GUI_LIB
      QT_WIDGETS_LIB
    )
  else()
    target_link_libraries(qteditor_mini_tests PRIVATE
      Qt5::Core
      Qt5::Gui
      Qt5::Widgets
      Qt5::OpenGL
      Qt5::Test
      OpenGL::GL
    )
    target_compile_definitions(qteditor_mini_tests PRIVATE
      QT_CORE_LIB
      QT_GUI_LIB
      QT_WIDGETS_LIB
      QT_OPENGL_LIB
    )
  endif()
endif()
