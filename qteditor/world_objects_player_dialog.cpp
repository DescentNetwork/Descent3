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

#include "world_objects_player_dialog.h"
#include "ui_worldobjectsplayer.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

#include <QFileInfo>

#include <cstring>
#include <filesystem>

#include "cfile.h"

#include "d3edit.h"

#include "manage.h"
#include "physics_dialog.h"
#include "polymodel.h"
#include "robotfire.h"
#include "ship.h"
#include "shippage.h"
#include "d3edit.h"

WorldObjectsPlayerDialog::WorldObjectsPlayerDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::WorldObjectsPlayerDialog)
{
  ui->setupUi(this);
  if (QPushButton *b = ui->IDC_ADD_PSHIP)
    connect(b, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onAddPship);
  if (QPushButton *b = ui->IDC_PSHIP_DELETE)
    connect(b, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipDelete);
  if (QPushButton *b = ui->IDC_PSHIP_LOCK)
    connect(b, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipLock);
  if (QPushButton *b = ui->IDC_PSHIP_CHECKIN)
    connect(b, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipCheckin);
  if (QPushButton *b = ui->IDC_PSHIPS_OUT)
    connect(b, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipsOut);
  if (QPushButton *b = ui->IDC_PSHIP_NEXT)
    connect(b, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipNext);
  if (QPushButton *b = ui->IDC_PSHIP_PREV)
    connect(b, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipPrev);
  if (QPushButton *b = ui->IDC_PSHIP_LOAD_MODEL)
    connect(b, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipLoadModel);
  if (QPushButton *b = ui->IDC_PSHIP_DYING_MODEL)
    connect(b, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipDyingModel);
  if (QPushButton *b = ui->IDC_NULL_DYING)
    connect(b, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onNullDying);
  if (QPushButton *b = ui->IDC_EDIT_WEAPONS)
    connect(b, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onEditWeapons);
  if (QPushButton *b = ui->IDC_PSHIP_COCKPIT)
    connect(b, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipCockpit);
  if (QPushButton *b = ui->IDC_PSHIP_EDIT_PHYSICS)
    connect(b, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipEditPhysics);
  if (QPushButton *b = ui->IDC_NOLOD)
    connect(b, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onNolod);

  if (QComboBox *combo = ui->IDC_PSHIP_PULLDOWN)
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &WorldObjectsPlayerDialog::onPshipPulldownChanged);

  const char *edits[] = {"IDC_PSHIP_NAME_EDIT", "IDC_PSHIP_COCKPIT_EDIT", "IDC_SHIP_ARMOR_EDIT",
                         "IDC_LOD_DISTANCE_EDIT"};
  for (const char *name : edits) {
    if (QLineEdit *edit = findChild<QLineEdit*>(name))
      connect(edit, &QLineEdit::editingFinished, this, [this, name]() {
        const int n = D3EditState.current_ship;
        if (n < 0 || n >= MAX_SHIPS || !Ships[n].used)
          return;
        if (QString::compare(name, "IDC_PSHIP_COCKPIT_EDIT") == 0)
          Ships[n].cockpit_name = findChild<QLineEdit*>(name)->text().toStdString();
        else if (QString::compare(name, "IDC_SHIP_ARMOR_EDIT") == 0) {
          float val = findChild<QLineEdit*>(name)->text().toFloat();
          if (val < .05f)
            val = .05f;
          if (val > 10)
            val = 10;
          Ships[n].armor_scalar = val;
          updateDialog();
        } else if (QString::compare(name, "IDC_LOD_DISTANCE_EDIT") == 0) {
          const float dist = findChild<QLineEdit*>(name)->text().toFloat();
          if (dist < 0)
            return;
          if (m_lod == 1)
            Ships[n].med_lod_distance = dist;
          else if (m_lod == 2)
            Ships[n].lo_lod_distance = dist;
        }
      });
  }

  if (QCheckBox *cb = ui->IDC_DEFAULTALLOW)
    connect(cb, &QCheckBox::toggled, this, &WorldObjectsPlayerDialog::onDefaultAllowToggled);

  if (QRadioButton *rb = ui->IDC_HIRES_RADIO)
    connect(rb, &QRadioButton::clicked, this, &WorldObjectsPlayerDialog::onHiresRadio);
  if (QRadioButton *rb = ui->IDC_MEDRES_RADIO)
    connect(rb, &QRadioButton::clicked, this, &WorldObjectsPlayerDialog::onMedresRadio);
  if (QRadioButton *rb = ui->IDC_LORES_RADIO)
    connect(rb, &QRadioButton::clicked, this, &WorldObjectsPlayerDialog::onLoresRadio);

  m_lod = 0;
  updateDialog();
}

