set(CMAKE_AUTOMOC ON)

find_package(Qt5 5.15 REQUIRED COMPONENTS Widgets UiTools Test)

qt5_add_resources(QTEDITOR_RESOURCES resources.qrc)

set(QTEDITOR_SOURCES
  about_dialog.cpp
  about_dialog.h
  addscript_dialog.cpp
  addscript_dialog.h
  brief_mission_flags_dialog.cpp
  brief_mission_flags_dialog.h
  brief_text_edit_dialog.cpp
  brief_text_edit_dialog.h
  createscript_dialog.cpp
  createscript_dialog.h
  customize_object_dialog.cpp
  customize_object_dialog.h
  d3_editor_init.cpp
  d3_editor_init.h
  d3_editor_state.cpp
  dallas_prompt_dialogs.cpp
  dialog_test.cpp
  dialog_test.h
  dallas_prompt_dialogs.h
  death_dialog.cpp
  death_dialog.h
  doorway_keypad.cpp
  doorway_keypad.h
  editline_dialog.cpp
  editline_dialog.h
  editor_file_dialogs.cpp
  editor_file_dialogs.h
  editor_room_state.h
  editor_settings.cpp
  editor_settings.h
  editor_test.cpp
  editor_view.cpp
  editor_view.h
  generic_death_dialog.cpp
  generic_death_dialog.h
  hog_dialog.cpp
  hog_dialog.h
  ${PROJECT_SOURCE_DIR}/descar/utils/posix_stream.cpp
  ${PROJECT_SOURCE_DIR}/descar/src/hog2_format.cpp
  keypad_dialog.cpp
  keypad_dialog.h
  megacell_keypad.cpp
  megacell_keypad.h
  level_info_dialog.cpp
  level_info_dialog.h
  level_io.cpp
  level_io.h
  main.cpp
  main_window.cpp
  main_window.h
  path_keypad.cpp
  path_keypad.h
  physics_dialog.cpp
  physics_dialog.h
  player_weapons_dialog.cpp
  player_weapons_dialog.h
  object_ops.cpp
  object_ops.h
  object_clipboard.cpp
  object_clipboard.h
  powprop_dialog.cpp
  powprop_dialog.h
  preferences_dialog.cpp
  preferences_dialog.h
  propai_dialog.cpp
  propai_dialog.h
  qt_messagebox.cpp
  qteditor_dialog.cpp
  qteditor_dialog.h
  robot_weapons_dialog.cpp
  robot_weapons_dialog.h
  room_ops.cpp
  room_ops.h
  selectrange_dialog.cpp
  selectrange_dialog.h
  sound_combo.cpp
  sound_combo.h
  sound_source_dialog.cpp
  sound_source_dialog.h
  status_dialog.cpp
  status_dialog.h
  terrain_keypad.cpp
  terrain_keypad.h
  terrain_sound_dialog.cpp
  terrain_sound_dialog.h
  trigger_keypad.cpp
  trigger_keypad.h
  ui_loader.cpp
  ui_loader.h
  viewer_prop_dialog.cpp
  viewer_prop_dialog.h
  viewer_ops.cpp
  viewer_ops.h
  world_objects_door_dialog.cpp
  world_objects_door_dialog.h
  world_objects_generic_dialog.cpp
  world_objects_generic_dialog.h
  world_objects_player_dialog.cpp
  world_objects_player_dialog.h
  world_sounds_dialog.cpp
  world_sounds_dialog.h
  world_textures_dialog.cpp
  world_textures_dialog.h
  world_weapons_dialog.cpp
  world_weapons_dialog.h
  worldobjectslight_dialog.cpp
  worldobjectslight_dialog.h
)

# The GUI editor excludes the QTest test driver (which has its own main()).
set(QTEDITOR_APP_SOURCES ${QTEDITOR_SOURCES})
list(FILTER QTEDITOR_APP_SOURCES EXCLUDE REGEX "editor_test\.cpp$")

# The test build excludes the editor's own main.cpp (the test driver supplies
# its own main()) but otherwise uses the same sources as the app.
set(QTEDITOR_TEST_SOURCES ${QTEDITOR_SOURCES})
list(FILTER QTEDITOR_TEST_SOURCES EXCLUDE REGEX "main\.cpp$")

add_executable(Descent3EditorQt
  ${QTEDITOR_APP_SOURCES}
  ${QTEDITOR_RESOURCES}
)

target_include_directories(Descent3EditorQt PRIVATE
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/editor
  ${PROJECT_SOURCE_DIR}/lib
  ${PROJECT_SOURCE_DIR}/fix
  ${PROJECT_SOURCE_DIR}/Descent3
  ${PROJECT_SOURCE_DIR}/cfile
  ${PROJECT_SOURCE_DIR}/manage
  ${PROJECT_SOURCE_DIR}/renderer
  ${PROJECT_SOURCE_DIR}/linux
  ${PROJECT_SOURCE_DIR}/vecmat
  ${PROJECT_SOURCE_DIR}/physics
  ${PROJECT_SOURCE_DIR}/grtext
  ${PROJECT_SOURCE_DIR}/bitmap
  ${PROJECT_SOURCE_DIR}/sndlib
  ${PROJECT_SOURCE_DIR}/descar/utils
  ${PROJECT_SOURCE_DIR}/descar/src
  ${PROJECT_BINARY_DIR}/lib
  ${SDL3_INCLUDE_DIRS}
)

# D3 core (game logic) plus the support libraries it pulls in.
set(PLATFORM_LIBS m ${CMAKE_DL_LIBS})

target_link_libraries(Descent3EditorQt PRIVATE
  $<LINK_LIBRARY:WHOLE_ARCHIVE,Descent3Core,linux>
  Qt5::Widgets
  Qt5::UiTools
  ${PLATFORM_LIBS}
)

set_target_properties(Descent3EditorQt PROPERTIES OUTPUT_NAME "Descent3EditorQt")

# QTest based editor tester (exercises every dialog/widget).
add_executable(Descent3EditorTests
  ${QTEDITOR_TEST_SOURCES}
  ${QTEDITOR_RESOURCES}
)
target_include_directories(Descent3EditorTests PRIVATE
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/editor
  ${PROJECT_SOURCE_DIR}/lib
  ${PROJECT_SOURCE_DIR}/fix
  ${PROJECT_SOURCE_DIR}/Descent3
  ${PROJECT_SOURCE_DIR}/cfile
  ${PROJECT_SOURCE_DIR}/manage
  ${PROJECT_SOURCE_DIR}/renderer
  ${PROJECT_SOURCE_DIR}/linux
  ${PROJECT_SOURCE_DIR}/vecmat
  ${PROJECT_SOURCE_DIR}/physics
  ${PROJECT_SOURCE_DIR}/grtext
  ${PROJECT_SOURCE_DIR}/bitmap
  ${PROJECT_SOURCE_DIR}/sndlib
  ${PROJECT_SOURCE_DIR}/descar/utils
  ${PROJECT_SOURCE_DIR}/descar/src
  ${PROJECT_BINARY_DIR}/lib
  ${SDL3_INCLUDE_DIRS}
)
target_link_libraries(Descent3EditorTests PRIVATE
  $<LINK_LIBRARY:WHOLE_ARCHIVE,Descent3Core,linux>
  Qt5::Widgets
  Qt5::UiTools
  Qt5::Test
  ${PLATFORM_LIBS}
)
set_target_properties(Descent3EditorTests PROPERTIES OUTPUT_NAME "Descent3EditorTests")
