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

WorldTexturesDialog::WorldTexturesDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::WorldTexturesDialog)
{
  ui->setupUi(this);
  if (QPushButton *b = ui->IDC_ADD_NEW_HUGE)
    connect(b, &QPushButton::clicked, this, &WorldTexturesDialog::onAddNew);
  if (QPushButton *b = ui->IDC_ADD_NEW_SMALL)
    connect(b, &QPushButton::clicked, this, &WorldTexturesDialog::onAddNew);
  if (QPushButton *b = ui->IDC_ADD_NEW_TINY)
    connect(b, &QPushButton::clicked, this, &WorldTexturesDialog::onAddNew);
  if (QPushButton *b = ui->IDC_WTEXDLG_ADDNEW)
    connect(b, &QPushButton::clicked, this, &WorldTexturesDialog::onAddNew);
  if (QPushButton *b = ui->IDC_DELETE)
    connect(b, &QPushButton::clicked, this, &WorldTexturesDialog::onDelete);
  if (QPushButton *b = ui->IDC_LOCK)
    connect(b, &QPushButton::clicked, this, &WorldTexturesDialog::onLock);
  if (QPushButton *b = ui->IDC_CHECKIN)
    connect(b, &QPushButton::clicked, this, &WorldTexturesDialog::onCheckin);
  if (QPushButton *b = ui->IDC_RCS_STATUS)
    connect(b, &QPushButton::clicked, this, &WorldTexturesDialog::onCheckedOut);
  if (QPushButton *b = ui->IDC_OVERRIDE)
    connect(b, &QPushButton::clicked, this, &WorldTexturesDialog::onOverride);
  if (QPushButton *b = ui->IDC_TEXTURE_CHANGE_NAME)
    connect(b, &QPushButton::clicked, this, &WorldTexturesDialog::onChangeName);
  if (QPushButton *b = ui->IDC_LOAD_BITMAP)
    connect(b, &QPushButton::clicked, this, &WorldTexturesDialog::onLoadBitmap);
  if (QPushButton *b = ui->IDC_TEXTURE_CURRENT)
    connect(b, &QPushButton::clicked, this, &WorldTexturesDialog::onCurrent);
  if (QPushButton *b = ui->IDC_NEXT)
    connect(b, &QPushButton::clicked, this, &WorldTexturesDialog::onNext);
  if (QPushButton *b = ui->IDC_PREVIOUS)
    connect(b, &QPushButton::clicked, this, &WorldTexturesDialog::onPrev);

  if (QComboBox *combo = ui->IDC_TEX_LIST)
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this, &WorldTexturesDialog::onTexListChanged);
  if (QComboBox *combo = ui->IDC_TEXTURE_AMBIENT_SOUND_PULLDOWN)
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &WorldTexturesDialog::onAmbientSoundChanged);

  // Float fields.
  const struct {
    const char *name;
    float texture::*field;
  } fields[] = {
      {"IDC_REFLECT", &texture::reflectivity},
      {"IDC_RED_LIGHTING", &texture::r},
      {"IDC_GREEN_LIGHTING", &texture::g},
      {"IDC_BLUE_LIGHTING", &texture::b},
      {"IDC_SLIDEU", &texture::slide_u},
      {"IDC_SLIDEV", &texture::slide_v},
      {"IDC_ALPHA_EDIT", &texture::alpha},
      {"IDC_SPEED_EDIT", &texture::speed},
      {"IDC_TEXTURE_AMBIENT_SOUND_VOLUME", &texture::sound_volume},
  };
  for (const auto &f : fields) {
    if (QLineEdit *edit = findChild<QLineEdit*>(f.name))
      connect(edit, &QLineEdit::editingFinished, this, [this, f]() {
        const int n = D3EditState.texdlg_texture;
        if (n >= 0 && n < MAX_TEXTURES && GameTextures[n].used)
          GameTextures[n].*f.field = findChild<QLineEdit*>(f.name)->text().toFloat();
      });
  }

  if (QLineEdit *edit = ui->IDC_DAMAGE)
    connect(edit, &QLineEdit::editingFinished, this, [this]() {
      const int n = D3EditState.texdlg_texture;
      if (n >= 0 && n < MAX_TEXTURES && GameTextures[n].used)
        GameTextures[n].damage = ui->IDC_DAMAGE->text().toInt();
    });

  // Flag checkboxes.
  #define CONNECT_TEXTURE_FLAG(IDC, MEMBER)                                                        \
    connect(ui->IDC, &QCheckBox::toggled, this, [this](bool checked) {                            \
      const int n = D3EditState.texdlg_texture;                                                   \
      if (n < 0 || n >= MAX_TEXTURES || !GameTextures[n].used)                                     \
        return;                                                                                    \
      GameTextures[n].flags.MEMBER = checked;                                                      \
    });
  CONNECT_TEXTURE_FLAG(IDC_MINE_TEXTURE, mine);
  CONNECT_TEXTURE_FLAG(IDC_OBJECT_TEXTURE, object);
  CONNECT_TEXTURE_FLAG(IDC_TERRAIN_TEXTURE, terrain);
  CONNECT_TEXTURE_FLAG(IDC_EFFECT_TEXTURE, effect);
  CONNECT_TEXTURE_FLAG(IDC_HUD_COCKPIT_TEXTURE, hud_cockpit);
  CONNECT_TEXTURE_FLAG(IDC_LIGHT_TEXTURE, light);
  CONNECT_TEXTURE_FLAG(IDC_WATER, water);
  CONNECT_TEXTURE_FLAG(IDC_VOLATILE, volatile_flag);
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
      const int t = FindTextureName(GlobalTrackLocks[i].name);
      if (t != -1)
        mng_ReplacePage(GameTextures[t].name, GameTextures[t].name, t, PAGETYPE_TEXTURE, 1);
    }
  }
}

