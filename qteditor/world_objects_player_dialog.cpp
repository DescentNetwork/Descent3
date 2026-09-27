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

#include "d3edit.h"

#include "manage.h"
#include "physics_dialog.h"
#include "polymodel.h"
#include "robotfire.h"
#include "ship.h"
#include "shippage.h"
#include "d3edit.h"

optref<ship> WorldObjectsPlayerDialog::data(void)
{
  if (app.current_ship < 0 || app.current_ship >= static_cast<int>(Ships.size()) || Ships.is_unused(app.current_ship))
    return std::nullopt;
  return Ships[app.current_ship];
}

WorldObjectsPlayerDialog::WorldObjectsPlayerDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::WorldObjectsPlayerDialog)
{
  ui->setupUi(this);
  connect(ui->IDC_ADD_PSHIP, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onAddPship);
  connect(ui->IDC_PSHIP_DELETE, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipDelete);
  connect(ui->IDC_PSHIP_LOCK, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipLock);
  connect(ui->IDC_PSHIP_CHECKIN, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipCheckin);
  connect(ui->IDC_PSHIPS_OUT, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipsOut);
  connect(ui->IDC_PSHIP_NEXT, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipNext);
  connect(ui->IDC_PSHIP_PREV, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipPrev);
  connect(ui->IDC_PSHIP_LOAD_MODEL, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipLoadModel);
  connect(ui->IDC_PSHIP_DYING_MODEL, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipDyingModel);
  connect(ui->IDC_NULL_DYING, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onNullDying);
  connect(ui->IDC_EDIT_WEAPONS, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onEditWeapons);
  connect(ui->IDC_PSHIP_COCKPIT, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipCockpit);
  connect(ui->IDC_PSHIP_EDIT_PHYSICS, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onPshipEditPhysics);
  connect(ui->IDC_NOLOD, &QPushButton::clicked, this, &WorldObjectsPlayerDialog::onNolod);

  connect(ui->IDC_PSHIP_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), this,
    &WorldObjectsPlayerDialog::onPshipPulldownChanged);

  connect(ui->IDC_PSHIP_NAME_EDIT, &QLineEdit::editingFinished, this, &WorldObjectsPlayerDialog::onKillfocusName);

  bindEdits();
  bindChecks();

  connect(ui->IDC_HIRES_RADIO, &QRadioButton::clicked, this, &WorldObjectsPlayerDialog::onHiresRadio);
  connect(ui->IDC_MEDRES_RADIO, &QRadioButton::clicked, this, &WorldObjectsPlayerDialog::onMedresRadio);
  connect(ui->IDC_LORES_RADIO, &QRadioButton::clicked, this, &WorldObjectsPlayerDialog::onLoresRadio);

  m_lod = 0;
  updateDialog();
}

WorldObjectsPlayerDialog::~WorldObjectsPlayerDialog() { delete ui; }

void WorldObjectsPlayerDialog::bindEdits() {
  connect(ui->IDC_PSHIP_COCKPIT_EDIT, &QLineEdit::editingFinished, [this]() {
    if (auto s = data()) s->cockpit_name = ui->IDC_PSHIP_COCKPIT_EDIT->text().toStdString();
  });
  connect(ui->IDC_SHIP_ARMOR_EDIT, &QLineEdit::editingFinished, [this]() {
    if (auto s = data()) {
      float val = ui->IDC_SHIP_ARMOR_EDIT->text().toFloat();
      if (val < .05f)
        val = .05f;
      if (val > 10)
        val = 10;
      s->armor_scalar = val;
      updateDialog();
    }
  });
  connect(ui->IDC_LOD_DISTANCE_EDIT, &QLineEdit::editingFinished, [this]() {
    if (auto s = data()) {
      const float dist = ui->IDC_LOD_DISTANCE_EDIT->text().toFloat();
      if (dist < 0)
        return;
      if (m_lod == 1)
        s->med_lod_distance = dist;
      else if (m_lod == 2)
        s->lo_lod_distance = dist;
    }
  });
}

