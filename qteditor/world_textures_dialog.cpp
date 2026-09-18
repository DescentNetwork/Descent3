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

#include "world_textures_dialog.h"
#include "ui_worldtextures.h"

#include <QMessageBox>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

#include <QFileInfo>

#include "gametexture.h"
#include "manage.h"
#include "sound_combo.h"
#include "ssl_lib.h"
#include "texpage.h"
#include "d3edit.h"

namespace {
// Returns the indexed texture, or a shared zero-initialized fallback when the
// index is stale/out of range. Mirrors the fixed-array behavior where GameTextures
// always had MAX_TEXTURES valid (mostly empty) slots.
texture &textureRef(int n) {
  static texture fallback{};
  return (n >= 0 && n < static_cast<int>(GameTextures.size())) ? GameTextures[n] : fallback;
}
} // namespace

WorldTexturesDialog::WorldTexturesDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::WorldTexturesDialog)
{
  ui->setupUi(this);
  connect(ui->IDC_ADD_NEW_HUGE, &QPushButton::clicked, this, &WorldTexturesDialog::onAddNew);
  connect(ui->IDC_ADD_NEW_SMALL, &QPushButton::clicked, this, &WorldTexturesDialog::onAddNew);
  connect(ui->IDC_ADD_NEW_TINY, &QPushButton::clicked, this, &WorldTexturesDialog::onAddNew);
  connect(ui->IDC_WTEXDLG_ADDNEW, &QPushButton::clicked, this, &WorldTexturesDialog::onAddNew);
  connect(ui->IDC_DELETE, &QPushButton::clicked, this, &WorldTexturesDialog::onDelete);
  connect(ui->IDC_LOCK, &QPushButton::clicked, this, &WorldTexturesDialog::onLock);
  connect(ui->IDC_CHECKIN, &QPushButton::clicked, this, &WorldTexturesDialog::onCheckin);
  connect(ui->IDC_RCS_STATUS, &QPushButton::clicked, this, &WorldTexturesDialog::onCheckedOut);
  connect(ui->IDC_OVERRIDE, &QPushButton::clicked, this, &WorldTexturesDialog::onOverride);
  connect(ui->IDC_TEXTURE_CHANGE_NAME, &QPushButton::clicked, this, &WorldTexturesDialog::onChangeName);
  connect(ui->IDC_LOAD_BITMAP, &QPushButton::clicked, this, &WorldTexturesDialog::onLoadBitmap);
  connect(ui->IDC_TEXTURE_CURRENT, &QPushButton::clicked, this, &WorldTexturesDialog::onCurrent);
  connect(ui->IDC_NEXT, &QPushButton::clicked, this, &WorldTexturesDialog::onNext);
  connect(ui->IDC_PREVIOUS, &QPushButton::clicked, this, &WorldTexturesDialog::onPrev);

  connect(ui->IDC_TEX_LIST, qOverload<int>(&QComboBox::currentIndexChanged), this, &WorldTexturesDialog::onTexListChanged);
      connect(ui->IDC_TEXTURE_AMBIENT_SOUND_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), this,
    &WorldTexturesDialog::onAmbientSoundChanged);

  connect(ui->IDC_REFLECT, &QLineEdit::editingFinished, this, [this]() {
    const int n = app.texdlg_texture;
    if (n < 0 || n >= static_cast<int>(GameTextures.size()) || !textureRef(n).used)
      return;
    textureRef(n).reflectivity = ui->IDC_REFLECT->text().toFloat();
  });
  connect(ui->IDC_RED_LIGHTING, &QLineEdit::editingFinished, this, [this]() {
    const int n = app.texdlg_texture;
    if (n < 0 || n >= static_cast<int>(GameTextures.size()) || !textureRef(n).used)
      return;
    textureRef(n).r = ui->IDC_RED_LIGHTING->text().toFloat();
  });
  connect(ui->IDC_GREEN_LIGHTING, &QLineEdit::editingFinished, this, [this]() {
    const int n = app.texdlg_texture;
    if (n < 0 || n >= static_cast<int>(GameTextures.size()) || !textureRef(n).used)
      return;
    textureRef(n).g = ui->IDC_GREEN_LIGHTING->text().toFloat();
  });
  connect(ui->IDC_BLUE_LIGHTING, &QLineEdit::editingFinished, this, [this]() {
    const int n = app.texdlg_texture;
    if (n < 0 || n >= static_cast<int>(GameTextures.size()) || !textureRef(n).used)
      return;
    textureRef(n).b = ui->IDC_BLUE_LIGHTING->text().toFloat();
  });
  connect(ui->IDC_SLIDEU, &QLineEdit::editingFinished, this, [this]() {
    const int n = app.texdlg_texture;
    if (n < 0 || n >= static_cast<int>(GameTextures.size()) || !textureRef(n).used)
      return;
    textureRef(n).slide_u = ui->IDC_SLIDEU->text().toFloat();
  });
  connect(ui->IDC_SLIDEV, &QLineEdit::editingFinished, this, [this]() {
    const int n = app.texdlg_texture;
    if (n < 0 || n >= static_cast<int>(GameTextures.size()) || !textureRef(n).used)
      return;
    textureRef(n).slide_v = ui->IDC_SLIDEV->text().toFloat();
  });
  connect(ui->IDC_ALPHA_EDIT, &QLineEdit::editingFinished, this, [this]() {
    const int n = app.texdlg_texture;
    if (n < 0 || n >= static_cast<int>(GameTextures.size()) || !textureRef(n).used)
      return;
    textureRef(n).alpha = ui->IDC_ALPHA_EDIT->text().toFloat();
  });
  connect(ui->IDC_SPEED_EDIT, &QLineEdit::editingFinished, this, [this]() {
    const int n = app.texdlg_texture;
    if (n < 0 || n >= static_cast<int>(GameTextures.size()) || !textureRef(n).used)
      return;
    textureRef(n).speed = ui->IDC_SPEED_EDIT->text().toFloat();
  });
  connect(ui->IDC_TEXTURE_AMBIENT_SOUND_VOLUME, &QLineEdit::editingFinished, this, [this]() {
    const int n = app.texdlg_texture;
    if (n < 0 || n >= static_cast<int>(GameTextures.size()) || !textureRef(n).used)
      return;
    textureRef(n).sound_volume = ui->IDC_TEXTURE_AMBIENT_SOUND_VOLUME->text().toFloat();
  });

      connect(ui->IDC_DAMAGE, &QLineEdit::editingFinished, this, [this]() {
    const int n = app.texdlg_texture;
    if (n >= 0 && n < static_cast<int>(GameTextures.size()) && textureRef(n).used)
    textureRef(n).damage = ui->IDC_DAMAGE->text().toInt();
    });

  // Flag checkboxes.
  #define CONNECT_TEXTURE_FLAG(IDC, MEMBER)                                                        \
    connect(ui->IDC, &QCheckBox::toggled, this, [this](bool checked) {                            \
      const int n = app.texdlg_texture;                                                   \
      if (n < 0 || n >= static_cast<int>(GameTextures.size()) || !textureRef(n).used)                                     \
        return;                                                                                    \
      textureRef(n).flags.MEMBER = checked;                                                      \
    });
  CONNECT_TEXTURE_FLAG(IDC_MINE_TEXTURE, mine);
  CONNECT_TEXTURE_FLAG(IDC_OBJECT_TEXTURE, object);
  CONNECT_TEXTURE_FLAG(IDC_TERRAIN_TEXTURE, terrain);
  CONNECT_TEXTURE_FLAG(IDC_EFFECT_TEXTURE, effect);
  CONNECT_TEXTURE_FLAG(IDC_HUD_COCKPIT_TEXTURE, hud_cockpit);
  CONNECT_TEXTURE_FLAG(IDC_LIGHT_TEXTURE, light);
  CONNECT_TEXTURE_FLAG(IDC_WATER, water);
  CONNECT_TEXTURE_FLAG(IDC_VOLATILE, explosive);
  CONNECT_TEXTURE_FLAG(IDC_SATURATE, saturate);
  CONNECT_TEXTURE_FLAG(IDC_MARBLE_CHECK, marble);
  CONNECT_TEXTURE_FLAG(IDC_TEXTURE_FLY_THRU_CHECK, fly_thru);
  CONNECT_TEXTURE_FLAG(IDC_FORCEFIELD, forcefield);
  CONNECT_TEXTURE_FLAG(IDC_METAL_CHECK, metal);
  CONNECT_TEXTURE_FLAG(IDC_PLASTIC_CHECK, plastic);
  CONNECT_TEXTURE_FLAG(IDC_CHECK_ANIMATE, animated);
  CONNECT_TEXTURE_FLAG(IDC_PING_PONG, ping_pong);
  CONNECT_TEXTURE_FLAG(IDC_CHECK_TMAP2, tmap2);
  CONNECT_TEXTURE_FLAG(IDC_CHECK_DESTROY, destroyable);
  CONNECT_TEXTURE_FLAG(IDC_CHECK_BREAKABLE, breakable);
  CONNECT_TEXTURE_FLAG(IDC_LAVA_CHECKBOX, lava);
  CONNECT_TEXTURE_FLAG(IDC_RUBBLE_CHECKBOX, rubble);
  CONNECT_TEXTURE_FLAG(IDC_SMOOTH_SPEC_CHECK, smooth_specular);
  #undef CONNECT_TEXTURE_FLAG

  updateDialog();
}

