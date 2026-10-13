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
#include <utility>


// Field set mirrors SaveEditorSettings/LoadEditorSettings in editor.cpp.
// We deliberately use string key names that match the Win32 record names
// (e.g. "texdlg_texture") so a user migrating from the original editor
// keeps their settings.

void saveEditorSettings(QSettings &settings, const d3edit_state &state) {
  settings.beginGroup(QStringLiteral("editor"));

  auto setval = [&settings]<typename T>(const char* const name, std::optional<T> val) {
    if (!val) {
      // Store an invalid QVariant rather than a sentinel like -1: it round-trips
      // through the INI as @Invalid() and reads back as null, which is what
      // getval() maps to nullopt. A -1 would instead come back as 4294967295 for
      // the unsigned targets and read as a bogus-but-present id.
      settings.setValue(name, QVariant());
      return;
    }
    if constexpr (std::is_enum_v<T>) {
      settings.setValue(name, std::to_underlying(*val));
    } else {
      settings.setValue(name, *val);
    }
  };

  setval("texdlg_texture",      state.texdlg_texture);
  setval("current_obj_type",    state.obj_page);
  setval("current_powerup",     state.current_powerup);
  setval("current_door",        state.current_door);
  setval("current_robot",       state.current_robot);
  setval("current_ship",        state.current_ship);
  setval("current_sound",       state.current_sound);
  setval("current_weapon",      state.current_weapon);
  setval("current_path",        state.current_path);
  setval("current_node",        state.current_node);
  setval("current_megacell",    state.current_megacell);
  setval("current_building",    state.current_building);
  setval("current_clutter",     state.current_clutter);
  setval("keypad_current",      state.keypad_current);
  setval("game_render_mode",    state.game_render_mode);
  setval("box_selection_mode",  state.box_selection_mode);
  setval("object_move_mode",    state.object_move_mode);
  setval("object_move_axis",    state.object_move_axis);

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
  settings.setValue(QStringLiteral("float_keypad_x"),        state.float_keypad_x);
  settings.setValue(QStringLiteral("float_keypad_y"),        state.float_keypad_y);
  settings.setValue(QStringLiteral("float_keypad_w"),        state.float_keypad_w);
  settings.setValue(QStringLiteral("float_keypad_h"),        state.float_keypad_h);
  settings.setValue(QStringLiteral("objmodeless_x"),         state.objmodeless_x);
  settings.setValue(QStringLiteral("objmodeless_y"),         state.objmodeless_y);
  settings.setValue(QStringLiteral("objmodeless_on"),        state.objmodeless_on);
  settings.setValue(QStringLiteral("tile_views"),            state.tile_views);
  settings.setValue(QStringLiteral("terrain_dots"),          state.terrain_dots);
  settings.setValue(QStringLiteral("terrain_flat_shade"),    state.terrain_flat_shade);
  settings.setValue(QStringLiteral("randomize_megacell"),    state.randomize_megacell);
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

  auto getval = [&settings]<typename Ret>(const char* const name) -> std::optional<Ret> {
    QVariant val = settings.value(name);
    if (val.isNull())
      return std::nullopt;
    if constexpr (std::is_enum_v<Ret>) {
      // Fetch the underlying type (e.g., int/uint), then cast back to the enum
      return static_cast<Ret>(val.value<std::underlying_type_t<Ret>>());
    } else {
      return val.value<Ret>();
    }
  };

  state.texdlg_texture      = getval.template operator()<uint32_t>("texdlg_texture");
  state.obj_page            = getval.template operator()<object_type_e>("current_obj_type");
  state.current_powerup     = getval.template operator()<uint16_t>("current_powerup");
  state.current_door        = getval.template operator()<uint32_t>("current_door");
  state.current_robot       = getval.template operator()<uint16_t>("current_robot");
  state.current_ship        = getval.template operator()<uint32_t>("current_ship");
  state.current_sound       = getval.template operator()<uint32_t>("current_sound");
  state.current_weapon      = getval.template operator()<uint32_t>("current_weapon");
  state.current_path        = getval.template operator()<uint32_t>("current_path");
  state.current_node        = getval.template operator()<uint16_t>("current_node");
  state.current_megacell    = getval.template operator()<uint32_t>("current_megacell");
  state.current_building    = getval.template operator()<uint16_t>("current_building");
  state.current_clutter     = getval.template operator()<uint16_t>("current_clutter");
  state.keypad_current      = getval.template operator()<uint32_t>("keypad_current");
  state.game_render_mode    = getval.template operator()<uint32_t>("game_render_mode");
  state.box_selection_mode  = getval.template operator()<uint32_t>("box_selection_mode");
  state.object_move_mode    = getval.template operator()<uint32_t>("object_move_mode");
  state.object_move_axis    = getval.template operator()<uint32_t>("object_move_axis");


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


  state.float_keypad_x    = settings.value(QStringLiteral("float_keypad_x"),    -1).toInt();
  state.float_keypad_y    = settings.value(QStringLiteral("float_keypad_y"),    -1).toInt();
  state.float_keypad_w    = settings.value(QStringLiteral("float_keypad_w"),    -1).toInt();
  state.float_keypad_h    = settings.value(QStringLiteral("float_keypad_h"),    -1).toInt();
  state.objmodeless_x     = settings.value(QStringLiteral("objmodeless_x"),     0).toInt();
  state.objmodeless_y     = settings.value(QStringLiteral("objmodeless_y"),     0).toInt();
  state.objmodeless_on    = settings.value(QStringLiteral("objmodeless_on"),    false).toBool();
  state.tile_views        = settings.value(QStringLiteral("tile_views"),        false).toBool();
  state.terrain_dots            = settings.value(QStringLiteral("terrain_dots"),            true).toBool();
  state.terrain_flat_shade      = settings.value(QStringLiteral("terrain_flat_shade"),      false).toBool();
  state.randomize_megacell      = settings.value(QStringLiteral("randomize_megacell"),      false).toBool();
  state.fullscreen_debug_state  = settings.value(QStringLiteral("fullscreen_debug_state"),  false).toBool();
  state.texture_display_flags   = settings.value(QStringLiteral("texture_display_flags"),   0).toInt();
  Slew_key_speed                = settings.value(QStringLiteral("slew_key_speed"),          1.0f).toFloat();
  state.joy_slewing             = settings.value(QStringLiteral("joy_slewing"),             false).toBool();
  state.objects_in_wireframe    = settings.value(QStringLiteral("objects_in_wireframe"),    true).toBool();

  settings.endGroup();
}