void WorldObjectsPlayerDialog::bindChecks() {
  connect(ui->IDC_DEFAULTALLOW, &QCheckBox::toggled, [this](bool checked) {
    if (auto s = data())
      s->flags.default_allowed = checked;
  });
}

void WorldObjectsPlayerDialog::updateDialog() {
  ui->IDC_PSHIP_NEXT->setEnabled(static_cast<int>(Ships.size()) >= 1);
  ui->IDC_PSHIP_PREV->setEnabled(static_cast<int>(Ships.size()) >= 1);
  ui->IDC_PSHIP_COCKPIT->setEnabled(static_cast<int>(Ships.size()) >= 1);
  if (!Network_up) {
    ui->IDC_PSHIP_LOCK->setEnabled(false);
    ui->IDC_PSHIP_CHECKIN->setEnabled(false);
    return;
  }
  if (static_cast<int>(Ships.size()) < 1)
    return;

  if (Ships.is_unused(app.current_ship))
    app.current_ship = GetNextShip(app.current_ship);

  if (auto s = data())
  {
    ui->IDC_PSHIP_NAME_EDIT->setText(QString::fromStdString(s->name));

    if (m_lod == 0)
      ui->IDC_PSHIP_MODEL_NAME_EDIT->setText(QString::fromStdString(Poly_models[s->model_handle].name));
    else if (m_lod == 1)
    {
      if(s->med_render_handle == -1)
        ui->IDC_PSHIP_MODEL_NAME_EDIT->setText("No model defined");
      else
        ui->IDC_PSHIP_MODEL_NAME_EDIT->setText(QString::fromStdString(Poly_models[s->med_render_handle].name));
    } else {
      if(s->lo_render_handle == -1)
        ui->IDC_PSHIP_MODEL_NAME_EDIT->setText("No model defined");
      else
        ui->IDC_PSHIP_MODEL_NAME_EDIT->setText(QString::fromStdString(Poly_models[s->lo_render_handle].name));
    }

    if (m_lod == 0)
      ui->IDC_LOD_DISTANCE_EDIT->setText("0");
    else if (m_lod == 1)
      ui->IDC_LOD_DISTANCE_EDIT->setText(QString::number(s->med_lod_distance));
    else
      ui->IDC_LOD_DISTANCE_EDIT->setText(QString::number(s->lo_lod_distance));

    if(s->dying_model_handle == -1)
      ui->IDC_PSHIP_DYING_MODEL_NAME_EDIT->setText("<none>");
    else
      ui->IDC_PSHIP_DYING_MODEL_NAME_EDIT->setText(QString::fromStdString(Poly_models[s->dying_model_handle].name));

    ui->IDC_PSHIP_COCKPIT_EDIT->setText(QString::fromStdString(s->cockpit_name));
    ui->IDC_SHIP_ARMOR_EDIT->setText(QString::number(s->armor_scalar));

    if (!mng_FindTrackLock(s->name, PAGETYPE_SHIP) ) {
      ui->IDC_PSHIP_CHECKIN->setEnabled(false);
      ui->IDC_PSHIP_LOCK->setEnabled(true);
    } else {
      ui->IDC_PSHIP_CHECKIN->setEnabled(true);
      ui->IDC_PSHIP_LOCK->setEnabled(false);
    }

    ui->IDC_DEFAULTALLOW->setChecked(s->flags.default_allowed);

    {
      QComboBox *combo = ui->IDC_PSHIP_PULLDOWN;
      QSignalBlocker blocker(combo);
      combo->clear();
      for (int i = 0; i < static_cast<int>(Ships.size()); i++)
        if (Ships.is_used(i))
          combo->addItem(QString::fromStdString(Ships[i].name));
      combo->setCurrentText(QString::fromStdString(s->name));
    }

    if (m_lod == 0)
      ui->IDC_NOLOD->setEnabled(false);
    else if (m_lod == 1)
      ui->IDC_NOLOD->setEnabled(s->med_render_handle != -1);
    else
      ui->IDC_NOLOD->setEnabled(s->lo_render_handle != -1);

    ui->IDC_HIRES_RADIO->setChecked(m_lod == 0);
    ui->IDC_MEDRES_RADIO->setChecked(m_lod == 1);
    ui->IDC_LORES_RADIO->setChecked(m_lod == 2);
  }
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
  app.current_ship = ship_handle;
  RemapShips();
  updateDialog();
}