WorldTexturesDialog::~WorldTexturesDialog() { saveTexturesOnClose(); }

void WorldTexturesDialog::saveTexturesOnClose() {
  if (!Network_up)
    return;
  for (int i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used == 1 && GlobalTrackLocks[i].pagetype == PAGETYPE_TEXTURE) {
      const std::optional<uint32_t> t = FindTextureName(GlobalTrackLocks[i].name);
      if (t)
        mng_ReplacePage(GameTextures[*t].name, GameTextures[*t].name, *t, PAGETYPE_TEXTURE, 1);
    }
  }
}

void WorldTexturesDialog::updateDialog() {
  const int n = app.texdlg_texture;

  if (n >= 0 && n < static_cast<int>(GameTextures.size()))
  {
    ui->IDC_NEXT->setEnabled(static_cast<int>(GameTextures.size()));
    ui->IDC_PREVIOUS->setEnabled(static_cast<int>(GameTextures.size()));
    if (!Network_up) {
      ui->IDC_LOCK->setEnabled(false);
      ui->IDC_CHECKIN->setEnabled(false);
      ui->IDC_OVERRIDE->setEnabled(false);
    }

    ui->IDC_TEX_NUM->setText(QString::number(n));

    ui->IDC_REFLECT->setText(QString::number(textureRef(n).reflectivity));
    ui->IDC_RED_LIGHTING->setText(QString::number(textureRef(n).r));
    ui->IDC_GREEN_LIGHTING->setText(QString::number(textureRef(n).g));
    ui->IDC_BLUE_LIGHTING->setText(QString::number(textureRef(n).b));
    ui->IDC_SLIDEU->setText(QString::number(textureRef(n).slide_u));
    ui->IDC_SLIDEV->setText(QString::number(textureRef(n).slide_v));
    ui->IDC_ALPHA_EDIT->setText(QString::number(textureRef(n).alpha));
    ui->IDC_SPEED_EDIT->setText(QString::number(textureRef(n).speed));
    ui->IDC_TEXTURE_AMBIENT_SOUND_VOLUME->setText(QString::number(textureRef(n).sound_volume));

    ui->IDC_MINE_TEXTURE->setChecked(textureRef(n).flags.mine);
    ui->IDC_OBJECT_TEXTURE->setChecked(textureRef(n).flags.object);
    ui->IDC_TERRAIN_TEXTURE->setChecked(textureRef(n).flags.terrain);
    ui->IDC_EFFECT_TEXTURE->setChecked(textureRef(n).flags.effect);
    ui->IDC_HUD_COCKPIT_TEXTURE->setChecked(textureRef(n).flags.hud_cockpit);
    ui->IDC_LIGHT_TEXTURE->setChecked(textureRef(n).flags.light);
    ui->IDC_WATER->setChecked(textureRef(n).flags.water);
    ui->IDC_VOLATILE->setChecked(textureRef(n).flags.explosive);
    ui->IDC_SATURATE->setChecked(textureRef(n).flags.saturate);
    ui->IDC_MARBLE_CHECK->setChecked(textureRef(n).flags.marble);
    ui->IDC_TEXTURE_FLY_THRU_CHECK->setChecked(textureRef(n).flags.fly_thru);
    ui->IDC_FORCEFIELD->setChecked(textureRef(n).flags.forcefield);
    ui->IDC_METAL_CHECK->setChecked(textureRef(n).flags.metal);
    ui->IDC_PLASTIC_CHECK->setChecked(textureRef(n).flags.plastic);
    ui->IDC_CHECK_ANIMATE->setChecked(textureRef(n).flags.animated);
    ui->IDC_PING_PONG->setChecked(textureRef(n).flags.ping_pong);
    ui->IDC_CHECK_TMAP2->setChecked(textureRef(n).flags.tmap2);
    ui->IDC_CHECK_DESTROY->setChecked(textureRef(n).flags.destroyable);
    ui->IDC_CHECK_BREAKABLE->setChecked(textureRef(n).flags.breakable);
    ui->IDC_LAVA_CHECKBOX->setChecked(textureRef(n).flags.lava);
    ui->IDC_RUBBLE_CHECKBOX->setChecked(textureRef(n).flags.rubble);
    ui->IDC_SMOOTH_SPEC_CHECK->setChecked(textureRef(n).flags.smooth_specular);

    if (const int bm = textureRef(n).bm_handle; bm >= 0)
      ui->IDC_BITMAP_NAME->setText(QString::fromStdString(GameBitmaps[bm].name));

    if (Network_up && !mng_FindTrackLock(textureRef(n).name, PAGETYPE_TEXTURE))
    {
      ui->IDC_CHECKIN->setEnabled(false);
      ui->IDC_LOCK->setEnabled(true);
    } else {
      ui->IDC_CHECKIN->setEnabled(false);
      ui->IDC_LOCK->setEnabled(false);
    }

    {
      QComboBox *combo = ui->IDC_TEX_LIST;
      QSignalBlocker blocker(combo);
      combo->clear();
      for (int i = 0; i < static_cast<int>(GameTextures.size()); i++)
        if (GameTextures[i].used)
          combo->addItem(QString::fromStdString(GameTextures[i].name));
      combo->setCurrentText(QString::fromStdString(textureRef(n).name));
    }

    populateSoundCombo(ui->IDC_TEXTURE_AMBIENT_SOUND_PULLDOWN, textureRef(n).sound);
  }
}

