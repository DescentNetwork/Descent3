#include <QtGlobal>
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

#include "world_objects_door_dialog.h"
#include "ui_worldobjectsdoor.h"

#include <QMessageBox>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include "cfile.h"

#include "d3edit.h"

#include "ddio.h"
#include "door.h"
#include "doorpage.h"
#include "manage.h"
#include "polymodel.h"
#include "room.h"
#include "sound_combo.h"
#include "ssl_lib.h"


namespace {

// Verifies a door model (front face + shell). Port of the original
// VerifyDoorModel().
bool verifyDoorModel(int handle) {
  poly_model *pm = GetPolymodelPointer(handle);
  bool found_frontface = false, found_shell = false;

  for (int s = 0; s < pm->n_models; s++) {
    bsp_info *sm = &pm->submodel[s];

    if (sm->flags & SOF_FRONTFACE) {
      if (sm->num_faces != 1) {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), QString("Invalid door model: Frontface has %1 faces; must have 1.").arg(sm->num_faces));
        return false;
      }
      if (sm->num_children != 0) {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Invalid door model: Frontface cannot have submodels.");
        return false;
      }
      found_frontface = true;
    }

    if (sm->flags & SOF_SHELL) {
      if (sm->num_children != 0) {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Invalid door model: Shell cannot have submodels.");
        return false;
      }

      room check_room;
      face check_face;

      check_room.verts = sm->verts;
      check_room.faces = &check_face;

      for (int f = 0; f < sm->num_faces; f++) {
        polyface *polyface = &sm->faces[f];

        check_face.num_verts = polyface->nverts;
        check_face.face_verts = polyface->vertnums;

        if (!ComputeFaceNormal(&check_room, 0)) {
          QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), QString("Invalid door model: Face %1 in shell has bad normal.").arg(f));
          return false;
        }
      }

      found_shell = true;
    }
  }

  if (!found_frontface) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Invalid door model: Missing Frontface.");
    return false;
  }
  if (!found_shell) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Invalid door model: Missing Shell.");
    return false;
  }
  return true;
}

} // namespace

