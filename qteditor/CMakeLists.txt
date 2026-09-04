cmake_minimum_required(VERSION 3.16)
project(qteditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEDITOR")
# Mini-specific build flag: marks code/tests that only make sense in the
# decoupled mini build (gated with #ifdef MINI_EDITOR in the shared sources so
# the original qteditor build omits them). Set as a target compile definition
# (not just CMAKE_CXX_FLAGS) so AUTOMOC's moc invocation also sees it and
# registers the gated Q_OBJECT test slots.
#add_compile_defxinitions(MINI_EDITOR)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(FORCE_QT5_EDITOR "Build the editor with Qt5" ON)

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

set(MINI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/mini)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MINI_SRC}
    ${MINI_SRC}/lib
    ${MINI_SRC}/editor
    ${MINI_SRC}/game
    ${MINI_SRC}/fix
    ${MINI_SRC}/logger
    ${MINI_SRC}/mem
    ${MINI_SRC}/sndlib
    ${MINI_SRC}/physics
    ${MINI_SRC}/2dlib
    ${MINI_SRC}/ui
    ${MINI_SRC}/lib/linux
    ${MINI_SRC}/bitmap
    ${MINI_SRC}/grtext
    ${MINI_SRC}/module
    ${MINI_SRC}/cfile
    ${MINI_SRC}/manage
    ${MINI_SRC}/ddebug
    ${MINI_SRC}/misc
    ${MINI_SRC}/renderer
    ${MINI_SRC}/scripts
    ${MINI_SRC}/unzip
    ${PROJECT_SOURCE_DIR}/../descar/src
    ${PROJECT_SOURCE_DIR}/../descar/utils
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

# All original root .cpp sources from the parent qteditor dir...
file(GLOB QTEDITOR_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Mini engine-replacement subdir sources.  CONFIGURE_DEPENDS re-globs this list
# when new .cpp files (e.g. manage/*page.cpp readers) are added, so they are
# picked up without a manual source-list edit.
file(GLOB QTEDITOR_SUBDIR_SOURCES CONFIGURE_DEPENDS
    "${MINI_SRC}/*.cpp"
    "${MINI_SRC}/lib/*.cpp"
    "${MINI_SRC}/editor/*.cpp"
    "${MINI_SRC}/game/*.cpp"
    "${MINI_SRC}/2dlib/*.cpp"
    "${MINI_SRC}/mem/*.cpp"
    "${MINI_SRC}/sndlib/*.cpp"
    "${MINI_SRC}/md5/*.cpp"
    "${MINI_SRC}/cfile/*.cpp"
    "${MINI_SRC}/manage/*.cpp"
    "${MINI_SRC}/bitmap/*.cpp"
    "${MINI_SRC}/physics/*.cpp"
    "${MINI_SRC}/renderer/*.cpp"
    "${MINI_SRC}/model/*.cpp"
)

# Headers from the mini engine replacement (AUTOMOC/AUTOUIC discovery and IDE
# browsing); the parent-dir headers are already inside QTEDITOR_SOURCES.
file(GLOB QTEDITOR_HEADERS
    "${MINI_SRC}/*.h"
    "${MINI_SRC}/lib/*.h"
    "${MINI_SRC}/editor/*.h"
    "${MINI_SRC}/game/*.h"
    "${MINI_SRC}/2dlib/*.h"
    "${MINI_SRC}/mem/*.h"
    "${MINI_SRC}/sndlib/*.h"
    "${MINI_SRC}/bitmap/*.h"
    "${MINI_SRC}/cfile/*.h"
    "${MINI_SRC}/ddebug/*.h"
    "${MINI_SRC}/ddio/*.h"
    "${MINI_SRC}/fix/*.h"
    "${MINI_SRC}/logger/*.h"
    "${MINI_SRC}/manage/*.h"
    "${MINI_SRC}/md5/*.h"
    "${MINI_SRC}/misc/*.h"
    "${MINI_SRC}/music/*.h"
    "${MINI_SRC}/model/*.h"
    "${MINI_SRC}/physics/*.h"
    "${MINI_SRC}/renderer/*.h"
    "${MINI_SRC}/scripts/*.h"
    "${MINI_SRC}/unzip/*.h"
    "${MINI_SRC}/ui/*.h"
    "${PROJECT_SOURCE_DIR}/../descar/src/*.h"
    "${PROJECT_SOURCE_DIR}/../descar/utils/*.h"
)

set(QTEDITOR_SOURCES
    ${QTEDITOR_SOURCES}
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../descar/src/hog2_format.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../descar/utils/posix_stream.cpp
)

file(GLOB QTEDITOR_UI "ui/*.ui")

# Resources
set(QTEDITOR_RESOURCES
    resources.qrc
)

# Set UIC search path for UI files
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

# Create executable
add_executable(qteditor
    ${QTEDITOR_SOURCES}
    ${QTEDITOR_HEADERS}
    ${DESCAR_SOURCES}
    ${QTEDITOR_UI}
    ${QTEDITOR_RESOURCES}
)

# Link libraries
if(Qt6_FOUND)
  target_link_libraries(qteditor PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGLWidgets
    OpenGL::GL
  )
  target_compile_definitions(qteditor PRIVATE
    QT_CORE_LIB
    QT_GUI_LIB
    QT_WIDGETS_LIB
  )
else()
  target_link_libraries(qteditor PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::OpenGL
    OpenGL::GL
  )
  target_compile_definitions(qteditor PRIVATE
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

  add_executable(qteditor_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/editor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dialog_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dialog_test.h
    ${QTEDITOR_TEST_SOURCES}
    ${QTEDITOR_HEADERS}
    ${DESCAR_SOURCES}
    ${QTEDITOR_UI}
    ${QTEDITOR_RESOURCES}
  )

  if(Qt6_FOUND)
    target_link_libraries(qteditor_tests PRIVATE
      Qt6::Core
      Qt6::Gui
      Qt6::Widgets
      Qt6::OpenGLWidgets
      Qt6::Test
      OpenGL::GL
    )
    target_compile_definitions(qteditor_tests PRIVATE
      QT_CORE_LIB
      QT_GUI_LIB
      QT_WIDGETS_LIB
    )
  else()
    target_link_libraries(qteditor_tests PRIVATE
      Qt5::Core
      Qt5::Gui
      Qt5::Widgets
      Qt5::OpenGL
      Qt5::Test
      OpenGL::GL
    )
    target_compile_definitions(qteditor_tests PRIVATE
      QT_CORE_LIB
      QT_GUI_LIB
      QT_WIDGETS_LIB
      QT_OPENGL_LIB
    )
  endif()
endif()