WorldObjectsPlayerDialog::~WorldObjectsPlayerDialog() { delete ui; }

void WorldObjectsPlayerDialog::updateDialog() {
  if (QPushButton *next = ui->IDC_PSHIP_NEXT)
    next->setEnabled(Num_ships >= 1);
  if (QPushButton *prev = ui->IDC_PSHIP_PREV)
    prev->setEnabled(Num_ships >= 1);
  if (QPushButton *cockpit = ui->IDC_PSHIP_COCKPIT)
    cockpit->setEnabled(Num_ships >= 1);
  if (!Network_up) {
    for (const char *name : {"IDC_PSHIP_LOCK", "IDC_PSHIP_CHECKIN", "IDC_OVERRIDE"}) {
      if (auto *w = findChild<QPushButton*>(name))
        w->setEnabled(false);
    }
    return;
  }
  if (Num_ships < 1)
    return;

  int n = D3EditState.current_ship;
  if (!Ships[n].used)
    n = D3EditState.current_ship = GetNextShip(n);

  if (QLineEdit *edit = ui->IDC_PSHIP_NAME_EDIT)
    edit->setText(QString::fromStdString(Ships[n].name));

  if (QLineEdit *edit = ui->IDC_PSHIP_MODEL_NAME_EDIT) {
    if (m_lod == 0)
      edit->setText(QString::fromStdString(Poly_models[Ships[n].model_handle].name));
    else if (m_lod == 1)
    {
      if(Ships[n].med_render_handle == -1)
        edit->setText("No model defined");
      else
        edit->setText(QString::fromStdString(Poly_models[Ships[n].med_render_handle].name));
    } else {
      if(Ships[n].lo_render_handle == -1)
        edit->setText("No model defined");
      else
        edit->setText(QString::fromStdString(Poly_models[Ships[n].lo_render_handle].name));
    }
  }

  if (QLineEdit *edit = ui->IDC_LOD_DISTANCE_EDIT) {
    if (m_lod == 0)
      edit->setText("0");
    else if (m_lod == 1)
      edit->setText(QString::number(Ships[n].med_lod_distance));
    else
      edit->setText(QString::number(Ships[n].lo_lod_distance));
  }

  if (QLineEdit *edit = ui->IDC_PSHIP_DYING_MODEL_NAME_EDIT)
  {
    if(Ships[n].dying_model_handle == -1)
      edit->setText("<none>");
    else
      edit->setText(QString::fromStdString(Poly_models[Ships[n].dying_model_handle].name));
  }
  if (QLineEdit *edit = ui->IDC_PSHIP_COCKPIT_EDIT)
    edit->setText(QString::fromStdString(Ships[n].cockpit_name));
  if (QLineEdit *edit = ui->IDC_SHIP_ARMOR_EDIT)
    edit->setText(QString::number(Ships[n].armor_scalar));

  if (QPushButton *checkin = ui->IDC_PSHIP_CHECKIN) {
    if (mng_FindTrackLock(Ships[n].name, PAGETYPE_SHIP) == -1) {
      checkin->setEnabled(false);
      if (QPushButton *lock = ui->IDC_PSHIP_LOCK)
        lock->setEnabled(true);
    } else {
      checkin->setEnabled(true);
      if (QPushButton *lock = ui->IDC_PSHIP_LOCK)
        lock->setEnabled(false);
    }
  }

  if (QCheckBox *cb = ui->IDC_DEFAULTALLOW)
    cb->setChecked(Ships[n].flags & SF_DEFAULT_ALLOW);

  if (QComboBox *combo = ui->IDC_PSHIP_PULLDOWN) {
    QSignalBlocker blocker(combo);
    combo->clear();
    for (int i = 0; i < MAX_SHIPS; i++)
      if (Ships[i].used)
        combo->addItem(QString::fromStdString(Ships[i].name));
    combo->setCurrentText(QString::fromStdString(Ships[n].name));
  }

  if (QPushButton *nolod = ui->IDC_NOLOD) {
    if (m_lod == 0)
      nolod->setEnabled(false);
    else if (m_lod == 1)
      nolod->setEnabled(Ships[n].med_render_handle != -1);
    else
      nolod->setEnabled(Ships[n].lo_render_handle != -1);
  }

  if (QRadioButton *rb = ui->IDC_HIRES_RADIO)
    rb->setChecked(m_lod == 0);
  if (QRadioButton *rb = ui->IDC_MEDRES_RADIO)
    rb->setChecked(m_lod == 1);
  if (QRadioButton *rb = ui->IDC_LORES_RADIO)
    rb->setChecked(m_lod == 2);
}