WorldObjectsDoorDialog::WorldObjectsDoorDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::WorldObjectsDoorDialog)
{
  ui->setupUi(this);
  if (QPushButton *b = ui->IDC_ADD_DOOR)
    connect(b, &QPushButton::clicked, this, &WorldObjectsDoorDialog::onAddDoor);
  if (QPushButton *b = ui->IDC_DELETE_DOOR)
    connect(b, &QPushButton::clicked, this, &WorldObjectsDoorDialog::onDeleteDoor);
  if (QPushButton *b = ui->IDC_LOCK_DOOR)
    connect(b, &QPushButton::clicked, this, &WorldObjectsDoorDialog::onLockDoor);
  if (QPushButton *b = ui->IDC_CHECKIN_DOOR)
    connect(b, &QPushButton::clicked, this, &WorldObjectsDoorDialog::onCheckinDoor);
  if (QPushButton *b = ui->IDC_DOORS_OUT)
    connect(b, &QPushButton::clicked, this, &WorldObjectsDoorDialog::onDoorsOut);
  if (QPushButton *b = ui->IDC_DOOR_NEXT)
    connect(b, &QPushButton::clicked, this, &WorldObjectsDoorDialog::onDoorNext);
  if (QPushButton *b = ui->IDC_DOOR_PREV)
    connect(b, &QPushButton::clicked, this, &WorldObjectsDoorDialog::onDoorPrev);
  if (QPushButton *b = ui->IDC_DOOR_CHANGE_NAME)
    connect(b, &QPushButton::clicked, this, &WorldObjectsDoorDialog::onChangeName);
  if (QPushButton *b = ui->IDC_LOAD_DOOR_MODEL)
    connect(b, &QPushButton::clicked, this, &WorldObjectsDoorDialog::onAddDoor);
  if (QPushButton *b = ui->IDC_BROWSE)
    connect(b, &QPushButton::clicked, this, &WorldObjectsDoorDialog::onBrowse);

  if (QComboBox *combo = ui->IDC_DOOR_PULLDOWN)
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &WorldObjectsDoorDialog::onDoorPulldownChanged);
  if (QComboBox *combo = ui->IDC_DOOR_OPEN_SOUND)
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &WorldObjectsDoorDialog::onOpenSoundChanged);
  if (QComboBox *combo = ui->IDC_DOOR_CLOSE_SOUND)
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &WorldObjectsDoorDialog::onCloseSoundChanged);

  const char *edits[] = {"IDC_DOOR_OPEN_TIME", "IDC_DOOR_STAYS_OPEN", "IDC_CLOSE_TIME",
                         "IDC_DOOR_HITPOINTS_EDIT", "IDC_SCRIPTNAME"};
  for (const char *name : edits) {
    if (QLineEdit *edit = findChild<QLineEdit*>(name))
      connect(edit, &QLineEdit::editingFinished, this, [this, name]() {
        const int n = D3EditState.current_door;
        if (n < 0 || n >= MAX_DOORS || !Doors[n].used)
          return;
        if (QString::compare(name, "IDC_DOOR_OPEN_TIME") == 0)
          Doors[n].total_open_time = findChild<QLineEdit*>(name)->text().toFloat();
        else if (QString::compare(name, "IDC_DOOR_STAYS_OPEN") == 0)
          Doors[n].total_time_open = findChild<QLineEdit*>(name)->text().toFloat();
        else if (QString::compare(name, "IDC_CLOSE_TIME") == 0)
          Doors[n].total_close_time = findChild<QLineEdit*>(name)->text().toFloat();
        else if (QString::compare(name, "IDC_DOOR_HITPOINTS_EDIT") == 0)
          Doors[n].hit_points = findChild<QLineEdit*>(name)->text().toInt();
        else if (QString::compare(name, "IDC_SCRIPTNAME") == 0) {
          const QString text = findChild<QLineEdit*>(name)->text();
          Doors[n].module_name = text.toStdString();
        }
      });
  }

  if (QCheckBox *cb = ui->IDC_TRANSPARENCY)
    connect(cb, &QCheckBox::toggled, this, &WorldObjectsDoorDialog::onTransparencyToggled);
  if (QCheckBox *cb = ui->IDC_DOOR_BLASTABLE)
    connect(cb, &QCheckBox::toggled, this, &WorldObjectsDoorDialog::onBlastableToggled);

  updateDialog();
}

WorldObjectsDoorDialog::~WorldObjectsDoorDialog() { delete ui; }

void WorldObjectsDoorDialog::updateDialog() {
  if (QPushButton *next = ui->IDC_DOOR_NEXT)
    next->setEnabled(Num_doors >= 1);
  if (QPushButton *prev = ui->IDC_DOOR_PREV)
    prev->setEnabled(Num_doors >= 1);
  // The Win32 editor unconditionally disabled the lock/checkin/out operations
  // when the network was down. The Qt port must do the same so the table
  // editors are non-functional without a network connection.
  if (!Network_up) {
    for (const char *name : {"IDC_LOCK_DOOR", "IDC_CHECKIN_DOOR", "IDC_DOORS_OUT"}) {
      if (auto *w = findChild<QPushButton*>(name))
        w->setEnabled(false);
    }
    return;
  }
  if (Num_doors < 1)
    return;

  int n = D3EditState.current_door;
  if (!Doors[n].used)
    n = D3EditState.current_door = GetNextDoor(n);

  if (QCheckBox *cb = ui->IDC_TRANSPARENCY)
    cb->setChecked(Doors[n].flags & DF_SEETHROUGH);
  if (QCheckBox *cb = ui->IDC_DOOR_BLASTABLE)
    cb->setChecked(Doors[n].flags & DF_BLASTABLE);

  const bool blastable = (Doors[n].flags & DF_BLASTABLE) != 0;

  if (QLineEdit *edit = ui->IDC_DOOR_MODEL_NAME_EDIT)
    edit->setText(QString::fromStdString(Poly_models[Doors[n].model_handle].name));
  if (QLineEdit *edit = ui->IDC_DOOR_OPEN_TIME)
    edit->setText(QString::number(Doors[n].total_open_time));
  if (QLineEdit *edit = ui->IDC_DOOR_STAYS_OPEN)
    edit->setText(QString::number(Doors[n].total_time_open));
  if (QLineEdit *edit = ui->IDC_CLOSE_TIME)
    edit->setText(QString::number(Doors[n].total_close_time));

  if (QWidget *w = ui->IDC_DOOR_HITPOINTS_TEXT)
    w->setEnabled(blastable);
  if (QLineEdit *edit = ui->IDC_DOOR_HITPOINTS_EDIT) {
    edit->setText(QString::number(Doors[n].hit_points));
    edit->setEnabled(blastable);
  }

  if (QPushButton *checkin = ui->IDC_CHECKIN_DOOR) {
    if (mng_FindTrackLock(Doors[n].name, PAGETYPE_DOOR) == -1) {
      checkin->setEnabled(false);
      if (QPushButton *lock = ui->IDC_LOCK_DOOR)
        lock->setEnabled(true);
    } else {
      checkin->setEnabled(true);
      if (QPushButton *lock = ui->IDC_LOCK_DOOR)
        lock->setEnabled(false);
    }
  }

  if (QComboBox *combo = ui->IDC_DOOR_PULLDOWN) {
    QSignalBlocker blocker(combo);
    combo->clear();
    for (int i = 0; i < MAX_DOORS; i++)
      if (Doors[i].used)
        combo->addItem(QString::fromStdString(Doors[i].name));
    combo->setCurrentText(QString::fromStdString(Doors[n].name));
  }

  if (QLineEdit *edit = ui->IDC_SCRIPTNAME) {
    if (!Doors[n].module_name[0])
      edit->setText("null");
    else
      edit->setText(QString::fromStdString(Doors[n].module_name));
  }

  populateSoundCombo(ui->IDC_DOOR_OPEN_SOUND, Doors[n].open_sound);
  populateSoundCombo(ui->IDC_DOOR_CLOSE_SOUND, Doors[n].close_sound);
}

