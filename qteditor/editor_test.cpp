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

// QTest based editor tester: walks every dialog, simulates every widget and
// verifies enabled/disabled states match the Win32 editor behaviour (e.g.
// options that should only be possible once a level is loaded).

#include <QtTest/QtTest>

#include <QAbstractButton>
#include <QComboBox>
#include <QLineEdit>
#include <QSlider>

#include "d3_editor_init.h"
#include "d3edit.h"
#include "door.h"
#include "gamepath.h"
#include "manage.h"
#include "object.h"
#include "room_external.h"
#include "ship.h"
#include "ssl_lib.h"
#include "terrain.h"
#include "trigger.h"
#include "weapon.h"

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
#include "splash_dialog.h"
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

namespace {

struct DialogInstance {
  QString name;
  QWidget *handle;
};

QList<DialogInstance> g_dialogs;

void addDialog(const char *name, QWidget *handle) { g_dialogs.append({name, handle}); }

void collectInteractive(QWidget *root, QList<QWidget *> *out) {
  const QList<QWidget *> all = root->findChildren<QWidget *>();
  for (QWidget *w : all) {
    if (qobject_cast<QAbstractButton *>(w) || qobject_cast<QLineEdit *>(w) || qobject_cast<QComboBox *>(w) ||
        qobject_cast<QSlider *>(w)) {
      out->append(w);
    }
  }
}

// Schedules any modal dialog that appears to be closed shortly after a click,
// so interaction tests don't deadlock on exec() calls.
void closeModalsSoon() {
  QTimer::singleShot(15, []() {
    for (QWidget *w : QApplication::topLevelWidgets()) {
      if (w->isModal() && w->isVisible())
        w->close();
    }
  });
}

QString widgetDesc(QWidget *w) {
  return QString("%1(%2)").arg(w->metaObject()->className()).arg(w->objectName());
}

} // namespace

class EditorTest : public QObject {
  Q_OBJECT

private slots:
  void initTestCase() {
    QCoreApplication::processEvents();
  }

  void testDialogsConstruct() {
    death_info di{};
    object_info oi{};
    soundsource_info_s ssi{};
    physics_info pi{};
    level_info li{};

    auto make = [&](const char *name, QWidget *w) { addDialog(name, w); };

    make("about", (new QtEditor::AboutDialog)->handle());
    make("addscript", (new QtEditor::AddScriptDialog)->handle());
    {
      auto *d = new QtEditor::BriefMissionFlagsDialog(0, 0);
      make("brief_mission_flags", d->handle());
    }
    {
      auto *d = new QtEditor::BriefTextEditDialog(0, nullptr, 0);
      make("brief_text_edit", d->handle());
    }
    make("createscript", (new QtEditor::CreateNewScriptDialog)->handle());
    make("customize_object", (new QtEditor::CustomObjectDialog)->handle());
    make("dallas_generic_prompt", (new QtEditor::DallasGenericPromptDialog)->handle());
    make("dallas_vector_prompt", (new QtEditor::DallasVectorPromptDialog)->handle());
    {
      auto *d = new QtEditor::DeathDialog(&di);
      make("death", d->handle());
    }
    {
      auto *d = new QtEditor::EditLineDialog("Test", nullptr);
      make("editline", d->handle());
    }
    {
      auto *d = new QtEditor::GenericDeathDialog(&oi);
      make("generic_death", d->handle());
    }
    make("hog", (new QtEditor::HogDialog)->handle());
    {
      auto *d = new QtEditor::LevelInfoDialog(&li);
      make("level_info", d->handle());
    }
    {
      auto *d = new QtEditor::PhysicsDialog(&pi);
      make("physics", d->handle());
    }
    {
      auto *d = new QtEditor::PlayerWeaponsDialog(0);
      make("player_weapons", d->handle());
    }
    make("powprop", (new QtEditor::PowerupPropDialog)->handle());
    make("preferences", (new QtEditor::PreferencesDialog)->handle());
    make("propai", (new QtEditor::PropertyAIDialog)->handle());
    make("selectrange", (new QtEditor::SelectRangeDialog)->handle());
    {
      auto *d = new QtEditor::SoundSourceDialog(&ssi);
      make("sound_source", d->handle());
    }
    make("splash", (new QtEditor::SplashDialog)->handle());
    make("status", (new QtEditor::StatusDialog)->handle());
    make("terrain_sound", (new QtEditor::TerrainSoundDialog)->handle());
    make("viewer_prop", (new QtEditor::ViewerPropDialog)->handle());
    make("world_objects_door", (new QtEditor::WorldObjectsDoorDialog)->handle());
    {
      auto *d = new QtEditor::WorldObjectsGenericDialog(OBJ_BUILDING, 0);
      make("world_objects_generic", d->handle());
    }
    make("world_objects_player", (new QtEditor::WorldObjectsPlayerDialog)->handle());
    make("world_sounds", (new QtEditor::WorldSoundsDialog)->handle());
    make("world_textures", (new QtEditor::WorldTexturesDialog)->handle());
    make("world_weapons", (new QtEditor::WorldWeaponsDialog)->handle());
    make("world_objects_light", (new QtEditor::WorldObjectsLightDialog)->handle());

    make("keypad_megacell", (new QtEditor::MegacellKeypad)->handle());
    make("keypad_doorway", (new QtEditor::DoorwayKeypad)->handle());
    make("keypad_trigger", (new QtEditor::TriggerKeypad)->handle());
    make("keypad_path", (new QtEditor::PathKeypad)->handle());

    for (const DialogInstance &d : g_dialogs)
      QVERIFY2(d.handle != nullptr, qPrintable("dialog failed to load: " + d.name));
  }