void WorldTexturesDialog::updateDialog() {
  const int n = D3EditState.texdlg_texture;

  if (QPushButton *next = ui->IDC_NEXT)
    next->setEnabled(Num_textures >= 1);
  if (QPushButton *prev = ui->IDC_PREVIOUS)
    prev->setEnabled(Num_textures >= 1);
  if (!Network_up) {
    for (const char *name : {"IDC_LOCK", "IDC_CHECKIN", "IDC_OVERRIDE"}) {
      if (auto *w = findChild<QPushButton*>(name))
        w->setEnabled(false);
    }
    return;
  }
  if (Num_textures < 1)
    return;


  ui->IDC_TEX_NUM->setText(QString::number(n));

  const struct {
    const char *name;
    float texture::*field;
  } fields[] = {
      {"IDC_REFLECT", &texture::reflectivity},
      {"IDC_RED_LIGHTING", &texture::r},
      {"IDC_GREEN_LIGHTING", &texture::g},
      {"IDC_BLUE_LIGHTING", &texture::b},
      {"IDC_SLIDEU", &texture::slide_u},
      {"IDC_SLIDEV", &texture::slide_v},
      {"IDC_ALPHA_EDIT", &texture::alpha},
      {"IDC_SPEED_EDIT", &texture::speed},
      {"IDC_TEXTURE_AMBIENT_SOUND_VOLUME", &texture::sound_volume},
  };
  for (const auto &f : fields)
    if (QLineEdit *edit = findChild<QLineEdit*>(f.name))
      edit->setText(QString::number(GameTextures[n].*f.field));

  ui->IDC_REFLECT->setText(QString::number(GameTextures[n].reflectivity));
  ui->IDC_RED_LIGHTING->setText(QString::number(GameTextures[n].r));
  ui->IDC_GREEN_LIGHTING->setText(QString::number(GameTextures[n].g));
  ui->IDC_BLUE_LIGHTING->setText(QString::number(GameTextures[n].b));
  ui->IDC_SLIDEU->setText(QString::number(GameTextures[n].slide_u));
  ui->IDC_SLIDEV->setText(QString::number(GameTextures[n].slide_v));
  ui->IDC_ALPHA_EDIT->setText(QString::number(GameTextures[n].alpha));
  ui->IDC_SPEED_EDIT->setText(QString::number(GameTextures[n].speed));
  ui->IDC_TEXTURE_AMBIENT_SOUND_VOLUME->setText(QString::number(GameTextures[n].sound_volume));

  ui->IDC_MINE_TEXTURE->setChecked(GameTextures[n].flags.mine);
  ui->IDC_OBJECT_TEXTURE->setChecked(GameTextures[n].flags.object);
  ui->IDC_TERRAIN_TEXTURE->setChecked(GameTextures[n].flags.terrain);
  ui->IDC_EFFECT_TEXTURE->setChecked(GameTextures[n].flags.effect);
  ui->IDC_HUD_COCKPIT_TEXTURE->setChecked(GameTextures[n].flags.hud_cockpit);
  ui->IDC_LIGHT_TEXTURE->setChecked(GameTextures[n].flags.light);
  ui->IDC_WATER->setChecked(GameTextures[n].flags.water);
  ui->IDC_VOLATILE->setChecked(GameTextures[n].flags.volatile_flag);
  ui->IDC_SATURATE->setChecked(GameTextures[n].flags.saturate);
  ui->IDC_MARBLE_CHECK->setChecked(GameTextures[n].flags.marble);
  ui->IDC_TEXTURE_FLY_THRU_CHECK->setChecked(GameTextures[n].flags.fly_thru);
  ui->IDC_FORCEFIELD->setChecked(GameTextures[n].flags.forcefield);
  ui->IDC_METAL_CHECK->setChecked(GameTextures[n].flags.metal);
  ui->IDC_PLASTIC_CHECK->setChecked(GameTextures[n].flags.plastic);
  ui->IDC_CHECK_ANIMATE->setChecked(GameTextures[n].flags.animated);
  ui->IDC_PING_PONG->setChecked(GameTextures[n].flags.ping_pong);
  ui->IDC_CHECK_TMAP2->setChecked(GameTextures[n].flags.tmap2);
  ui->IDC_CHECK_DESTROY->setChecked(GameTextures[n].flags.destroyable);
  ui->IDC_CHECK_BREAKABLE->setChecked(GameTextures[n].flags.breakable);
  ui->IDC_LAVA_CHECKBOX->setChecked(GameTextures[n].flags.lava);
  ui->IDC_RUBBLE_CHECKBOX->setChecked(GameTextures[n].flags.rubble);
  ui->IDC_SMOOTH_SPEC_CHECK->setChecked(GameTextures[n].flags.smooth_specular);

  if (QLineEdit *edit = ui->IDC_BITMAP_NAME) {
    const int bm = GameTextures[n].bm_handle;
    if (bm >= 0)
      edit->setText(QString::fromStdString(GameBitmaps[bm].name));
  }

  if (QPushButton *checkin = ui->IDC_CHECKIN) {
    if (mng_FindTrackLock(GameTextures[n].name, PAGETYPE_TEXTURE) == -1) {
      checkin->setEnabled(false);
      if (QPushButton *lock = ui->IDC_LOCK)
        lock->setEnabled(true);
    } else {
      checkin->setEnabled(true);
      if (QPushButton *lock = ui->IDC_LOCK)
        lock->setEnabled(false);
    }
  }

  if (QComboBox *combo = ui->IDC_TEX_LIST) {
    QSignalBlocker blocker(combo);
    combo->clear();
    for (int i = 0; i < MAX_TEXTURES; i++)
      if (GameTextures[i].used)
        combo->addItem(QString::fromStdString(GameTextures[i].name));
    combo->setCurrentText(QString::fromStdString(GameTextures[n].name));
  }

  populateSoundCombo(ui->IDC_TEXTURE_AMBIENT_SOUND_PULLDOWN, GameTextures[n].sound);
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
  GameTextures[handle].name = fileInfo.baseName().toStdString();
  GameTextures[handle].bm_handle = bm;
  mng_AllocTrackLock(GameTextures[handle].name, PAGETYPE_TEXTURE);
  D3EditState.texdlg_texture = handle;
  updateDialog();
}

