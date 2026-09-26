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
#include "lib/slew.h"

#include "d3edit.h"

#include <QSettings>


// Field set mirrors SaveEditorSettings/LoadEditorSettings in editor.cpp.
// We deliberately use string key names that match the Win32 record names
// (e.g. "texdlg_texture") so a user migrating from the original editor
// keeps their settings.

void saveEditorSettings(QSettings &settings, const d3edit_state &state) {
  settings.beginGroup(QStringLiteral("editor"));

  settings.setValue(QStringLiteral("texdlg_texture"),        state.texdlg_texture);
  settings.setValue(QStringLiteral("current_obj_type"),      state.current_obj_type);
  settings.setValue(QStringLiteral("current_obj_id"),        state.current_obj_id);
  settings.setValue(QStringLiteral("current_powerup"),       state.current_powerup);
  settings.setValue(QStringLiteral("current_door"),          state.current_door);
  settings.setValue(QStringLiteral("current_robot"),         state.current_robot);
  settings.setValue(QStringLiteral("current_ship"),          state.current_ship);
  settings.setValue(QStringLiteral("current_sound"),         state.current_sound);
  settings.setValue(QStringLiteral("current_weapon"),        state.current_weapon);
  settings.setValue(QStringLiteral("current_path"),          state.current_path);
  settings.setValue(QStringLiteral("current_node"),          state.current_node);
  settings.setValue(QStringLiteral("current_megacell"),      state.current_megacell);
  settings.setValue(QStringLiteral("current_building"),      state.current_building);
  settings.setValue(QStringLiteral("current_clutter"),       state.current_clutter);

  settings.setValue(QStringLiteral("texscr_visible"),        state.texscr_visible);
  settings.setValue(QStringLiteral("texscr_x"),              state.texscr_x);
  settings.setValue(QStringLiteral("texscr_y"),              state.texscr_y);
  settings.setValue(QStringLiteral("texscr_w"),              state.texscr_w);
  settings.setValue(QStringLiteral("texscr_h"),              state.texscr_h);

  settings.setValue(QStringLiteral("wirescr_visible"),       state.wirescr_visible);
  settings.setValue(QStringLiteral("wirescr_x"),             state.wirescr_x);
  settings.setValue(QStringLiteral("wirescr_y"),             state.wirescr_y);
  settings.setValue(QStringLiteral("wirescr_w"),             state.wirescr_w);
  settings.setValue(QStringLiteral("wirescr_h"),             state.wirescr_h);

  settings.setValue(QStringLiteral("keypad_visible"),        state.keypad_visible);
  settings.setValue(QStringLiteral("keypad_current"),        state.keypad_current);

  settings.setValue(QStringLiteral("float_keypad_x"),        state.float_keypad_x);
  settings.setValue(QStringLiteral("float_keypad_y"),        state.float_keypad_y);
  settings.setValue(QStringLiteral("float_keypad_w"),        state.float_keypad_w);
  settings.setValue(QStringLiteral("float_keypad_h"),        state.float_keypad_h);

  settings.setValue(QStringLiteral("objmodeless_x"),         state.objmodeless_x);
  settings.setValue(QStringLiteral("objmodeless_y"),         state.objmodeless_y);
  settings.setValue(QStringLiteral("objmodeless_on"),        state.objmodeless_on);

  settings.setValue(QStringLiteral("tile_views"),            state.tile_views);
  settings.setValue(QStringLiteral("game_render_mode"),      state.game_render_mode);

  settings.setValue(QStringLiteral("terrain_dots"),          state.terrain_dots);
  settings.setValue(QStringLiteral("terrain_flat_shade"),    state.terrain_flat_shade);

  settings.setValue(QStringLiteral("randomize_megacell"),    state.randomize_megacell);
  settings.setValue(QStringLiteral("box_selection_mode"),    state.box_selection_mode);
  settings.setValue(QStringLiteral("object_move_mode"),      state.object_move_mode);
  settings.setValue(QStringLiteral("object_move_axis"),      state.object_move_axis);
  settings.setValue(QStringLiteral("fullscreen_debug_state"), state.fullscreen_debug_state);

  settings.setValue(QStringLiteral("texture_display_flags"), state.texture_display_flags);

  // Slew_key_speed lives outside app in d3edit.h but is part of the
  // editor preferences surface; persist it under the same group so a one-stop
  // QSettings backup covers the editor UI tweaks.
  settings.setValue(QStringLiteral("slew_key_speed"),        Slew_key_speed);
  settings.setValue(QStringLiteral("joy_slewing"),           state.joy_slewing);
  settings.setValue(QStringLiteral("objects_in_wireframe"),  state.objects_in_wireframe);

  settings.endGroup();
  settings.sync();
}

