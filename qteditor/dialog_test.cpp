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

#include "dialog_test.h"

#include <cstdio>

#include "about_dialog.h"
#include "addscript_dialog.h"
#include "brief_mission_flags_dialog.h"
#include "brief_text_edit_dialog.h"
#include "createscript_dialog.h"
#include "customize_object_dialog.h"
#include "dallas_prompt_dialogs.h"
#include "death_dialog.h"
#include "doorway_keypad.h"
#include "editline_dialog.h"
#include "generic_death_dialog.h"
#include "hog_dialog.h"
#include "level_info_dialog.h"
#include "megacell_keypad.h"
#include "path_keypad.h"
#include "physics_dialog.h"
#include "player_weapons_dialog.h"
#include "powprop_dialog.h"
#include "preferences_dialog.h"
#include "propai_dialog.h"
#include "robot_weapons_dialog.h"
#include "selectrange_dialog.h"
#include "sound_source_dialog.h"
#include "status_dialog.h"
#include "terrain_sound_dialog.h"
#include "trigger_keypad.h"
#include "viewer_prop_dialog.h"
#include "world_objects_door_dialog.h"
#include "world_objects_generic_dialog.h"
#include "world_objects_player_dialog.h"
#include "world_sounds_dialog.h"
#include "world_textures_dialog.h"
#include "world_weapons_dialog.h"
#include "worldobjectslight_dialog.h"

namespace QtEditor {

namespace {

int failures = 0;
int total = 0;

void check(const char *name, bool ok) {
  total++;
  if (!ok)
    failures++;
  fprintf(stderr, "DIALOG-TEST %-28s %s\n", name, ok ? "OK" : "FAIL");
}

template <typename T>
void construct(const char *name, QWidget *parent) {
  try {
    T dlg(parent);
    check(name, dlg.handle() != nullptr);
  } catch (...) {
    check(name, false);
  }
}

} // namespace

int runDialogTest() {
  death_info di{};
  object_info oi{};
  soundsource_info_s ssi{};
  physics_info pi{};
  level_info li{};

  construct<AboutDialog>("about", nullptr);
  construct<AddScriptDialog>("addscript", nullptr);
  {
    BriefMissionFlagsDialog dlg(0, 0);
    check("brief_mission_flags", dlg.handle() != nullptr);
  }
  {
    BriefTextEditDialog dlg(0, nullptr, 0);
    check("brief_text_edit", dlg.handle() != nullptr);
  }
  construct<CreateNewScriptDialog>("createscript", nullptr);
  construct<CustomObjectDialog>("customize_object", nullptr);
  construct<DallasGenericPromptDialog>("dallas_generic_prompt", nullptr);
  construct<DallasVectorPromptDialog>("dallas_vector_prompt", nullptr);
  {
    DeathDialog dlg(&di);
    check("death", dlg.handle() != nullptr);
  }
  {
    EditLineDialog dlg("Test", nullptr);
    check("editline", dlg.handle() != nullptr);
  }
  {
    GenericDeathDialog dlg(&oi);
    check("generic_death", dlg.handle() != nullptr);
  }
  construct<HogDialog>("hog", nullptr);
  {
    LevelInfoDialog dlg(&li);
    check("level_info", dlg.handle() != nullptr);
  }
  {
    PhysicsDialog dlg(&pi);
    check("physics", dlg.handle() != nullptr);
  }
  {
    PlayerWeaponsDialog dlg(0);
    check("player_weapons", dlg.handle() != nullptr);
  }
  construct<PowerupPropDialog>("powprop", nullptr);
  construct<PreferencesDialog>("preferences", nullptr);
  construct<PropertyAIDialog>("propai", nullptr);
  construct<SelectRangeDialog>("selectrange", nullptr);
  {
    SoundSourceDialog dlg(&ssi);
    check("sound_source", dlg.handle() != nullptr);
  }
  construct<StatusDialog>("status", nullptr);
  construct<TerrainSoundDialog>("terrain_sound", nullptr);
  construct<ViewerPropDialog>("viewer_prop", nullptr);
  construct<WorldObjectsDoorDialog>("world_objects_door", nullptr);
  {
    WorldObjectsGenericDialog dlg(OBJ_BUILDING, 0);
    check("world_objects_generic", dlg.handle() != nullptr);
  }
  construct<WorldObjectsPlayerDialog>("world_objects_player", nullptr);
  construct<WorldSoundsDialog>("world_sounds", nullptr);
  construct<WorldTexturesDialog>("world_textures", nullptr);
  construct<WorldWeaponsDialog>("world_weapons", nullptr);
  construct<WorldObjectsLightDialog>("world_objects_light", nullptr);

  // Keypads.
  construct<MegacellKeypad>("keypad_megacell", nullptr);
  construct<DoorwayKeypad>("keypad_doorway", nullptr);
  construct<TriggerKeypad>("keypad_trigger", nullptr);
  construct<PathKeypad>("keypad_path", nullptr);

  fprintf(stderr, "DIALOG-TEST total=%d failures=%d\n", total, failures);
  return failures;
}

}