void WorldTexturesDialog::onDelete() {
  const int n = D3EditState.texdlg_texture;
  const int tl = mng_FindTrackLock(GameTextures[n].name, PAGETYPE_TEXTURE);
  if (tl == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "This texture is not yours to delete.  Lock first.");
    return;
  }
  if (QMessageBox::question(this, "Delete texture",
                            QString("Are you sure you want to delete this texture? %1").arg(QString::fromStdString(GameTextures[n].name))) !=
      QMessageBox::Yes)
    return;
  if (!mng_MakeLocker())
    return;
  mngs_Pagelock pl;
  pl.name = GameTextures[n].name;
  pl.pagetype = PAGETYPE_TEXTURE;
  if (mng_CheckIfPageOwned(&pl, TableUser.toStdString()) != 1) {
    mng_FreeTrackLock(tl);
    Q_ASSERT(mng_DeletePage(GameTextures[n].name, PAGETYPE_TEXTURE, 1));
  } else {
    mng_FreeTrackLock(tl);
    mng_DeletePage(GameTextures[n].name, PAGETYPE_TEXTURE, 1);
    mng_DeletePage(GameTextures[n].name, PAGETYPE_TEXTURE, 0);
    mng_DeletePagelock(GameTextures[n].name, PAGETYPE_TEXTURE);
  }
  D3EditState.texdlg_texture = GetNextTexture(n);
  FreeTexture(n);
  mng_EraseLocker();
  QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Texture deleted.");
  updateDialog();
}