void loadEditorSettings(QSettings &settings, d3edit_state &state)
{
  settings.beginGroup(QStringLiteral("editor"));

  state.texdlg_texture    = settings.value(QStringLiteral("texdlg_texture"),   0).toInt();
  state.current_obj_type  = settings.value(QStringLiteral("current_obj_type"), 0).toInt();
  state.current_obj_id    = settings.value(QStringLiteral("current_obj_id"),   0).toInt();
  state.current_powerup   = settings.value(QStringLiteral("current_powerup"),  0).toInt();
  state.current_door      = settings.value(QStringLiteral("current_door"),     0).toInt();
  state.current_robot     = settings.value(QStringLiteral("current_robot"),    0).toInt();
  state.current_ship      = settings.value(QStringLiteral("current_ship"),     0).toInt();
  state.current_sound     = settings.value(QStringLiteral("current_sound"),    0).toInt();
  state.current_weapon    = settings.value(QStringLiteral("current_weapon"),   0).toInt();
  state.current_path      = settings.value(QStringLiteral("current_path"),     0).toInt();
  state.current_node      = settings.value(QStringLiteral("current_node"),     0).toInt();
  state.current_megacell  = settings.value(QStringLiteral("current_megacell"), 0).toInt();
  state.current_building  = settings.value(QStringLiteral("current_building"), 0).toInt();
  state.current_clutter   = settings.value(QStringLiteral("current_clutter"),  0).toInt();

  state.texscr_visible    = settings.value(QStringLiteral("texscr_visible"),    false).toBool();
  state.texscr_x          = settings.value(QStringLiteral("texscr_x"),          0).toInt();
  state.texscr_y          = settings.value(QStringLiteral("texscr_y"),          0).toInt();
  state.texscr_w          = settings.value(QStringLiteral("texscr_w"),          0).toInt();
  state.texscr_h          = settings.value(QStringLiteral("texscr_h"),          0).toInt();

  state.wirescr_visible   = settings.value(QStringLiteral("wirescr_visible"),   false).toBool();
  state.wirescr_x         = settings.value(QStringLiteral("wirescr_x"),         0).toInt();
  state.wirescr_y         = settings.value(QStringLiteral("wirescr_y"),         0).toInt();
  state.wirescr_w         = settings.value(QStringLiteral("wirescr_w"),         0).toInt();
  state.wirescr_h         = settings.value(QStringLiteral("wirescr_h"),         0).toInt();

  state.keypad_visible    = settings.value(QStringLiteral("keypad_visible"),    false).toBool();
  state.keypad_current    = settings.value(QStringLiteral("keypad_current"),    0).toInt();

  state.float_keypad_x    = settings.value(QStringLiteral("float_keypad_x"),    -1).toInt();
  state.float_keypad_y    = settings.value(QStringLiteral("float_keypad_y"),    -1).toInt();
  state.float_keypad_w    = settings.value(QStringLiteral("float_keypad_w"),    -1).toInt();
  state.float_keypad_h    = settings.value(QStringLiteral("float_keypad_h"),    -1).toInt();

  state.objmodeless_x     = settings.value(QStringLiteral("objmodeless_x"),     0).toInt();
  state.objmodeless_y     = settings.value(QStringLiteral("objmodeless_y"),     0).toInt();
  state.objmodeless_on    = settings.value(QStringLiteral("objmodeless_on"),    false).toBool();

  state.tile_views        = settings.value(QStringLiteral("tile_views"),        false).toBool();
  state.game_render_mode  = settings.value(QStringLiteral("game_render_mode"),  0).toInt();

  state.terrain_dots            = settings.value(QStringLiteral("terrain_dots"),            true).toBool();
  state.terrain_flat_shade      = settings.value(QStringLiteral("terrain_flat_shade"),      false).toBool();

  state.randomize_megacell      = settings.value(QStringLiteral("randomize_megacell"),      false).toBool();
  state.box_selection_mode      = settings.value(QStringLiteral("box_selection_mode"),      0).toInt();
  state.object_move_mode        = settings.value(QStringLiteral("object_move_mode"),        0).toInt();
  state.object_move_axis        = settings.value(QStringLiteral("object_move_axis"),        0).toInt();
  state.fullscreen_debug_state  = settings.value(QStringLiteral("fullscreen_debug_state"),  false).toBool();

  state.texture_display_flags   = settings.value(QStringLiteral("texture_display_flags"),   0).toInt();

  Slew_key_speed                = settings.value(QStringLiteral("slew_key_speed"),          1.0f).toFloat();
  state.joy_slewing             = settings.value(QStringLiteral("joy_slewing"),             false).toBool();
  state.objects_in_wireframe    = settings.value(QStringLiteral("objects_in_wireframe"),    true).toBool();

  settings.endGroup();
}