void WorldTexturesDialog::onAddNew() {
  if (!Network_up) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Sorry babe, the network is down.  This action is a no-no.\n");
    return;
  }

  QString Current_bitmap_dir; // get from settings

  const QString pathname =
      QFileDialog::getOpenFileName(this, "Load bitmap", Current_bitmap_dir, "Images (*.pcx *.tga *.bmp)");
  if (pathname.isEmpty())
    return;
  const std::filesystem::path pathFs(pathname.toStdString());
  const int bm = LoadTextureImage(pathFs, 0, 0, 0);
  if (bm < 0) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't load that bitmap.");
    return;
  }
  QFileInfo fileInfo(pathname);
  const int handle = AllocTexture();
  if (handle == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot add texture: no free slots.");
    return;
  }
  textureRef(handle).name = fileInfo.baseName().toStdString();
  textureRef(handle).bm_handle = bm;
  mng_AllocTrackLock(textureRef(handle).name, PAGETYPE_TEXTURE);
  app.texdlg_texture = handle;
  updateDialog();
}

void WorldTexturesDialog::onDelete() {
  const int n = app.texdlg_texture;
  const std::optional<uint32_t> tl = mng_FindTrackLock(textureRef(n).name, PAGETYPE_TEXTURE);
  if (!tl) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "This texture is not yours to delete.  Lock first.");
    return;
  }
  if (QMessageBox::question(this, "Delete texture",
                            QString("Are you sure you want to delete this texture? %1").arg(QString::fromStdString(textureRef(n).name))) !=
      QMessageBox::Yes)
    return;
  if (!mng_MakeLocker())
    return;
  mngs_Pagelock pl;
  pl.name = textureRef(n).name;
  pl.pagetype = PAGETYPE_TEXTURE;
  if (mng_CheckIfPageOwned(&pl, TableUser.toStdString()) != 1) {
    mng_FreeTrackLock(*tl);
    Q_ASSERT(mng_DeletePage(textureRef(n).name, PAGETYPE_TEXTURE, 1));
  } else {
    mng_FreeTrackLock(*tl);
    mng_DeletePage(textureRef(n).name, PAGETYPE_TEXTURE, 1);
    mng_DeletePage(textureRef(n).name, PAGETYPE_TEXTURE, 0);
    mng_DeletePagelock(textureRef(n).name, PAGETYPE_TEXTURE);
  }
  app.texdlg_texture = GetNextTexture(n);
  FreeTexture(n);
  mng_EraseLocker();
  QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Texture deleted.");
  updateDialog();
}