  // The Win32 editor only enables room/object/viewer editing once a level is
  // loaded. This test runs with no level, so those controls must be disabled.
  void testLevelGatedEnabledStates() {
    const bool levelLoaded = (Curroomp != nullptr);

    for (const DialogInstance &d : g_dialogs) {
      if (d.name.startsWith("keypad_doorway")) {
        QList<QWidget *> ws;
        collectInteractive(d.handle, &ws);
        for (QWidget *w : ws) {
          const QString oname = w->objectName();
          const bool editing = oname.startsWith("IDC_KEY") || oname.startsWith("IDC_DOORWAY");
          if (editing)
            QVERIFY2(w->isEnabled() == levelLoaded,
                     qPrintable(QString("doorway %1 enabled=%2 levelLoaded=%3")
                                    .arg(widgetDesc(w))
                                    .arg(w->isEnabled())
                                    .arg(levelLoaded)));
        }
      }

      if (d.name.startsWith("keypad_trigger")) {
        QList<QWidget *> ws;
        collectInteractive(d.handle, &ws);
        const bool hasTriggers = Num_triggers > 0 && levelLoaded;
        for (QWidget *w : ws) {
          if (w->objectName().startsWith("IDC_TRIG"))
            QVERIFY2(w->isEnabled() == hasTriggers,
                     qPrintable(QString("trigger %1 enabled=%2 hasTriggers=%3")
                                    .arg(widgetDesc(w))
                                    .arg(w->isEnabled())
                                    .arg(hasTriggers)));
        }
      }

      if (d.name.startsWith("keypad_path")) {
        QList<QWidget *> ws;
        collectInteractive(d.handle, &ws);
        const bool hasPaths = Num_game_paths > 0 && levelLoaded;
        for (QWidget *w : ws) {
          if (w->objectName().startsWith("IDC_PATHPAD") || w->objectName().startsWith("IDC_DELETE_PATH"))
            QVERIFY2(w->isEnabled() == hasPaths,
                     qPrintable(QString("path %1 enabled=%2 hasPaths=%3")
                                    .arg(widgetDesc(w))
                                    .arg(w->isEnabled())
                                    .arg(hasPaths)));
        }
      }

      if (d.name.startsWith("viewer_prop")) {
        QList<QWidget *> ws;
        collectInteractive(d.handle, &ws);
        const bool hasViewer = (Viewer_object != nullptr);
        for (QWidget *w : ws) {
          if (w->objectName().startsWith("IDC_"))
            QVERIFY2(w->isEnabled() == hasViewer,
                     qPrintable(QString("viewer %1 enabled=%2 hasViewer=%3")
                                    .arg(widgetDesc(w))
                                    .arg(w->isEnabled())
                                    .arg(hasViewer)));
        }
      }
    }
  }

  // The Win32 table editors gate lock/checkin/undo/override operations on the
  // network; without a level they are also gated on the lock state. Add buttons
  // vary per dialog (some are enabled and just error out), so only assert the
  // operations the Win32 code disables unconditionally when Network_up is 0.
  void testTableEnabledStates() {
    const bool net = Network_up != 0;

    for (const DialogInstance &d : g_dialogs) {
      QList<QWidget *> ws;
      collectInteractive(d.handle, &ws);
      for (QWidget *w : ws) {
        const QString oname = w->objectName();
        const bool isLock = oname.contains("LOCK", Qt::CaseInsensitive);
        const bool isCheckin = oname.contains("CHECKIN", Qt::CaseInsensitive);
        const bool isUndoLock = oname.contains("UNDO", Qt::CaseInsensitive);
        const bool isOverride = oname.contains("OVERRIDE", Qt::CaseInsensitive);
        const bool isCheckedOut = oname.contains("OUT", Qt::CaseInsensitive);
        const bool networkOp = isLock || isCheckin || isUndoLock || isOverride || isCheckedOut;
        if (networkOp) {
          if (!net)
            QVERIFY2(!w->isEnabled(), qPrintable(QString("%1 %2 should be disabled (no network)")
                                                     .arg(d.name)
                                                     .arg(widgetDesc(w))));
        }
      }
    }
  }

  void testInteractEveryWidget() {
    int clicks = 0;
    int edits = 0;
    for (const DialogInstance &d : g_dialogs) {
      QList<QWidget *> ws;
      collectInteractive(d.handle, &ws);
      for (QWidget *w : ws) {
        const QString name = w->objectName();
        if (name.isEmpty())
          continue;

        if (QAbstractButton *b = qobject_cast<QAbstractButton *>(w)) {
          qInfo() << "click" << d.name << name;
          closeModalsSoon();
          QTest::mouseClick(b, Qt::LeftButton);
          QCoreApplication::processEvents();
          QCoreApplication::processEvents();
          clicks++;
        } else if (QLineEdit *e = qobject_cast<QLineEdit *>(w)) {
          qInfo() << "edit" << d.name << name;
          QTest::keyClicks(e, "1");
          QTest::keyClick(e, Qt::Key_Return);
          edits++;
        } else if (QComboBox *c = qobject_cast<QComboBox *>(w)) {
          qInfo() << "combo" << d.name << name;
          if (c->count() > 0)
            c->setCurrentIndex(0);
        }
        // Fail loudly if the widget was destroyed by its own handler.
        QVERIFY2(d.handle->findChild<QWidget *>(name) != nullptr,
                 qPrintable(QString("widget deleted during interaction: %1 %2").arg(d.name).arg(name)));
      }
    }
    qInfo() << "interacted with" << clicks << "buttons," << edits << "edits";
  }
};

// Custom main: initialise the D3 core (loads game data) before running tests.
int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QtEditor::initD3Core(argc, argv);
  EditorTest tc;
  return QTest::qExec(&tc, argc, argv);
}
#include "editor_test.moc"
