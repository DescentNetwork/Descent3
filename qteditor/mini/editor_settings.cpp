/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "editor_settings.h"

#include "d3edit.h"
#include "lib/slew.h"

#include <QSettings>


namespace {

// Field set mirrors SaveEditorSettings/LoadEditorSettings in editor.cpp.
// We deliberately use string key names that match the Win32 record names
// (e.g. "texdlg_texture") so a user migrating from the original editor
// keeps their settings.

void writeInt(QSettings &s, const char *key, int value) {
  s.setValue(QString::fromLatin1(key), value);
}

int readInt(QSettings &s, const char *key, int fallback) {
  return s.value(QString::fromLatin1(key), fallback).toInt();
}

void writeBool(QSettings &s, const char *key, bool value) {
  s.setValue(QString::fromLatin1(key), value);
}

bool readBool(QSettings &s, const char *key, bool fallback) {
  return s.value(QString::fromLatin1(key), fallback).toBool();
}

void writeFloat(QSettings &s, const char *key, float value) {
  s.setValue(QString::fromLatin1(key), value);
}

float readFloat(QSettings &s, const char *key, float fallback) {
  return s.value(QString::fromLatin1(key), fallback).toFloat();
}

} // namespace

void saveEditorSettings(QSettings &settings, const d3edit_state &state) {
  settings.beginGroup(QStringLiteral("editor"));

  writeInt(settings, "texdlg_texture",        state.texdlg_texture);
  writeInt(settings, "current_obj_type",     state.current_obj_type);
  writeInt(settings, "current_obj_id",       state.current_obj_id);
  writeInt(settings, "current_powerup",      state.current_powerup);
  writeInt(settings, "current_door",         state.current_door);
  writeInt(settings, "current_robot",        state.current_robot);
  writeInt(settings, "current_ship",         state.current_ship);
  writeInt(settings, "current_sound",        state.current_sound);
  writeInt(settings, "current_weapon",       state.current_weapon);
  writeInt(settings, "current_path",         state.current_path);
  writeInt(settings, "current_node",         state.current_node);
  writeInt(settings, "current_megacell",     state.current_megacell);
  writeInt(settings, "current_building",     state.current_building);
  writeInt(settings, "current_clutter",      state.current_clutter);

  writeBool(settings, "texscr_visible",      state.texscr_visible);
  writeInt(settings,  "texscr_x",            state.texscr_x);
  writeInt(settings,  "texscr_y",            state.texscr_y);
  writeInt(settings,  "texscr_w",            state.texscr_w);
  writeInt(settings,  "texscr_h",            state.texscr_h);

  writeBool(settings, "wirescr_visible",     state.wirescr_visible);
  writeInt(settings,  "wirescr_x",           state.wirescr_x);
  writeInt(settings,  "wirescr_y",           state.wirescr_y);
  writeInt(settings,  "wirescr_w",           state.wirescr_w);
  writeInt(settings,  "wirescr_h",           state.wirescr_h);

  writeBool(settings, "keypad_visible",      state.keypad_visible);
  writeInt(settings,  "keypad_current",      state.keypad_current);

  writeInt(settings,  "float_keypad_x",      state.float_keypad_x);
  writeInt(settings,  "float_keypad_y",      state.float_keypad_y);
  writeInt(settings,  "float_keypad_w",      state.float_keypad_w);
  writeInt(settings,  "float_keypad_h",      state.float_keypad_h);

  writeInt(settings,  "objmodeless_x",       state.objmodeless_x);
  writeInt(settings,  "objmodeless_y",       state.objmodeless_y);
  writeBool(settings, "objmodeless_on",      state.objmodeless_on);

  writeBool(settings, "tile_views",          state.tile_views);
  writeInt(settings,  "game_render_mode",    state.game_render_mode);

  writeBool(settings, "terrain_dots",        state.terrain_dots);
  writeBool(settings, "terrain_flat_shade",  state.terrain_flat_shade);

  writeBool(settings, "randomize_megacell",  state.randomize_megacell);
  writeInt(settings,  "box_selection_mode",  state.box_selection_mode);
  writeInt(settings,  "object_move_mode",    state.object_move_mode);
  writeInt(settings,  "object_move_axis",    state.object_move_axis);
  writeBool(settings, "fullscreen_debug_state", state.fullscreen_debug_state);

  writeInt(settings,  "texture_display_flags", state.texture_display_flags);

  // Slew_key_speed lives outside D3EditState in d3edit.h but is part of the
  // editor preferences surface; persist it under the same group so a one-stop
  // QSettings backup covers the editor UI tweaks.
  writeFloat(settings, "slew_key_speed",     Slew_key_speed);
  writeBool(settings, "joy_slewing",         state.joy_slewing);
  writeBool(settings, "objects_in_wireframe", state.objects_in_wireframe);

  settings.endGroup();
  settings.sync();
}

