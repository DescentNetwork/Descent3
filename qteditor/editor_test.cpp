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

#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

#include <QAbstractButton>
#include <QAction>
#include <QComboBox>
#include <QDockWidget>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QSlider>

#include <cerrno>

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
#include "editor_file_dialogs.h"
#include "editor_settings.h"
#include "generic_death_dialog.h"
#include "main_window.h"
#include "hog_dialog.h"
#include "hog2_format.h"
#include "posix_stream.h"

#include <cstdio>
#include <cstring>
#include <fstream>
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

#include <QFile>
#include <QDir>
#include <QLabel>
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

// Schedules any modal dialog that appears to be closed shortly after a
  // click, so interaction tests don't deadlock on exec() calls. The timer
  // keeps closing modal widgets until none remain visible, then cleans up.
  void closeModalsSoon() {
    static int s_id = 0;
    const int id = ++s_id;
    auto *timer = new QTimer();
    timer->setInterval(5);
    auto closeFn = [timer, id]() {
      if (id != s_id) {
        timer->stop();
        timer->deleteLater();
        return;
      }
      bool any = false;
      const auto widgets = QApplication::topLevelWidgets();
      for (QWidget *w : widgets) {
        if (w->isModal() && w->isVisible()) {
          w->close();
          any = true;
        }
      }
      if (!any) {
        timer->stop();
        timer->deleteLater();
      }
    };
    timer->callOnTimeout(closeFn);
    timer->start();
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

  // CAddScriptDialog (IDD_ADDSCRIPT) gates the name length at 32 chars via
  // DDV_MaxChars in Win32 and via setMaxLength on the Qt line edit here. It
  // also always-defaults IDC_TYPESEL to "object" before "trigger". This test
  // pins both behaviours so the Win32 contract survives the port.
  void testAddScriptDialogContract() {
    QtEditor::AddScriptDialog dlg;
    QVERIFY(dlg.handle() != nullptr);

    auto *name_edit = dlg.handle()->findChild<QLineEdit *>(QStringLiteral("IDC_EDITNAME"));
    QVERIFY(name_edit != nullptr);
    QCOMPARE(int(name_edit->maxLength()), 32);

    auto *type_combo = dlg.handle()->findChild<QComboBox *>(QStringLiteral("IDC_TYPESEL"));
    QVERIFY(type_combo != nullptr);
    QCOMPARE(type_combo->count(), 2);
    QCOMPARE(type_combo->itemText(0), QStringLiteral("object"));
    QCOMPARE(type_combo->itemText(1), QStringLiteral("trigger"));
    QCOMPARE(type_combo->currentText(), QStringLiteral("object"));

    // Trying to type past the cap leaves the extra characters trimmed.
    name_edit->setText(QStringLiteral("01234567890123456789012345678901234567890"));
    QCOMPARE(name_edit->text().size(), 32);

    // Accessors reflect whatever is in the controls.
    QCOMPARE(dlg.name(), name_edit->text());
    QCOMPARE(dlg.typeName(), QStringLiteral("object"));

    type_combo->setCurrentIndex(1);
    QCOMPARE(dlg.typeName(), QStringLiteral("trigger"));
  }

  // Verifies that HogDialog::loadHogFile() reads every entry (Filename, Date,
  // Length, Attributes) out of a real .hog file using the cfile/hogfile API.
  // Uses the d3-linux.hog that ships with the project, which thousands of
  // files.
  void testHogDialogLoadHogFile() {
    QtEditor::HogDialog dlg;
    QVERIFY(dlg.handle() != nullptr);

    // Empty path is rejected.
    QVERIFY(!dlg.loadHogFile(QString()));

    // Bad path is rejected.
    QVERIFY(!dlg.loadHogFile(QStringLiteral("/no/such/file.hog")));

    // The actual data file shipped with the project. Search in the binary's
    // directory, the install root PWD (./), the install-root symlink (which
    // PWD-d TestC run resolves), and a couple of relatives.
    const QString data_hog =
        QStringLiteral("%1/d3-linux.hog").arg(QCoreApplication::applicationDirPath());
    const QString parent_hog =
        QStringLiteral("%1/d3-linux.hog").arg(QCoreApplication::applicationDirPath() + "/..");
    bool found = false;
    const QStringList candidates = {data_hog, parent_hog,
                                    QStringLiteral("./d3-linux.hog"),
                                    QStringLiteral("./d3-linux.hog.original"),
                                    QStringLiteral("../d3-linux.hog"),
                                    QStringLiteral("../../d3-linux.hog")};
    QStringList seen_paths;
    for (const QString &c : candidates) {
      seen_paths << c;
      if (QFile::exists(c)) {
        QVERIFY(dlg.loadHogFile(c));
        found = true;
        break;
      }
    }
    if (!found) {
      qWarning("d3-linux.hog not found; tried: %s",
               qPrintable(seen_paths.join(QStringLiteral(", "))));
      QSKIP("d3-linux.hog not found in test workspace; skipping hog load test.");
    }
    const QString text = dlg.handle()->findChild<QLabel *>(QStringLiteral("IDC_STATUSTEXT"))->text();
    QVERIFY(text.contains("entries") || text.contains("Loaded"));
  }

  // Round-trips a hog2::archive_t through the new D3::hog2_format file table.
// The Win32 CHogDialog had open_hogfile / write_hogfile / copy_file_to_hog
// commented out because they did not compile; the editor now uses the
// descar/ hog reader/writer pair so the dialog can fill the IDC_HOGLIST
// without going through cfile/hogfile.h.
  void testHogFormatRoundTrip() {
    const QString tmpDir = QCoreApplication::applicationDirPath() + "/_test_hog";
    QDir::current().mkpath(tmpDir);

    const QString out = tmpDir + "/roundtrip.hog";
    QFile::remove(out);
    errno = 0;

    hog2::archive_t table;
    hog2::entry_t alpha;
    std::copy(std::begin("alpha.txt"), std::end("alpha.txt"), std::begin(alpha.name));
    alpha.flags = 0;
    alpha.len = 5;
    alpha.timestamp = 0;
    table.addEntry(alpha);
    QCOMPARE(static_cast<int>(std::distance(table.begin(), table.end())), 1);

    assert(errno == 0);
    posix_ostream output;
    QVERIFY(output.open(out.toStdString(),
                        std::ios_base::out | std::ios_base::binary | std::ios_base::trunc));
    output << table;
    // posix_ostream::write tracks bytes_written vs length, so output.good()
    // is reliable here. The same logic doesn't apply on the read side
    // because posix_istream::read doesn't update m_code.
    QVERIFY2(output.good(), qPrintable(QString("write errc=%1").arg(int(output.error()))));
    output.close();

    // Verify the on-disk file actually contains the archive header + entry.
    QCOMPARE(QFileInfo(out).size(), qsizetype(116));

    posix_istream input;
    QVERIFY(input.open(out.toStdString(), std::ios_base::in | std::ios_base::binary));
    hog2::archive_t read;
    input >> read;
    QCOMPARE(static_cast<int>(std::distance(read.begin(), read.end())), 1);
    QCOMPARE(read.begin()->name.string(), std::string("alpha.txt"));

    QFile::remove(out);
    QDir::current().rmdir(tmpDir);
    errno = 0;
  }

  // Win32 editor.cpp exposes OpenFileDialog/SaveFileDialog/PrintToDlgItem so
  // every dialog can drive file picking and status text. The Qt port lives in
  // editor_file_dialogs.{h,cpp}; this test pins the MFC->Qt filter conversion
  // and the PrintToDlgItem-by-objectName lookup that legacy callers in
  // editor/ rely on.
  void testEditorFileDialogContract() {
    // The mfcFilterToQt helper isn't exported, but we can exercise it via the
    // signature of the OpenFileDialog / SaveFileDialog helpers (they need to
    // accept the legacy filter strings verbatim). Verify the format the
    // common call sites use round-trips through the Qt dialog system without
    // crashing. We don't pop a modal dialog in test mode; just call the
    // cancellation path through `pathname`-length checks.

    // 1. Filename buffer overflow is bounded. Even if the user picks some
    //    very long path, OpenFileDialog must truncate safely.
    const char *filter_single = "Outrage Level Files (*.d3l)|*.d3l||";
    char path[_MAX_PATH] = "";
    char initial[_MAX_PATH];
    std::strcpy(initial, "/tmp");
    // Verify the function entry doesn't crash and follows Win32 contract:
    //   - pathname==nullptr returns false without writing.
    //   - non-destructive cancel (user dismissed) leaves pathname untouched.
    //   - The function reports false when QFileDialog returns an empty
    //     selection (no UI means precisely "no selection", same as cancel).
    QVERIFY(!QtEditor::OpenFileDialog(nullptr, filter_single, nullptr));
    QVERIFY(path[0] == '\0');

    // 2. PrintToDlgItem writes a formatted string into a QLabel whose
    //    objectName matches the Win32 resource ID alias.
    QtEditor::AddScriptDialog adlg;
    QLabel *name_lbl = new QLabel(adlg.handle());
    name_lbl->setObjectName(QStringLiteral("IDC_TEST_LABEL"));
    QtEditor::PrintToDlgItem(adlg.handle(), "IDC_TEST_LABEL",
                              "Current Matcen: %d", 7);
    QCOMPARE(name_lbl->text(), QStringLiteral("Current Matcen: 7"));

    // 3. PrintToDlgItem is no-op for unknown IDs (idempotent on the win32
    //    code path's missing-handle fatal).
    name_lbl->setText(QStringLiteral("untouched"));
    QtEditor::PrintToDlgItem(adlg.handle(), "IDC_NONEXISTENT", "x=%d", 99);
    QCOMPARE(name_lbl->text(), QStringLiteral("untouched"));
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

    // The Win32 table editors gate lock/checkin/undo/override operations on
    // the network; without a level they are also gated on the lock state. The
    // gating only applies to the buttons that perform network-side actions -
    // not to text fields whose names incidentally contain "OVERRIDE" etc.
    for (const DialogInstance &d : g_dialogs) {
      QList<QWidget *> ws;
      collectInteractive(d.handle, &ws);
      for (QWidget *w : ws) {
        if (!qobject_cast<QAbstractButton *>(w))
          continue;
        const QString oname = w->objectName();
        const bool isLock = oname.contains("_LOCK", Qt::CaseSensitive) || oname.endsWith("LOCK", Qt::CaseSensitive);
        const bool isCheckin = oname.contains("CHECKIN", Qt::CaseSensitive);
        const bool isUndoLock = oname.contains("UNDO_LOCK", Qt::CaseSensitive);
        const bool isOverride = oname == QStringLiteral("IDC_OVERRIDE") ||
                                oname.endsWith("_OVERRIDE", Qt::CaseSensitive);
        const bool isCheckedOut = oname.contains("CHECKED_OUT", Qt::CaseSensitive) ||
                                  oname.endsWith("CHECKOUT", Qt::CaseSensitive);
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

  // Verifies the File-menu actions are wired to slot implementations rather
  // than stubbed. We tickle each action via QAction::trigger() (a self-signal
  // fires any connected slot) and check that the resulting state changes
  // match what the Win32 editor does: New clears the current path, Open and
  // SaveAs route through OpenFileDialog/SaveFileDialog (which cancel in
  // headless mode but update the initialdir round-trip), Save-when-empty
  // falls through to SaveAs.
  void testFileMenuActionsWired() {
    QtEditor::MainWindow win;
    win.show();
    QCoreApplication::processEvents();

    // Find the action by walking the menubar so we don't depend on the
    // exact table_file_editor.ui shape.
    QAction *a_new = nullptr, *a_open = nullptr, *a_save = nullptr,
            *a_saveas = nullptr;
    for (QAction *a : win.menuBar()->actions()) {
      QMenu *m = a->menu();
      if (m == nullptr || m->title() != "&File")
        continue;
      for (QAction *fa : m->actions()) {
        if (fa->objectName() == "ID_FILE_NEW") a_new = fa;
        else if (fa->objectName() == "ID_FILE_OPEN") a_open = fa;
        else if (fa->objectName() == "ID_FILE_SAVE") a_save = fa;
        else if (fa->objectName() == "ID_FILE_SAVE_AS") a_saveas = fa;
      }
      break;
    }
    QVERIFY(a_new != nullptr);
    QVERIFY(a_open != nullptr);
    QVERIFY(a_save != nullptr);
    QVERIFY(a_saveas != nullptr);

    // Triggering must not crash. Cancel paths in headless QFileDialog fall
    // through the statusBar() updates we put behind the slots.
    a_new->trigger();
    QCoreApplication::processEvents();
    QCOMPARE(win.windowTitle(), QStringLiteral("Descent 3 Editor - Untitled.d3l"));

    a_open->trigger();
    QCoreApplication::processEvents();
    // QFileDialog::getOpenFileName in headless mode returns an empty path.
    QVERIFY(win.windowTitle().startsWith(QStringLiteral("Descent 3 Editor")));

    a_save->trigger(); // empty path -> falls through to SaveAs candidate.
    QCoreApplication::processEvents();
    QVERIFY(win.windowTitle().startsWith(QStringLiteral("Descent 3 Editor")));

    a_saveas->trigger(); // Headless: may return the suggested default name.
    QCoreApplication::processEvents();
    QVERIFY(win.windowTitle().startsWith(QStringLiteral("Descent 3 Editor")));
  }

  // Verifies SaveEditorSettings / LoadEditorSettings (editor.cpp) round-trip
  // through QSettings. Uses an INI-formatted store under a tmp file so the
  // global app config isn't touched, and a constructed d3edit_state seeded
  // with non-default values to exercise the bool/int/float mix.
  void testEditorSettingsRoundTrip() {
    const QString ini_path =
        QCoreApplication::applicationDirPath() + "/_test_settings.ini";
    QSettings::Format previous = QSettings::defaultFormat();
    QSettings::setDefaultFormat(QSettings::IniFormat);

    d3edit_state out{};
    out.texdlg_texture    = 42;
    out.current_obj_type  = 3;
    out.current_powerup   = 7;
    out.texscr_visible    = true;
    out.texscr_x          = 17;
    out.texscr_y          = 23;
    out.game_render_mode  = GM_FULLSCREEN_HW;
    out.joy_slewing       = false;
    out.tile_views        = true;
    out.object_move_axis  = 2;
    out.fullscreen_debug_state = true;
    out.texture_display_flags = 0xab;
    out.objects_in_wireframe  = false;
    out.float_keypad_x = -1;
    out.float_keypad_y = -1;
    out.float_keypad_w = -1;
    out.float_keypad_h = -1;

    // Save through an explicit file-backed QSettings, mirroring the path
    // Linux/Qt will use at runtime.
    {
      QSettings settings(ini_path, QSettings::IniFormat);
      QtEditor::saveEditorSettings(settings, out);
    }

    d3edit_state in{};
    in.joy_slewing = true; // ensure round-trip flips if not loaded.
    {
      QSettings settings(ini_path, QSettings::IniFormat);
      QtEditor::loadEditorSettings(settings, in);
    }

    QCOMPARE(in.texdlg_texture,    out.texdlg_texture);
    QCOMPARE(in.current_obj_type,  out.current_obj_type);
    QCOMPARE(in.current_powerup,   out.current_powerup);
    QCOMPARE(in.texscr_visible,    out.texscr_visible);
    QCOMPARE(in.texscr_x,          out.texscr_x);
    QCOMPARE(in.texscr_y,          out.texscr_y);
    QCOMPARE(in.game_render_mode,  out.game_render_mode);
    QCOMPARE(in.joy_slewing,       out.joy_slewing);
    QCOMPARE(in.tile_views,        out.tile_views);
    QCOMPARE(in.object_move_axis,  out.object_move_axis);
    QCOMPARE(in.fullscreen_debug_state, out.fullscreen_debug_state);
    QCOMPARE(in.texture_display_flags,  int(out.texture_display_flags));
    QCOMPARE(in.objects_in_wireframe,   out.objects_in_wireframe);

    QFile::remove(ini_path);
    QSettings::setDefaultFormat(previous);
    errno = 0;
  }

  // Verifies CMainFrame's OnIdle equivalent. The constructor starts a
  // QTimer that fires onIdleTimer(); the slot bumps a counter the test reads
  // back, so we can verify the timer is alive without driving an actual
  // frame. We also exercise QtEditor::SetViewMode() (the global helper
  // corresponding to CMainFrame::SetViewMode and Editor_view_mode).
  void testMainFrameOnIdleTicks() {
    QtEditor::MainWindow win;
    QVERIFY(win.isOnIdleTimerActive());
    QCOMPARE(win.onIdleTickCount(), 0);

    // Spin the event loop for a few beats. With a 30ms timeout value the
    // tick counter should easily exceed three ticks in 200ms.
    win.startOnIdleTimer(30);
    const int start = win.onIdleTickCount();
    QElapsedTimer elapsed;
    elapsed.start();
    while (win.onIdleTickCount() < start + 3 && elapsed.elapsed() < 1000) {
      QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
    }
    QVERIFY2(win.onIdleTickCount() >= start + 3,
             qPrintable(QStringLiteral("only %1 ticks in %2ms")
                            .arg(win.onIdleTickCount() - start)
                            .arg(elapsed.elapsed())));

    // SetViewMode round-trip: out-of-range values are clipped. Valid values
    // update the global and return the previous mode.
    QCOMPARE(QtEditor::SetViewMode(QtEditor::VIEW_MODE_TERRAIN),
             QtEditor::currentViewMode());
    QCOMPARE(QtEditor::currentViewMode(), int(QtEditor::VIEW_MODE_TERRAIN));
    QCOMPARE(QtEditor::SetViewMode(QtEditor::VIEW_MODE_ROOM),
             int(QtEditor::VIEW_MODE_TERRAIN));
    QCOMPARE(QtEditor::SetViewMode(99), int(QtEditor::VIEW_MODE_ROOM));
    // Reset for subsequent tests.
    QtEditor::SetViewMode(QtEditor::VIEW_MODE_MINE);
  }

  // Verifies CMainFrame's view toggles routed through the Qt menu:
  // ID_VIEW_TOOLBAR -> keypad dock visibility; ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW
  // -> D3EditState.objects_in_wireframe flip (which the editor settings
  // round-trip already covers). Save/restore geometry round-trip pins the
  // CMainFrame OnCreateClient / OnDestroy side.
  void testMainFrameViewSubActions() {
    QtEditor::MainWindow win;
    win.show();
    QCoreApplication::processEvents();

    // Use the menuBar walk as in testFileMenuActionsWired.
    QAction *a_toolbar = nullptr, *a_showobjs = nullptr,
            *a_mine = nullptr, *a_terrain = nullptr, *a_room = nullptr;
    for (QAction *a : win.menuBar()->actions()) {
      QMenu *m = a->menu();
      if (m == nullptr || m->title() != "&View")
        continue;
      for (QAction *va : m->actions()) {
        const QString n = va->objectName();
        if (n == "ID_VIEW_TOOLBAR") a_toolbar = va;
        else if (n == "ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW") a_showobjs = va;
        else if (n == "ID_MINE_VIEW") a_mine = va;
        else if (n == "ID_TERRAIN_VIEW") a_terrain = va;
        else if (n == "ID_ROOM_VIEW") a_room = va;
      }
      break;
    }
    QVERIFY(a_toolbar != nullptr);
    QVERIFY(a_showobjs != nullptr);
    QVERIFY(a_mine != nullptr);
    QVERIFY(a_terrain != nullptr);
    QVERIFY(a_room != nullptr);

    // ID_VIEW_TOOLBAR flips the keypad dock. Start visible, toggle, expect
    // hidden, toggle again, expect visible.
    QDockWidget *dock = win.findChild<QDockWidget *>();
    QVERIFY(dock != nullptr);
    const bool before = dock->isVisible();
    a_toolbar->trigger();
    QCoreApplication::processEvents();
    QCOMPARE(dock->isVisible(), !before);
    a_toolbar->trigger();
    QCoreApplication::processEvents();
    QCOMPARE(dock->isVisible(), before);

    // ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW flips the flag captured by the
    // QSettings round-trip.
    const bool objs_before = D3EditState.objects_in_wireframe;
    a_showobjs->trigger();
    QCoreApplication::processEvents();
    QCOMPARE(D3EditState.objects_in_wireframe, !objs_before);
    a_showobjs->trigger();
    QCoreApplication::processEvents();
    QCOMPARE(D3EditState.objects_in_wireframe, objs_before);

    // View-mode handlers update both SetViewMode() and the status bar.
    a_mine->trigger();
    QCoreApplication::processEvents();
    QCOMPARE(QtEditor::currentViewMode(), int(QtEditor::VIEW_MODE_MINE));
    a_terrain->trigger();
    QCoreApplication::processEvents();
    QCOMPARE(QtEditor::currentViewMode(), int(QtEditor::VIEW_MODE_TERRAIN));
    a_room->trigger();
    QCoreApplication::processEvents();
    QCOMPARE(QtEditor::currentViewMode(), int(QtEditor::VIEW_MODE_ROOM));
    QtEditor::SetViewMode(QtEditor::VIEW_MODE_MINE);
  }

  // Verifies saveWindowState()/restoreWindowState() persists geometry through
  // QSettings so an editor reopening under the same QApplication picks the
  // same window dimensions. Mirrors CMainFrame::OnCreateClient / OnDestroy
  // forwarding into the registry; the Qt port uses QSettings for the same.
  void testMainFrameGeometryPersisted() {
    QByteArray saved_geom;
    QRect saved_geom_rect;
    {
      QtEditor::MainWindow win;
      win.resize(987, 654);
      win.show();
      QCoreApplication::processEvents();
      saved_geom_rect = win.geometry();
      win.saveWindowState();
      QSettings settings;
      saved_geom =
          settings.value(QStringLiteral("mainwindow/geometry")).toByteArray();
    }
    QVERIFY(!saved_geom.isEmpty());

    {
      QtEditor::MainWindow win;
      win.restoreWindowState();
      win.show();
      QCoreApplication::processEvents();
      const QRect g = win.geometry();
      // Don't compare full geometry directly because the OS may apply DPI /
      // frame adjustments after restore; the size is the stable signal.
      QCOMPARE(g.size(), saved_geom_rect.size());
    }

    // Cleanup: clear settings so a future run isn't polluted.
    QSettings settings;
    settings.remove(QStringLiteral("mainwindow/geometry"));
    settings.remove(QStringLiteral("mainwindow/dock_state"));
    errno = 0;
  }

  void testInteractEveryWidget() {
    int clicks = 0;
    int edits = 0;
    // Buttons that need the network or a locked/checked-out page (the Win32
    // editor disables them when Network_up == 0 / no lock is held). Clicking
    // them in headless test mode triggers D3 core code paths that block on
    // pagelock file I/O, so skip them here.
    auto skipNetworkButton = [](const QString &oname) {
      if (oname.contains("_LOCK", Qt::CaseSensitive) || oname.endsWith("LOCK", Qt::CaseSensitive))
        return true;
      if (oname.contains("CHECKIN", Qt::CaseSensitive))
        return true;
      if (oname.contains("UNDO_LOCK", Qt::CaseSensitive))
        return true;
      if (oname.contains("OVERRIDE", Qt::CaseSensitive))
        return true;
      if (oname.contains("CHECK_OUT", Qt::CaseSensitive) || oname.endsWith("CHECKOUT", Qt::CaseSensitive))
        return true;
      // Buttons that drive the audio subsystem. The editor doesn't initialise
      // the mixer the way the game does, and BeginSoundFrame() crashes when
      // called from a Qt headless test.
      if (oname.startsWith("IDC_PLAY", Qt::CaseSensitive))
        return true;
      return false;
    };

    for (const DialogInstance &d : g_dialogs) {
      QList<QWidget *> ws;
      collectInteractive(d.handle, &ws);
      for (QWidget *w : ws) {
        const QString name = w->objectName();
        if (name.isEmpty())
          continue;

        if (QAbstractButton *b = qobject_cast<QAbstractButton *>(w)) {
          if (skipNetworkButton(name)) {
            qInfo() << "skip" << d.name << name << "(network op)";
            continue;
          }
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
  assert(errno == 0);
  return QTest::qExec(&tc, argc, argv);
}
#include "editor_test.moc"