void WorldObjectsPlayerDialog::onAddPship() {
  if (!Network_up) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Sorry babe, the network is down.  This action is a no-no.\n");
    return;
  }

  QString Current_model_dir; // get from settings
  const QString pathname =
      QFileDialog::getOpenFileName(this, "Select ship model", Current_model_dir, "Descent III files (*.pof *.oof)");
  if (pathname.isEmpty())
    return;

  QFileInfo fileInfo(pathname);
  const std::filesystem::path pathFs(pathname.toStdString());
  const std::string fname = fileInfo.baseName().toStdString();

  const int img_handle = LoadShipImage(pathFs);
  if (img_handle < 0) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't open that model file.");
    return;
  }

  int ship_handle = AllocShip();
  int c = 1;
  bool finding_name = true;
  std::string cur_name;
  while (finding_name) {
    if (c == 1)
      cur_name = fname;
    else
      cur_name = fname + std::to_string(c);
    if (FindShipName(cur_name) != -1)
      c++;
    else
      finding_name = false;
  }

  Ships[ship_handle].name = cur_name;
  Ships[ship_handle].model_handle = img_handle;

  std::filesystem::path destname = LocalModelsDir / Poly_models[Ships[ship_handle].model_handle].name;
  std::filesystem::copy(pathFs, (destname), std::filesystem::copy_options::overwrite_existing);

  mng_AllocTrackLock(cur_name, PAGETYPE_SHIP);
  D3EditState.current_ship = ship_handle;
  RemapShips();
  updateDialog();
}

void WorldObjectsPlayerDialog::onPshipDelete() {
  const int n = D3EditState.current_ship;
  if (Num_ships < 1)
    return;

  const int tl = mng_FindTrackLock(Ships[n].name, PAGETYPE_SHIP);
  if (tl == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "This ship is not yours to delete.  Lock first.");
    return;
  }

  if (QMessageBox::question(this, "Delete ship", QString("Are you sure you want to delete this ship? %1").arg(QString::fromStdString(Ships[n].name))) !=
      QMessageBox::Yes)
    return;

  if (!mng_MakeLocker())
    return;

  mngs_Pagelock pl;
  pl.name = Ships[n].name;
  pl.pagetype = PAGETYPE_SHIP;

  if (mng_CheckIfPageOwned(&pl, TableUser.toStdString()) != 1) {
    mng_FreeTrackLock(tl);
    Q_ASSERT(mng_DeletePage(Ships[n].name, PAGETYPE_SHIP, 1));
  } else {
    mng_FreeTrackLock(tl);
    mng_DeletePage(Ships[n].name, PAGETYPE_SHIP, 0);
    mng_DeletePage(Ships[n].name, PAGETYPE_SHIP, 1);
    mng_DeletePagelock(Ships[n].name, PAGETYPE_SHIP);
  }

  D3EditState.current_ship = GetNextShip(n);
  FreePolyModel(Ships[n].model_handle);
  if (Ships[n].dying_model_handle != -1)
    FreePolyModel(Ships[n].dying_model_handle);
  FreeShip(n);
  mng_EraseLocker();

  QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Ship deleted.");
  RemapShips();
  updateDialog();
}