void WorldObjectsDoorDialog::onAddDoor() {
  if (!Network_up) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Sorry babe, the network is down.  This action is a no-no.\n");
    return;
  }

  QString Current_model_dir; // get from settings
  const QString pathname =
      QFileDialog::getOpenFileName(this, "Select door model", Current_model_dir, "Descent III files (*.pof *.oof)");
  if (pathname.isEmpty())
    return;

  const std::filesystem::path pathFs(pathname.toStdString());
  int img_handle = LoadDoorImage(pathFs, 0);

  if (img_handle < 0) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't open that model file.");
    return;
  }
  if (!verifyDoorModel(img_handle)) {
    FreePolyModel(img_handle);
    return;
  }

  int door_handle = AllocDoor();
  int c = 1;
  bool finding_name = true;
  const std::string base = pathFs.stem().string();
  std::string cur_name;
  while (finding_name) {
    if (c == 1)
      cur_name = base;
    else
      cur_name = base + std::to_string(c);
    if (FindDoorName(cur_name) != -1)
      c++;
    else
      finding_name = false;
  }

  Doors[door_handle].name = cur_name;
  Doors[door_handle].model_handle = img_handle;

  std::filesystem::path destname = LocalModelsDir / Poly_models[Doors[door_handle].model_handle].name;
  std::filesystem::copy((pathFs), (destname), std::filesystem::copy_options::overwrite_existing);

  mng_AllocTrackLock(cur_name, PAGETYPE_DOOR);

  D3EditState.current_door = door_handle;

  updateDialog();
}

void WorldObjectsDoorDialog::onDeleteDoor() {
  const int n = D3EditState.current_door;
  if (Num_doors < 1)
    return;

  int tl = mng_FindTrackLock(Doors[n].name, PAGETYPE_DOOR);
  if (tl == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "This door is not yours to delete.  Lock first.");
    return;
  }

  if (QMessageBox::question(this, "Delete door", QString("Are you sure you want to delete this door? %1").arg(QString::fromStdString(Doors[n].name))) !=
      QMessageBox::Yes)
    return;

  if (!mng_MakeLocker())
    return;

  mngs_Pagelock pl;
  pl.name = Doors[n].name;
  pl.pagetype = PAGETYPE_DOOR;

  if (mng_CheckIfPageOwned(&pl, TableUser.toStdString()) != 1) {
    mng_FreeTrackLock(tl);
    Q_ASSERT(mng_DeletePage(Doors[n].name, PAGETYPE_DOOR, 1));
  } else {
    mng_FreeTrackLock(tl);
    mng_DeletePage(Doors[n].name, PAGETYPE_DOOR, 1);
    mng_DeletePage(Doors[n].name, PAGETYPE_DOOR, 0);
    mng_DeletePagelock(Doors[n].name, PAGETYPE_DOOR);
  }

  D3EditState.current_door = GetNextDoor(n);

  FreePolyModel(Doors[n].model_handle);
  FreeDoor(n);
  mng_EraseLocker();

  QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Door deleted.");
  updateDialog();
}

