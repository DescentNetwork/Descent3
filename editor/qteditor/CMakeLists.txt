set(CMAKE_AUTOMOC ON)

find_package(Qt5 5.15 REQUIRED COMPONENTS Widgets UiTools)

qt5_add_resources(QTEDITOR_RESOURCES ../editor.qrc)

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
  dallas_prompt_dialogs.h
  death_dialog.cpp
  death_dialog.h
  editline_dialog.cpp
  editline_dialog.h
  generic_death_dialog.cpp
  generic_death_dialog.h
  hog_dialog.cpp
  hog_dialog.h
  keypad_dialog.cpp
  keypad_dialog.h
  level_info_dialog.cpp
  level_info_dialog.h
  main.cpp
  main_window.cpp
  main_window.h
  physics_dialog.cpp
  physics_dialog.h
  powprop_dialog.cpp
  powprop_dialog.h
  preferences_dialog.cpp
  preferences_dialog.h
  propai_dialog.cpp
  propai_dialog.h
  qteditor_dialog.cpp
  qteditor_dialog.h
  selectrange_dialog.cpp
  selectrange_dialog.h
  sound_combo.cpp
  sound_combo.h
  sound_source_dialog.cpp
  sound_source_dialog.h
  splash_dialog.cpp
  splash_dialog.h
  status_dialog.cpp
  status_dialog.h
  terrain_sound_dialog.cpp
  terrain_sound_dialog.h
  ui_loader.cpp
  ui_loader.h
  viewer_prop_dialog.cpp
  viewer_prop_dialog.h
  worldobjectslight_dialog.cpp
  worldobjectslight_dialog.h
)

add_executable(Descent3EditorQt
  ${QTEDITOR_SOURCES}
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