void WorldObjectsPlayerDialog::onPshipLock() {
  const int n = D3EditState.current_ship;
  mngs_Pagelock temp_pl;
  mngs_ship_page shippage;

  if (Num_ships < 1)
    return;
  if (!mng_MakeLocker())
    return;

  temp_pl.name = Ships[n].name;
  temp_pl.pagetype = PAGETYPE_SHIP;

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
    } else if (mng_FindSpecificShipPage(temp_pl.name, &shippage)) {
      if (mng_AssignShipPageToShip(&shippage, n)) {
        if (!mng_ReplacePage(Ships[n].name, Ships[n].name, n, PAGETYPE_SHIP, 1)) {
          QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was problem writing that page locally!");
          mng_EraseLocker();
          return;
        }
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Ship locked.");
      } else {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was a problem loading this ship.");
      }
      mng_AllocTrackLock(Ships[n].name, PAGETYPE_SHIP);
      updateDialog();
    } else {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't find that ship in the table file!");
    }
  }
  mng_EraseLocker();
}

void WorldObjectsPlayerDialog::onPshipCheckin() {
  const int n = D3EditState.current_ship;
  mngs_Pagelock temp_pl;

  if (Num_ships < 1)
    return;
  if (!mng_MakeLocker())
    return;

  temp_pl.name = Ships[n].name;
  temp_pl.pagetype = PAGETYPE_SHIP;

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
    } else if (!mng_ReplacePage(Ships[n].name, Ships[n].name, n, PAGETYPE_SHIP, 0)) {
      QMessageBox::critical(this, "Error!", ErrorString);
    } else {
      std::filesystem::path srcname = LocalModelsDir / Poly_models[Ships[n].model_handle].name;
      std::filesystem::path destname = NetModelsDir / Poly_models[Ships[n].model_handle].name;
      std::filesystem::copy((srcname), (destname), std::filesystem::copy_options::overwrite_existing);
      if (Ships[n].dying_model_handle != -1) {
        srcname = LocalModelsDir / Poly_models[Ships[n].dying_model_handle].name;
        destname = NetModelsDir / Poly_models[Ships[n].dying_model_handle].name;
        std::filesystem::copy((srcname), (destname), std::filesystem::copy_options::overwrite_existing);
      }
      if (Ships[n].med_render_handle != -1) {
        srcname = LocalModelsDir / Poly_models[Ships[n].med_render_handle].name;
        destname = NetModelsDir / Poly_models[Ships[n].med_render_handle].name;
        std::filesystem::copy((srcname), (destname), std::filesystem::copy_options::overwrite_existing);
      }
      if (Ships[n].lo_render_handle != -1) {
        srcname = LocalModelsDir / Poly_models[Ships[n].lo_render_handle].name;
        destname = NetModelsDir / Poly_models[Ships[n].lo_render_handle].name;
        std::filesystem::copy((srcname), (destname), std::filesystem::copy_options::overwrite_existing);
      }
      if (!Ships[n].cockpit_name.empty()) {
        srcname = LocalMiscDir / Ships[n].cockpit_name;
        destname = NetMiscDir / Ships[n].cockpit_name;
        std::filesystem::copy((srcname), (destname), std::filesystem::copy_options::overwrite_existing);
      }

      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Ship checked in.");

      Q_ASSERT(mng_DeletePage(Ships[n].name, PAGETYPE_SHIP, 1) == 1);
      mng_EraseLocker();

      const int p = mng_FindTrackLock(Ships[n].name, PAGETYPE_SHIP);
      Q_ASSERT(p != -1);
      mng_FreeTrackLock(p);
      updateDialog();
    }
  }
  mng_EraseLocker();
}

void WorldObjectsPlayerDialog::onPshipsOut() {
  QString str = QString("User %1 has these ships held locally:\n\n").arg(TableUser);
  int total = 0;
  for (int i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used && GlobalTrackLocks[i].pagetype == PAGETYPE_SHIP) {
      str += QString::fromStdString(GlobalTrackLocks[i].name);
      str += "\n";
      total++;
    }
  }
  if (total != 0)
    QMessageBox::information(this, "Ships", str);
}

void WorldObjectsPlayerDialog::onPshipNext() {
  D3EditState.current_ship = GetNextShip(D3EditState.current_ship);
  m_lod = 0;
  updateDialog();
}

void WorldObjectsPlayerDialog::onPshipPrev() {
  D3EditState.current_ship = GetPrevShip(D3EditState.current_ship);
  m_lod = 0;
  updateDialog();
}