void WorldTexturesDialog::onLock() {
  const int n = app.texdlg_texture;
  if (!mng_MakeLocker())
    return;
  mngs_Pagelock temp_pl;
  mngs_texture_page texturepage;
  temp_pl.name = textureRef(n).name;
  temp_pl.pagetype = PAGETYPE_TEXTURE;
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
    }
    if (mng_FindSpecificTexPage(temp_pl.name, &texturepage)) {
      if (mng_AssignTexPageToTexture(&texturepage, n)) {
        if (!mng_ReplacePage(textureRef(n).name, textureRef(n).name, n, PAGETYPE_TEXTURE, 1)) {
          QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was problem writing that page locally!");
          mng_EraseLocker();
          return;
        }
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Texture locked.");
      } else {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was a problem loading this texture.");
      }
      mng_AllocTrackLock(textureRef(n).name, PAGETYPE_TEXTURE);
    } else {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't find that texture in the table file!");
    }
  }
  mng_EraseLocker();
  updateDialog();
}

void WorldTexturesDialog::onCheckin() {
  const int n = app.texdlg_texture;
  if (!mng_MakeLocker())
    return;
  mngs_Pagelock temp_pl;
  temp_pl.name = textureRef(n).name;
  temp_pl.pagetype = PAGETYPE_TEXTURE;
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
    }
    if (!mng_ReplacePage(textureRef(n).name, textureRef(n).name, n, PAGETYPE_TEXTURE, 0))
      QMessageBox::critical(this, "Error!", ErrorString);
    else {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Texture checked in.");
      Q_ASSERT(mng_DeletePage(textureRef(n).name, PAGETYPE_TEXTURE, 1) == 1);
      mng_EraseLocker();
      const int p = mng_FindTrackLock(textureRef(n).name, PAGETYPE_TEXTURE).value_or(-1);
      Q_ASSERT(p != -1);
      mng_FreeTrackLock(p);
    }
  }
  mng_EraseLocker();
  updateDialog();
}