void WorldObjectsPlayerDialog::onPshipDelete() {
  if (auto s = data()) {
    const int n = app.current_ship;
    const std::optional<uint32_t> tl = mng_FindTrackLock(s->name, PAGETYPE_SHIP);
    if (!tl) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "This ship is not yours to delete.  Lock first.");
      return;
    }

    if (QMessageBox::question(this, "Delete ship", QString("Are you sure you want to delete this ship? %1").arg(QString::fromStdString(s->name))) !=
        QMessageBox::Yes)
      return;

    if (!mng_MakeLocker())
      return;

    mngs_Pagelock pl;
    pl.name = s->name;
    pl.pagetype = PAGETYPE_SHIP;

    if (mng_CheckIfPageOwned(&pl, TableUser.toStdString()) != 1) {
      mng_FreeTrackLock(*tl);
      Q_ASSERT(mng_DeletePage(s->name, PAGETYPE_SHIP, 1));
    } else {
      mng_FreeTrackLock(*tl);
      mng_DeletePage(s->name, PAGETYPE_SHIP, 0);
      mng_DeletePage(s->name, PAGETYPE_SHIP, 1);
      mng_DeletePagelock(s->name, PAGETYPE_SHIP);
    }

    app.current_ship = GetNextShip(n);
    if (s->model_handle >= 0 && s->model_handle < MAX_POLY_MODELS && Poly_models[s->model_handle].used)
      FreePolyModel(s->model_handle);
    if (s->dying_model_handle != -1)
      if (s->dying_model_handle >= 0 && s->dying_model_handle < MAX_POLY_MODELS && Poly_models[s->dying_model_handle].used)
        FreePolyModel(s->dying_model_handle);
    FreeShip(n);
    mng_EraseLocker();

    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Ship deleted.");
    RemapShips();
    updateDialog();
  }
}

void WorldObjectsPlayerDialog::onPshipLock() {
  if (auto s = data()) {
    mngs_Pagelock temp_pl;
    mngs_ship_page shippage;

    if (!mng_MakeLocker())
      return;

    temp_pl.name = s->name;
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
        if (mng_AssignShipPageToShip(&shippage, app.current_ship)) {
          if (!mng_ReplacePage(s->name, s->name, app.current_ship, PAGETYPE_SHIP, 1)) {
            QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was problem writing that page locally!");
            mng_EraseLocker();
            return;
          }
          QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Ship locked.");
        } else {
          QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was a problem loading this ship.");
        }
        mng_AllocTrackLock(s->name, PAGETYPE_SHIP);
        updateDialog();
      } else {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't find that ship in the table file!");
      }
    }
    mng_EraseLocker();
  }
}