void WorldObjectsPlayerDialog::onPshipPulldownChanged() {
  QComboBox *combo = ui->IDC_PSHIP_PULLDOWN;
  if (combo == nullptr)
    return;
  const int i = FindShipName(combo->currentText().toStdString());
  if (i == -1)
    return;
  D3EditState.current_ship = i;
  updateDialog();
}

void WorldObjectsPlayerDialog::onPshipLoadModel() {
  QString Current_model_dir; // get from settings
  const QString pathname =
      QFileDialog::getOpenFileName(this, "Select ship model", Current_model_dir, "Descent III files (*.pof *.oof)");
  if (pathname.isEmpty())
    return;

  const std::filesystem::path pathFs(pathname.toStdString());
  const int img_handle = LoadPolyModel(pathFs, 0);
  if (img_handle < 0) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't open that animation/model file.");
    return;
  }

  const int ship_handle = D3EditState.current_ship;
  if (m_lod == 0) {
    ChangeOldModelsForObjects(Ships[ship_handle].model_handle, img_handle);
    FreePolyModel(Ships[ship_handle].model_handle);
    Ships[ship_handle].model_handle = img_handle;
  } else if (m_lod == 1) {
    if (Ships[ship_handle].med_render_handle != -1)
      FreePolyModel(Ships[ship_handle].med_render_handle);
    Ships[ship_handle].med_render_handle = img_handle;
  } else {
    if (Ships[ship_handle].lo_render_handle != -1)
      FreePolyModel(Ships[ship_handle].lo_render_handle);
    Ships[ship_handle].lo_render_handle = img_handle;
  }

  if (QMessageBox::question(this, "Are you sure?", "Would you like to clear the weapon battery info?") == QMessageBox::Yes) {
    WBClearInfo(Ships[ship_handle].static_wb);
  }

  std::filesystem::path curname = LocalModelsDir / Poly_models[img_handle].name;
  std::filesystem::copy(pathFs, (curname), std::filesystem::copy_options::overwrite_existing);
  updateDialog();
}

void WorldObjectsPlayerDialog::onPshipDyingModel() {
  QString Current_model_dir; // get from settings
  const QString pathname =
      QFileDialog::getOpenFileName(this, "Select dying model", Current_model_dir, "Descent III files (*.pof *.oof)");
  if (pathname.isEmpty())
    return;

  const std::filesystem::path pathFs(pathname.toStdString());
  const int img_handle = LoadShipImage(pathFs);
  if (img_handle < 0) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't open that animation/model file.");
    return;
  }

  const int ship_handle = D3EditState.current_ship;
  Ships[ship_handle].dying_model_handle = img_handle;
  std::filesystem::path curname = LocalModelsDir / Poly_models[Ships[ship_handle].dying_model_handle].name;
  std::filesystem::copy(pathFs, (curname), std::filesystem::copy_options::overwrite_existing);
  updateDialog();
}

void WorldObjectsPlayerDialog::onNullDying() {
  const int n = D3EditState.current_ship;
  Ships[n].dying_model_handle = -1;
  updateDialog();
}

void WorldObjectsPlayerDialog::onEditWeapons() {
  // Ported in the player_weapons_dialog module (PlayerWeaponsDialog).
  extern void editPlayerWeapons(int shipHandle, QWidget *parent);
  editPlayerWeapons(D3EditState.current_ship, this);
}

void WorldObjectsPlayerDialog::onPshipCockpit()
{
  const QString pathname =
      QFileDialog::getOpenFileName(this, "Select cockpit file", {}, "Descent III files (*.inf)");
  if (pathname.isEmpty())
    return;

  // Keep only the file name (drop the source directory) so the cockpit is
  // referenced relative to the local misc dir, matching the Win32 editor.
  const std::filesystem::path picked{pathname.toStdString()};
  const std::string cockpitFile = picked.filename().string();
  if (cockpitFile.empty())
    return;

  ship &shp = Ships[D3EditState.current_ship];
  shp.cockpit_name = cockpitFile;

  // Copy the picked file into the local misc dir under its relative name.
  const std::filesystem::path dest = LocalMiscDir / cockpitFile;
  std::filesystem::copy_file(picked, dest, std::filesystem::copy_options::overwrite_existing);

  updateDialog();
}