void WorldTexturesDialog::onCheckedOut() {
  QString str = QString("User %1 has these textures held locally:\n\n").arg(TableUser);
  int total = 0;
  for (int i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used && GlobalTrackLocks[i].pagetype == PAGETYPE_TEXTURE) {
      str += QString::fromStdString(GlobalTrackLocks[i].name);
      str += "\n";
      total++;
    }
  }
  if (total != 0)
    QMessageBox::information(this, "Textures", str);
}

void WorldTexturesDialog::onOverride() {
  const int n = app.texdlg_texture;
  mngs_Pagelock temp_pl;
  temp_pl.name = textureRef(n).name;
  temp_pl.pagetype = PAGETYPE_TEXTURE;
  mng_OverrideToUnlocked(&temp_pl);
}

void WorldTexturesDialog::onChangeName() {
  const std::optional<uint32_t> p = mng_FindTrackLock(textureRef(app.texdlg_texture).name, PAGETYPE_TEXTURE);
  if (!p) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You must lock this texture if you wish to change its name.");
    return;
  }
  bool ok = false;
  const QString name = QInputDialog::getText(this, "Texture name", "Enter a new name for this texture:",
                                             QLineEdit::Normal, QString::fromStdString(textureRef(app.texdlg_texture).name), &ok);
  if (!ok || name.isEmpty())
    return;
  if (!FindTextureName(name.toStdString())) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "That name is taken, please choose another.");
    return;
  }
  const std::string newName = name.toStdString();
  GlobalTrackLocks[*p].name = newName;
  textureRef(app.texdlg_texture).name = newName;
  updateDialog();
}

void WorldTexturesDialog::onLoadBitmap() {
  const int n = app.texdlg_texture;
  QString Current_bitmap_dir; // get from settings
  const QString pathname =
      QFileDialog::getOpenFileName(this, "Load bitmap", Current_bitmap_dir, "Images (*.pcx *.tga *.bmp)");
  if (pathname.isEmpty())
    return;
  const std::filesystem::path pathFs(pathname.toStdString());
  const int bm = LoadTextureImage(pathFs, 0, 0, 0);
  if (bm < 0)
    return;
  textureRef(n).bm_handle = bm;
  updateDialog();
}

void WorldTexturesDialog::onCurrent() {
  if (app.texdlg_texture >= 0)
    updateDialog();
}

void WorldTexturesDialog::onNext() {
  app.texdlg_texture = GetNextTexture(app.texdlg_texture);
  updateDialog();
}
void WorldTexturesDialog::onPrev() {
  app.texdlg_texture = GetPreviousTexture(app.texdlg_texture);
  updateDialog();
}

void WorldTexturesDialog::onTexListChanged()
{
  if(const std::optional<uint32_t> i = FindTextureName(ui->IDC_TEX_LIST->currentText().toStdString()); i)
  {
    app.texdlg_texture = *i;
    updateDialog();
  }
}

void WorldTexturesDialog::onAmbientSoundChanged()
{
  GameTextures[app.texdlg_texture].sound = soundComboSelected(ui->IDC_TEXTURE_AMBIENT_SOUND_PULLDOWN);
}