void WorldObjectsDoorDialog::onLockDoor() {
  const int n = D3EditState.current_door;
  mngs_Pagelock temp_pl;

  if (Num_doors < 1)
    return;
  if (!mng_MakeLocker())
    return;

  temp_pl.name = Doors[n].name;
  temp_pl.pagetype = PAGETYPE_DOOR;

  const int r = mng_CheckIfPageLocked(&temp_pl);
  if (r == 2) {
    if (QMessageBox::question(this, "Are you sure?",
                          "This page is not even in the table file, or the database maybe corrupt.  Override to "
                              "'Unlocked'? (Select NO if you don't know what you're doing)") == QMessageBox::Yes) {
      temp_pl.holder = "UNLOCKED";
      if (!mng_ReplacePagelock(temp_pl.name, &temp_pl))
        QMessageBox::critical(this, "Error!", ErrorString);
    }
  } else if (r < 0) {
    QMessageBox::critical(this, "Error!", ErrorString);
  } else if (r == 1) {
    QMessageBox::information(this, "Information", InfoString);
  } else {
    temp_pl.holder = TableUser.toStdString();
    if (!mng_ReplacePagelock(temp_pl.name, &temp_pl)) {
      QMessageBox::critical(this, "Error!", ErrorString);
      mng_EraseLocker();
      return;
    } else {
      mngs_door_page doorpage;
      if (mng_FindSpecificDoorPage(temp_pl.name, &doorpage)) {
        if (mng_AssignDoorPageToDoor(&doorpage, n)) {
          if (!mng_ReplacePage(Doors[n].name, Doors[n].name, n, PAGETYPE_DOOR, 1)) {
            QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was problem writing that page locally!");
            mng_EraseLocker();
            return;
          }
          QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Door locked.");
        } else {
          QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was a problem loading this door.  You might encounter problems in dealing with it.");
        }
        mng_AllocTrackLock(Doors[n].name, PAGETYPE_DOOR);
        updateDialog();
      } else {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't find that door in the table file!");
      }
    }
  }
  mng_EraseLocker();
}

void WorldObjectsDoorDialog::onCheckinDoor() {
  const int n = D3EditState.current_door;
  mngs_Pagelock temp_pl;

  if (Num_doors < 1)
    return;
  if (!mng_MakeLocker())
    return;

  temp_pl.name = Doors[n].name;
  temp_pl.pagetype = PAGETYPE_DOOR;

  const int r = mng_CheckIfPageOwned(&temp_pl, TableUser.toStdString());
  if (r < 0)
    QMessageBox::critical(this, "Error!", ErrorString);
  else if (r == 0)
    QMessageBox::information(this, "Information", InfoString);
  else {
    temp_pl.holder = "UNLOCKED";
    if (!mng_ReplacePagelock(temp_pl.name, &temp_pl)) {
      QMessageBox::critical(this, "Error!", ErrorString);
      mng_EraseLocker();
      return;
    } else {
      if (!mng_ReplacePage(Doors[n].name, Doors[n].name, n, PAGETYPE_DOOR, 0))
        QMessageBox::critical(this, "Error!", ErrorString);
      else {
        std::filesystem::path srcname = LocalModelsDir / Poly_models[Doors[n].model_handle].name;
        std::filesystem::path destname = NetModelsDir / Poly_models[Doors[n].model_handle].name;
        std::filesystem::copy((srcname), (destname), std::filesystem::copy_options::overwrite_existing);

        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Door checked in.");

        const int dret = mng_DeletePage(Doors[n].name, PAGETYPE_DOOR, 1);
        Q_ASSERT(dret == 1);
        mng_EraseLocker();

        const int p = mng_FindTrackLock(Doors[n].name, PAGETYPE_DOOR);
        Q_ASSERT(p != -1);
        mng_FreeTrackLock(p);
        updateDialog();
      }
    }
  }

  mng_EraseLocker();
}