void WorldObjectsPlayerDialog::onPshipEditPhysics() {
  const int n = D3EditState.current_ship;
  PhysicsDialog dlg(&Ships[n].phys_info, this);
  dlg.exec();
}

void WorldObjectsPlayerDialog::onKillfocusName() {
  const int n = D3EditState.current_ship;
  QLineEdit *edit = ui->IDC_PSHIP_NAME_EDIT;
  if (edit == nullptr)
    return;

  const int p = mng_FindTrackLock(Ships[n].name, PAGETYPE_SHIP);
  if (p == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You must lock this ship if you wish to change its name.");
    edit->setText(QString::fromStdString(Ships[n].name));
    return;
  }

  std::string name;
  name = edit->text().toStdString();
  if (FindShipName(name) != -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There already is a ship with that name...choose another name.");
    edit->setText(QString::fromStdString(Ships[n].name));
    return;
  }

  if (!mng_MakeLocker())
    return;

  mngs_Pagelock pl;
  pl.name = Ships[n].name;
  pl.pagetype = PAGETYPE_SHIP;

  const int ret = mng_CheckIfPageOwned(&pl, TableUser.toStdString());
  if (ret < 0)
    QMessageBox::critical(this, "Error!", ErrorString);
  else if (ret == 1)
    mng_RenamePage(Ships[n].name, name, PAGETYPE_SHIP);
  else if (ret == 2) {
    std::string oldname;
    oldname = Ships[n].name;
    Ships[n].name = name;
    mng_ReplacePage(oldname, Ships[n].name, n, PAGETYPE_SHIP, 1);
  } else if (ret == 0) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You don't own this page.  Get Jason now!");
    mng_FreeTrackLock(p);
    return;
  }

  GlobalTrackLocks[p].name = name;
  Ships[n].name = name;
  mng_EraseLocker();
  RemapShips();
  updateDialog();
}

void WorldObjectsPlayerDialog::onKillfocusCockpit() {
  const int n = D3EditState.current_ship;
  if (QLineEdit *edit = ui->IDC_PSHIP_COCKPIT_EDIT)
    Ships[n].cockpit_name = edit->text().toStdString();
}

void WorldObjectsPlayerDialog::onKillfocusArmor() {
  const int n = D3EditState.current_ship;
  if (QLineEdit *edit = ui->IDC_SHIP_ARMOR_EDIT) {
    float val = edit->text().toFloat();
    if (val < .05f)
      val = .05f;
    if (val > 10)
      val = 10;
    Ships[n].armor_scalar = val;
    updateDialog();
  }
}

void WorldObjectsPlayerDialog::onKillfocusLodDistance() {
  const int n = D3EditState.current_ship;
  if (QLineEdit *edit = ui->IDC_LOD_DISTANCE_EDIT) {
    const float dist = edit->text().toFloat();
    if (dist < 0)
      return;
    if (m_lod == 1)
      Ships[n].med_lod_distance = dist;
    else if (m_lod == 2)
      Ships[n].lo_lod_distance = dist;
    updateDialog();
  }
}

void WorldObjectsPlayerDialog::onDefaultAllowToggled(bool checked) {
  const int n = D3EditState.current_ship;
  if (checked)
    Ships[n].flags |= SF_DEFAULT_ALLOW;
  else
    Ships[n].flags &= ~SF_DEFAULT_ALLOW;
}

void WorldObjectsPlayerDialog::onHiresRadio() {
  m_lod = 0;
  updateDialog();
}

void WorldObjectsPlayerDialog::onMedresRadio() {
  m_lod = 1;
  updateDialog();
}

void WorldObjectsPlayerDialog::onLoresRadio() {
  m_lod = 2;
  updateDialog();
}

void WorldObjectsPlayerDialog::onNolod() {
  const int n = D3EditState.current_ship;
  if (m_lod == 0) {
    QMessageBox::warning(this, "No LOD", "You must have a hi-res model.");
    return;
  }
  if (m_lod == 1) {
    FreePolyModel(Ships[n].med_render_handle);
    Ships[n].med_render_handle = -1;
  } else {
    FreePolyModel(Ships[n].lo_render_handle);
    Ships[n].lo_render_handle = -1;
  }
  updateDialog();
}