void loadEditorSettings(QSettings &settings, d3edit_state &state) {
  settings.beginGroup(QStringLiteral("editor"));

  state.texdlg_texture    = readInt(settings,  "texdlg_texture",    0);
  state.current_obj_type  = readInt(settings,  "current_obj_type",  0);
  state.current_obj_id    = readInt(settings,  "current_obj_id",    0);
  state.current_powerup   = readInt(settings,  "current_powerup",   0);
  state.current_door      = readInt(settings,  "current_door",      0);
  state.current_robot     = readInt(settings,  "current_robot",     0);
  state.current_ship      = readInt(settings,  "current_ship",      0);
  state.current_sound     = readInt(settings,  "current_sound",     0);
  state.current_weapon    = readInt(settings,  "current_weapon",    0);
  state.current_path      = readInt(settings,  "current_path",      0);
  state.current_node      = readInt(settings,  "current_node",      0);
  state.current_megacell  = readInt(settings,  "current_megacell",  0);
  state.current_building  = readInt(settings,  "current_building",  0);
  state.current_clutter   = readInt(settings,  "current_clutter",   0);

  state.texscr_visible    = readBool(settings, "texscr_visible",    false);
  state.texscr_x          = readInt(settings,  "texscr_x",          0);
  state.texscr_y          = readInt(settings,  "texscr_y",          0);
  state.texscr_w          = readInt(settings,  "texscr_w",          0);
  state.texscr_h          = readInt(settings,  "texscr_h",          0);

  state.wirescr_visible   = readBool(settings, "wirescr_visible",   false);
  state.wirescr_x         = readInt(settings,  "wirescr_x",         0);
  state.wirescr_y         = readInt(settings,  "wirescr_y",         0);
  state.wirescr_w         = readInt(settings,  "wirescr_w",         0);
  state.wirescr_h         = readInt(settings,  "wirescr_h",         0);

  state.keypad_visible    = readBool(settings, "keypad_visible",    false);
  state.keypad_current    = readInt(settings,  "keypad_current",    0);

  state.float_keypad_x    = readInt(settings,  "float_keypad_x",    -1);
  state.float_keypad_y    = readInt(settings,  "float_keypad_y",    -1);
  state.float_keypad_w    = readInt(settings,  "float_keypad_w",    -1);
  state.float_keypad_h    = readInt(settings,  "float_keypad_h",    -1);

  state.objmodeless_x     = readInt(settings,  "objmodeless_x",     0);
  state.objmodeless_y     = readInt(settings,  "objmodeless_y",     0);
  state.objmodeless_on    = readBool(settings, "objmodeless_on",    false);

  state.tile_views        = readBool(settings, "tile_views",        false);
  state.game_render_mode  = readInt(settings,  "game_render_mode",  0);

  state.terrain_dots      = readBool(settings, "terrain_dots",      false);
  state.terrain_flat_shade = readBool(settings, "terrain_flat_shade", false);

  state.randomize_megacell = readBool(settings, "randomize_megacell", false);
  state.box_selection_mode = readInt(settings,  "box_selection_mode", 0);
  state.object_move_mode   = readInt(settings,  "object_move_mode",   0);
  state.object_move_axis   = readInt(settings,  "object_move_axis",   0);
  state.fullscreen_debug_state = readBool(settings, "fullscreen_debug_state", false);

  state.texture_display_flags = readInt(settings, "texture_display_flags", 0);

  Slew_key_speed = readFloat(settings, "slew_key_speed", 1.0f);
  state.joy_slewing       = readBool(settings, "joy_slewing",         false);
  state.objects_in_wireframe = readBool(settings, "objects_in_wireframe", false);

  settings.endGroup();
}