void WorldObjectsPlayerDialog::onPshipCheckin() {
  if (auto s = data()) {
    mngs_Pagelock temp_pl;

    if (!mng_MakeLocker())
      return;

    temp_pl.name = s->name;
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
      } else if (!mng_ReplacePage(s->name, s->name, app.current_ship, PAGETYPE_SHIP, 0)) {
        QMessageBox::critical(this, "Error!", ErrorString);
      } else {
        std::filesystem::path srcname = LocalModelsDir / Poly_models[s->model_handle].name;
        std::filesystem::path destname = NetModelsDir / Poly_models[s->model_handle].name;
        std::filesystem::copy((srcname), (destname), std::filesystem::copy_options::overwrite_existing);
        if (s->dying_model_handle != -1) {
          srcname = LocalModelsDir / Poly_models[s->dying_model_handle].name;
          destname = NetModelsDir / Poly_models[s->dying_model_handle].name;
          std::filesystem::copy((srcname), (destname), std::filesystem::copy_options::overwrite_existing);
        }
        if (s->med_render_handle != -1) {
          srcname = LocalModelsDir / Poly_models[s->med_render_handle].name;
          destname = NetModelsDir / Poly_models[s->med_render_handle].name;
          std::filesystem::copy((srcname), (destname), std::filesystem::copy_options::overwrite_existing);
        }
        if (s->lo_render_handle != -1) {
          srcname = LocalModelsDir / Poly_models[s->lo_render_handle].name;
          destname = NetModelsDir / Poly_models[s->lo_render_handle].name;
          std::filesystem::copy((srcname), (destname), std::filesystem::copy_options::overwrite_existing);
        }
        if (!s->cockpit_name.empty()) {
          srcname = LocalMiscDir / s->cockpit_name;
          destname = NetMiscDir / s->cockpit_name;
          std::filesystem::copy((srcname), (destname), std::filesystem::copy_options::overwrite_existing);
        }

        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Ship checked in.");

        Q_ASSERT(mng_DeletePage(s->name, PAGETYPE_SHIP, 1) == 1);
        mng_EraseLocker();

        const std::optional<uint32_t> p = mng_FindTrackLock(s->name, PAGETYPE_SHIP);
        Q_ASSERT(p);
        mng_FreeTrackLock(*p);
        updateDialog();
      }
    }
    mng_EraseLocker();
  }
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
  app.current_ship = GetNextShip(app.current_ship);
  m_lod = 0;
  updateDialog();
}

void WorldObjectsPlayerDialog::onPshipPrev() {
  app.current_ship = GetPrevShip(app.current_ship);
  m_lod = 0;
  updateDialog();
}

void WorldObjectsPlayerDialog::onPshipPulldownChanged() {
  const int i = FindShipName(ui->IDC_PSHIP_PULLDOWN->currentText().toStdString());
  if (i != -1)
  {
    app.current_ship = i;
    updateDialog();
  }
}

void WorldObjectsPlayerDialog::onPshipLoadModel() {
  if (auto s = data()) {
    QString Current_model_dir; // get from settings
    const QString pathname =
        QFileDialog::getOpenFileName(this, "Select ship model", Current_model_dir, "Descent III files (*.pof *.oof)");
    if (pathname.isEmpty())
      return;

    const std::filesystem::path pathFs(pathname.toStdString());
    const int img_handle = static_cast<int>(LoadPolyModel(pathFs, 0).value_or(-1));
    if (img_handle < 0) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't open that animation/model file.");
      return;
    }

    if (m_lod == 0) {
      ChangeOldModelsForObjects(s->model_handle, img_handle);
      if (s->model_handle >= 0 && s->model_handle < MAX_POLY_MODELS && Poly_models[s->model_handle].used)
        FreePolyModel(s->model_handle);
      s->model_handle = img_handle;
    } else if (m_lod == 1) {
      if (s->med_render_handle >= 0 && s->med_render_handle < MAX_POLY_MODELS && Poly_models[s->med_render_handle].used)
        FreePolyModel(s->med_render_handle);
      s->med_render_handle = img_handle;
    } else {
      if (s->lo_render_handle >= 0 && s->lo_render_handle < MAX_POLY_MODELS && Poly_models[s->lo_render_handle].used)
        FreePolyModel(s->lo_render_handle);
      s->lo_render_handle = img_handle;
    }

    if (QMessageBox::question(this, "Are you sure?", "Would you like to clear the weapon battery info?") == QMessageBox::Yes) {
      WBClearInfo(s->static_wb.data());
    }

    std::filesystem::path curname = LocalModelsDir / Poly_models[img_handle].name;
    std::filesystem::copy(pathFs, (curname), std::filesystem::copy_options::overwrite_existing);
    updateDialog();
  }
}