void WorldTexturesDialog::onLock() {
  const int n = D3EditState.texdlg_texture;
  if (!mng_MakeLocker())
    return;
  mngs_Pagelock temp_pl;
  mngs_texture_page texturepage;
  temp_pl.name = GameTextures[n].name;
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
        if (!mng_ReplacePage(GameTextures[n].name, GameTextures[n].name, n, PAGETYPE_TEXTURE, 1)) {
          QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was problem writing that page locally!");
          mng_EraseLocker();
          return;
        }
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Texture locked.");
      } else {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was a problem loading this texture.");
      }
      mng_AllocTrackLock(GameTextures[n].name, PAGETYPE_TEXTURE);
    } else {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't find that texture in the table file!");
    }
  }
  mng_EraseLocker();
  updateDialog();
}

void WorldTexturesDialog::onCheckin() {
  const int n = D3EditState.texdlg_texture;
  if (!mng_MakeLocker())
    return;
  mngs_Pagelock temp_pl;
  temp_pl.name = GameTextures[n].name;
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
    if (!mng_ReplacePage(GameTextures[n].name, GameTextures[n].name, n, PAGETYPE_TEXTURE, 0))
      QMessageBox::critical(this, "Error!", ErrorString);
    else {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Texture checked in.");
      Q_ASSERT(mng_DeletePage(GameTextures[n].name, PAGETYPE_TEXTURE, 1) == 1);
      mng_EraseLocker();
      const int p = mng_FindTrackLock(GameTextures[n].name, PAGETYPE_TEXTURE);
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
  const int n = D3EditState.texdlg_texture;
  mngs_Pagelock temp_pl;
  temp_pl.name = GameTextures[n].name;
  temp_pl.pagetype = PAGETYPE_TEXTURE;
  mng_OverrideToUnlocked(&temp_pl);
}

void WorldTexturesDialog::onChangeName() {
  const int n = D3EditState.texdlg_texture;
  const int p = mng_FindTrackLock(GameTextures[n].name, PAGETYPE_TEXTURE);
  if (p == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You must lock this texture if you wish to change its name.");
    return;
  }
  bool ok = false;
  const QString name = QInputDialog::getText(this, "Texture name", "Enter a new name for this texture:",
                                             QLineEdit::Normal, QString::fromStdString(GameTextures[n].name), &ok);
  if (!ok || name.isEmpty())
    return;
  if (FindTextureName(name.toStdString()) != -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "That name is taken, please choose another.");
    return;
  }
  const std::string newName = name.toStdString();
  GlobalTrackLocks[p].name = newName;
  GameTextures[n].name = newName;
  updateDialog();
}

void WorldTexturesDialog::onLoadBitmap() {
  const int n = D3EditState.texdlg_texture;
  QString Current_bitmap_dir; // get from settings
  const QString pathname =
      QFileDialog::getOpenFileName(this, "Load bitmap", Current_bitmap_dir, "Images (*.pcx *.tga *.bmp)");
  if (pathname.isEmpty())
    return;
  const std::filesystem::path pathFs(pathname.toStdString());
  const int bm = LoadTextureImage(pathFs, 0, 0, 0);
  if (bm < 0)
    return;
  GameTextures[n].bm_handle = bm;
  updateDialog();
}

void WorldTexturesDialog::onCurrent() {
  if (D3EditState.texdlg_texture >= 0)
    updateDialog();
}

void WorldTexturesDialog::onNext() {
  D3EditState.texdlg_texture = GetNextTexture(D3EditState.texdlg_texture);
  updateDialog();
}
void WorldTexturesDialog::onPrev() {
  D3EditState.texdlg_texture = GetPreviousTexture(D3EditState.texdlg_texture);
  updateDialog();
}

void WorldTexturesDialog::onTexListChanged() {
  QComboBox *combo = ui->IDC_TEX_LIST;
  const int i = FindTextureName(combo->currentText().toStdString());
  if (i == -1)
    return;
  D3EditState.texdlg_texture = i;
  updateDialog();
}

void WorldTexturesDialog::onAmbientSoundChanged() {
  const int n = D3EditState.texdlg_texture;
  GameTextures[n].sound = soundComboSelected(ui->IDC_TEXTURE_AMBIENT_SOUND_PULLDOWN);
}