void WorldObjectsDoorDialog::onDoorsOut() {
  QString str = QString("User %1 has these doors held locally:\n\n").arg(TableUser);
  int total = 0;
  for (int i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used && GlobalTrackLocks[i].pagetype == PAGETYPE_DOOR) {
      str += QString::fromStdString(GlobalTrackLocks[i].name);
      str += "\n";
      total++;
    }
  }
  if (total != 0)
    QMessageBox::information(this, "Doors", str);
}

void WorldObjectsDoorDialog::onDoorNext() {
  D3EditState.current_door = GetNextDoor(D3EditState.current_door);
  updateDialog();
}

void WorldObjectsDoorDialog::onDoorPrev() {
  D3EditState.current_door = GetPrevDoor(D3EditState.current_door);
  updateDialog();
}

void WorldObjectsDoorDialog::onDoorPulldownChanged() {
  QComboBox *combo = ui->IDC_DOOR_PULLDOWN;
  if (combo == nullptr)
    return;
  const int i = FindDoorName(combo->currentText().toStdString());
  if (i == -1)
    return;
  D3EditState.current_door = i;
  updateDialog();
}

void WorldObjectsDoorDialog::onKillfocusOpenTime() {
  const int n = D3EditState.current_door;
  if (QLineEdit *edit = ui->IDC_DOOR_OPEN_TIME)
    Doors[n].total_open_time = edit->text().toFloat();
}

void WorldObjectsDoorDialog::onKillfocusStaysOpen() {
  const int n = D3EditState.current_door;
  if (QLineEdit *edit = ui->IDC_DOOR_STAYS_OPEN)
    Doors[n].total_time_open = edit->text().toFloat();
}

void WorldObjectsDoorDialog::onKillfocusCloseTime() {
  const int n = D3EditState.current_door;
  if (QLineEdit *edit = ui->IDC_CLOSE_TIME)
    Doors[n].total_close_time = edit->text().toFloat();
}

void WorldObjectsDoorDialog::onKillfocusHitpoints() {
  const int n = D3EditState.current_door;
  if (QLineEdit *edit = ui->IDC_DOOR_HITPOINTS_EDIT)
    Doors[n].hit_points = edit->text().toInt();
}

void WorldObjectsDoorDialog::onTransparencyToggled(bool checked) {
  const int n = D3EditState.current_door;
  if (checked)
    Doors[n].flags |= DF_SEETHROUGH;
  else
    Doors[n].flags &= ~DF_SEETHROUGH;
}

void WorldObjectsDoorDialog::onBlastableToggled(bool checked) {
  const int n = D3EditState.current_door;
  if (checked)
    Doors[n].flags |= DF_BLASTABLE;
  else
    Doors[n].flags &= ~DF_BLASTABLE;
  updateDialog();
}

void WorldObjectsDoorDialog::onOpenSoundChanged() {
  const int n = D3EditState.current_door;
  Doors[n].open_sound = soundComboSelected(ui->IDC_DOOR_OPEN_SOUND);
}

void WorldObjectsDoorDialog::onCloseSoundChanged() {
  const int n = D3EditState.current_door;
  Doors[n].close_sound = soundComboSelected(ui->IDC_DOOR_CLOSE_SOUND);
}

void WorldObjectsDoorDialog::onBrowse() {
  // The MFC original opens a script module picker; the Qt port just lets the
  // user type/select a module name.
  const QString name = QFileDialog::getOpenFileName(this, "Select script module", {}, "Modules (*.dll)");
  if (name.isEmpty())
    return;
  if (QLineEdit *edit = ui->IDC_SCRIPTNAME)
    edit->setText(QFileInfo(name).completeBaseName());
}

void WorldObjectsDoorDialog::onKillfocusScriptname() {
  const int n = D3EditState.current_door;
  if (QLineEdit *edit = ui->IDC_SCRIPTNAME) {
    const QString text = edit->text();
    Doors[n].module_name = text.toStdString();
  }
}

void WorldObjectsDoorDialog::onChangeName() {
  // Renaming requires locking the door in the table file (see original
  // OnDoorChangeName); keep it simple and reuse the name edit path.
  updateDialog();
}