void WorldObjectsPlayerDialog::onPshipDyingModel() {
  if (auto s = data()) {
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

    s->dying_model_handle = img_handle;
    std::filesystem::path curname = LocalModelsDir / Poly_models[s->dying_model_handle].name;
    std::filesystem::copy(pathFs, (curname), std::filesystem::copy_options::overwrite_existing);
    updateDialog();
  }
}

void WorldObjectsPlayerDialog::onNullDying() {
  if (auto s = data()) s->dying_model_handle = -1;
  updateDialog();
}

void WorldObjectsPlayerDialog::onEditWeapons() {
  // Ported in the player_weapons_dialog module (PlayerWeaponsDialog).
  extern void editPlayerWeapons(int shipHandle, QWidget *parent);
  editPlayerWeapons(app.current_ship, this);
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

  if (auto s = data()) {
    s->cockpit_name = cockpitFile;

    // Copy the picked file into the local misc dir under its relative name.
    const std::filesystem::path dest = LocalMiscDir / cockpitFile;
    std::filesystem::copy_file(picked, dest, std::filesystem::copy_options::overwrite_existing);

    updateDialog();
  }
}

void WorldObjectsPlayerDialog::onPshipEditPhysics() {
  if (auto s = data()) {
    PhysicsDialog dlg(this);
    dlg.setData(s->phys_info);
    if(dlg.exec() == QDialog::Accepted)
      s->phys_info = dlg.getData();
  }
}

void WorldObjectsPlayerDialog::onKillfocusName() {
  if (auto s = data()) {
    const std::optional<uint32_t> p = mng_FindTrackLock(s->name, PAGETYPE_SHIP);
    if (!p)
    {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You must lock this ship if you wish to change its name.");
      ui->IDC_PSHIP_NAME_EDIT->setText(QString::fromStdString(s->name));
      return;
    }

    std::string name = ui->IDC_PSHIP_NAME_EDIT->text().toStdString();
    if (FindShipName(name) != -1) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There already is a ship with that name...choose another name.");
      ui->IDC_PSHIP_NAME_EDIT->setText(QString::fromStdString(s->name));
      return;
    }

    if (!mng_MakeLocker())
      return;

    mngs_Pagelock pl;
    pl.name = s->name;
    pl.pagetype = PAGETYPE_SHIP;

    const int ret = mng_CheckIfPageOwned(&pl, TableUser.toStdString());
    if (ret < 0)
      QMessageBox::critical(this, "Error!", ErrorString);
    else if (ret == 1)
      mng_RenamePage(s->name, name, PAGETYPE_SHIP);
    else if (ret == 2) {
      std::string oldname;
      oldname = s->name;
      s->name = name;
      mng_ReplacePage(oldname, s->name, app.current_ship, PAGETYPE_SHIP, 1);
    } else if (ret == 0) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You don't own this page.  Get Jason now!");
      mng_FreeTrackLock(*p);
      return;
    }

    GlobalTrackLocks[*p].name = name;
    s->name = name;
    mng_EraseLocker();
    RemapShips();
    updateDialog();
  }
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
  if (auto s = data()) {
    if (m_lod == 0) {
      QMessageBox::warning(this, "No LOD", "You must have a hi-res model.");
      return;
    }
    if (m_lod == 1) {
      if (s->med_render_handle >= 0 && s->med_render_handle < MAX_POLY_MODELS && Poly_models[s->med_render_handle].used)
        FreePolyModel(s->med_render_handle);
      s->med_render_handle = -1;
    } else {
      if (s->lo_render_handle >= 0 && s->lo_render_handle < MAX_POLY_MODELS && Poly_models[s->lo_render_handle].used)
        FreePolyModel(s->lo_render_handle);
      s->lo_render_handle = -1;
    }
    updateDialog();
  }
}
