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

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

#include "gametexture.h"
#include "manage.h"
#include "sound_combo.h"
#include "ssl_lib.h"
#include "texpage.h"
#include "d3edit.h"

// Returns the current texture, or nullopt when the index is stale or the slot is unused.
optref<texture> WorldTexturesDialog::data(void)
{
  if (!app.texdlg_texture.has_value()) return std::nullopt;
  uint32_t idx = *app.texdlg_texture;
  if (idx >= static_cast<uint32_t>(GameTextures.size()) || GameTextures.is_unused(idx))
    return std::nullopt;
  return GameTextures[idx];
}

WorldTexturesDialog::WorldTexturesDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::WorldTexturesDialog) {
  ui->setupUi(this);
  connect(ui->IDOK, &QPushButton::clicked, this, &QDialog::accept);
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
  connect(ui->IDC_TEXTURE_CURRENT, &QPushButton::clicked, [this]() {
    if (app.texdlg_texture) updateDialog();
  });
  connect(ui->IDC_NEXT, &QPushButton::clicked, this, &WorldTexturesDialog::onNext);
  connect(ui->IDC_PREVIOUS, &QPushButton::clicked, this, &WorldTexturesDialog::onPrev);

  connect(ui->IDC_TEX_LIST, qOverload<int>(&QComboBox::currentIndexChanged), this, &WorldTexturesDialog::onTexListChanged);

  bindEdits();
  bindChecks();
  bindCombos();

  updateDialog();
}

WorldTexturesDialog::~WorldTexturesDialog() { saveTexturesOnClose(); }

void WorldTexturesDialog::saveTexturesOnClose() {
  if (!Network_up)
    return;
  for (int i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used == 1 && GlobalTrackLocks[i].pagetype == page_type::texture) {
      const index_t t = FindTextureName(GlobalTrackLocks[i].name);
      if (t)
        mng_ReplacePage(GameTextures[*t].name, GameTextures[*t].name, *t, page_type::texture, 1);
    }
  }
}

void WorldTexturesDialog::bindEdits() {
  connect(ui->IDC_REFLECT, &QLineEdit::editingFinished, [this]() {
    if (auto t = data()) t->reflectivity = ui->IDC_REFLECT->text().toFloat();
  });
  connect(ui->IDC_RED_LIGHTING, &QLineEdit::editingFinished, [this]() {
    if (auto t = data()) t->r = ui->IDC_RED_LIGHTING->text().toFloat();
  });
  connect(ui->IDC_GREEN_LIGHTING, &QLineEdit::editingFinished, [this]() {
    if (auto t = data()) t->g = ui->IDC_GREEN_LIGHTING->text().toFloat();
  });
  connect(ui->IDC_BLUE_LIGHTING, &QLineEdit::editingFinished, [this]() {
    if (auto t = data()) t->b = ui->IDC_BLUE_LIGHTING->text().toFloat();
  });
  connect(ui->IDC_SLIDEU, &QLineEdit::editingFinished, [this]() {
    if (auto t = data()) t->slide_u = ui->IDC_SLIDEU->text().toFloat();
  });
  connect(ui->IDC_SLIDEV, &QLineEdit::editingFinished, [this]() {
    if (auto t = data()) t->slide_v = ui->IDC_SLIDEV->text().toFloat();
  });
  connect(ui->IDC_ALPHA_EDIT, &QLineEdit::editingFinished, [this]() {
    if (auto t = data()) t->alpha = ui->IDC_ALPHA_EDIT->text().toFloat();
  });
  connect(ui->IDC_SPEED_EDIT, &QLineEdit::editingFinished, [this]() {
    if (auto t = data()) t->speed = ui->IDC_SPEED_EDIT->text().toFloat();
  });
  connect(ui->IDC_TEXTURE_AMBIENT_SOUND_VOLUME, &QLineEdit::editingFinished, [this]() {
    if (auto t = data()) t->sound_volume = ui->IDC_TEXTURE_AMBIENT_SOUND_VOLUME->text().toFloat();
  });
  connect(ui->IDC_DAMAGE, &QLineEdit::editingFinished, [this]() {
    if (auto t = data()) t->damage = ui->IDC_DAMAGE->text().toInt();
  });
}

void WorldTexturesDialog::bindChecks() {
  if (data())
  {
    auto tf = [this]() -> texture_flags_t& { static texture_flags_t dummy_tf; return data() ? data()->flags : dummy_tf; };

    connect(ui->IDC_MINE_TEXTURE, &QCheckBox::toggled, [&tf](bool checked) { tf().mine = checked; });
    connect(ui->IDC_OBJECT_TEXTURE, &QCheckBox::toggled, [&tf](bool checked) { tf().object = checked; });
    connect(ui->IDC_TERRAIN_TEXTURE, &QCheckBox::toggled, [&tf](bool checked) { tf().terrain = checked; });
    connect(ui->IDC_EFFECT_TEXTURE, &QCheckBox::toggled, [&tf](bool checked) { tf().effect = checked; });
    connect(ui->IDC_HUD_COCKPIT_TEXTURE, &QCheckBox::toggled, [&tf](bool checked) { tf().hud_cockpit = checked; });
    connect(ui->IDC_LIGHT_TEXTURE, &QCheckBox::toggled, [&tf](bool checked) { tf().light = checked; });
    connect(ui->IDC_WATER, &QCheckBox::toggled, [&tf](bool checked) { tf().water = checked; });
    connect(ui->IDC_VOLATILE, &QCheckBox::toggled, [&tf](bool checked) { tf().explosive = checked; });
    connect(ui->IDC_SATURATE, &QCheckBox::toggled, [&tf](bool checked) { tf().saturate = checked; });
    connect(ui->IDC_MARBLE_CHECK, &QCheckBox::toggled, [&tf](bool checked) { tf().marble = checked; });
    connect(ui->IDC_TEXTURE_FLY_THRU_CHECK, &QCheckBox::toggled, [&tf](bool checked) { tf().fly_thru = checked; });
    connect(ui->IDC_FORCEFIELD, &QCheckBox::toggled, [&tf](bool checked) { tf().forcefield = checked; });
    connect(ui->IDC_METAL_CHECK, &QCheckBox::toggled, [&tf](bool checked) { tf().metal = checked; });
    connect(ui->IDC_PLASTIC_CHECK, &QCheckBox::toggled, [&tf](bool checked) { tf().plastic = checked; });
    connect(ui->IDC_CHECK_ANIMATE, &QCheckBox::toggled, [&tf](bool checked) { tf().animated = checked; });
    connect(ui->IDC_PING_PONG, &QCheckBox::toggled, [&tf](bool checked) { tf().ping_pong = checked; });
    connect(ui->IDC_CHECK_TMAP2, &QCheckBox::toggled, [&tf](bool checked) { tf().tmap2 = checked; });
    connect(ui->IDC_CHECK_DESTROY, &QCheckBox::toggled, [&tf](bool checked) { tf().destroyable = checked; });
    connect(ui->IDC_CHECK_BREAKABLE, &QCheckBox::toggled, [&tf](bool checked) { tf().breakable = checked; });
    connect(ui->IDC_LAVA_CHECKBOX, &QCheckBox::toggled, [&tf](bool checked) { tf().lava = checked; });
    connect(ui->IDC_RUBBLE_CHECKBOX, &QCheckBox::toggled, [&tf](bool checked) { tf().rubble = checked; });
    connect(ui->IDC_SMOOTH_SPEC_CHECK, &QCheckBox::toggled, [&tf](bool checked) { tf().smooth_specular = checked; });
  }
}

void WorldTexturesDialog::bindCombos() {
  connect(ui->IDC_TEXTURE_AMBIENT_SOUND_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), [this]() {
    if (auto t = data()) t->sound = soundComboSelected(ui->IDC_TEXTURE_AMBIENT_SOUND_PULLDOWN);
  });
}

void WorldTexturesDialog::updateDialog() {
  ui->IDC_NEXT->setEnabled(static_cast<int>(GameTextures.size()) >= 1);
  ui->IDC_PREVIOUS->setEnabled(static_cast<int>(GameTextures.size()) >= 1);

  if (!Network_up)
  {
    ui->IDC_LOCK->setEnabled(false);
    ui->IDC_CHECKIN->setEnabled(false);
    ui->IDC_OVERRIDE->setEnabled(false);
  }
  else if (auto t = data())
  {
    if (app.texdlg_texture) ui->IDC_TEX_NUM->setText(QString::number(*app.texdlg_texture));

    ui->IDC_REFLECT->setText(QString::number(t->reflectivity));
    ui->IDC_RED_LIGHTING->setText(QString::number(t->r));
    ui->IDC_GREEN_LIGHTING->setText(QString::number(t->g));
    ui->IDC_BLUE_LIGHTING->setText(QString::number(t->b));
    ui->IDC_SLIDEU->setText(QString::number(t->slide_u));
    ui->IDC_SLIDEV->setText(QString::number(t->slide_v));
    ui->IDC_ALPHA_EDIT->setText(QString::number(t->alpha));
    ui->IDC_SPEED_EDIT->setText(QString::number(t->speed));
    ui->IDC_TEXTURE_AMBIENT_SOUND_VOLUME->setText(QString::number(t->sound_volume));

    const texture_flags_t& tf = t->flags;

    ui->IDC_MINE_TEXTURE->setChecked(tf.mine);
    ui->IDC_OBJECT_TEXTURE->setChecked(tf.object);
    ui->IDC_TERRAIN_TEXTURE->setChecked(tf.terrain);
    ui->IDC_EFFECT_TEXTURE->setChecked(tf.effect);
    ui->IDC_HUD_COCKPIT_TEXTURE->setChecked(tf.hud_cockpit);
    ui->IDC_LIGHT_TEXTURE->setChecked(tf.light);
    ui->IDC_WATER->setChecked(tf.water);
    ui->IDC_VOLATILE->setChecked(tf.explosive);
    ui->IDC_SATURATE->setChecked(tf.saturate);
    ui->IDC_MARBLE_CHECK->setChecked(tf.marble);
    ui->IDC_TEXTURE_FLY_THRU_CHECK->setChecked(tf.fly_thru);
    ui->IDC_FORCEFIELD->setChecked(tf.forcefield);
    ui->IDC_METAL_CHECK->setChecked(tf.metal);
    ui->IDC_PLASTIC_CHECK->setChecked(tf.plastic);
    ui->IDC_CHECK_ANIMATE->setChecked(tf.animated);
    ui->IDC_PING_PONG->setChecked(tf.ping_pong);
    ui->IDC_CHECK_TMAP2->setChecked(tf.tmap2);
    ui->IDC_CHECK_DESTROY->setChecked(tf.destroyable);
    ui->IDC_CHECK_BREAKABLE->setChecked(tf.breakable);
    ui->IDC_LAVA_CHECKBOX->setChecked(tf.lava);
    ui->IDC_RUBBLE_CHECKBOX->setChecked(tf.rubble);
    ui->IDC_SMOOTH_SPEC_CHECK->setChecked(tf.smooth_specular);

    if (const int bm = t->bm_handle; bm >= 0)
      ui->IDC_BITMAP_NAME->setText(QString::fromStdString(GameBitmaps[bm].name));

    if (!mng_FindTrackLock(t->name, page_type::texture)) {
      ui->IDC_CHECKIN->setEnabled(false);
      ui->IDC_LOCK->setEnabled(true);
    } else {
      ui->IDC_CHECKIN->setEnabled(true);
      ui->IDC_LOCK->setEnabled(false);
    }

    {
      QComboBox *combo = ui->IDC_TEX_LIST;
      QSignalBlocker blocker(combo);
      combo->clear();
      for (int i = 0; i < static_cast<int>(GameTextures.size()); i++)
        if (GameTextures.is_used(i))
          combo->addItem(QString::fromStdString(GameTextures[i].name));
      combo->setCurrentText(QString::fromStdString(t->name));
    }

    populateSoundCombo(ui->IDC_TEXTURE_AMBIENT_SOUND_PULLDOWN, t->sound);
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
  const int bm = LoadTextureImage(pathFs, std::nullopt, texture_size_type::none, 0);
  if (bm < 0) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't load that bitmap.");
    return;
  }
  QFileInfo fileInfo(pathname);
  const index_t handle = AllocTexture();
  if (!handle) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot add texture: no free slots.");
    return;
  }
  GameTextures[*handle].name = fileInfo.baseName().toStdString();
  GameTextures[*handle].bm_handle = bm;
  mng_AllocTrackLock(GameTextures[*handle].name, page_type::texture);
  app.texdlg_texture = *handle;
  updateDialog();
}

void WorldTexturesDialog::onDelete() {
  if (auto t = data())
  {
    const uint32_t n = *app.texdlg_texture;
    const int tl = mng_FindTrackLock(t->name, page_type::texture).value_or(-1);
    if (tl == -1) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "This texture is not yours to delete.  Lock first.");
      return;
    }
    if (QMessageBox::question(this, "Delete texture",
                              QString("Are you sure you want to delete this texture? %1").arg(QString::fromStdString(t->name))) !=
        QMessageBox::Yes)
      return;
    if (!mng_MakeLocker())
      return;
    mngs_Pagelock pl;
    pl.name = t->name;
    pl.pagetype = page_type::texture;
    if (mng_CheckIfPageOwned(&pl, TableUser.toStdString()) != 1) {
      mng_FreeTrackLock(tl);
      Q_ASSERT(mng_DeletePage(t->name, page_type::texture, 1));
    } else {
      mng_FreeTrackLock(tl);
      mng_DeletePage(t->name, page_type::texture, 1);
      mng_DeletePage(t->name, page_type::texture, 0);
      mng_DeletePagelock(t->name, page_type::texture);
    }
    if (const index_t next = GetNextTexture(n))
      app.texdlg_texture = static_cast<int>(*next);
    FreeTexture(n);
    mng_EraseLocker();
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Texture deleted.");
    updateDialog();
  }
}

void WorldTexturesDialog::onLock() {
  if (auto t = data())
  {
    const int n = *app.texdlg_texture;
    if (!mng_MakeLocker())
      return;
    mngs_Pagelock temp_pl;
    mngs_texture_page texturepage;
    temp_pl.name = t->name;
    temp_pl.pagetype = page_type::texture;
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
          if (!mng_ReplacePage(t->name, t->name, n, page_type::texture, 1)) {
            QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was problem writing that page locally!");
            mng_EraseLocker();
            return;
          }
          QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Texture locked.");
        } else {
          QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was a problem loading this texture.");
        }
        mng_AllocTrackLock(t->name, page_type::texture);
      } else {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't find that texture in the table file!");
      }
    }
    mng_EraseLocker();
    updateDialog();
  }
}

void WorldTexturesDialog::onCheckin() {
  if (auto t = data())
  {
    const int n = *app.texdlg_texture;
    if (!mng_MakeLocker())
      return;
    mngs_Pagelock temp_pl;
    temp_pl.name = t->name;
    temp_pl.pagetype = page_type::texture;
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
      if (!mng_ReplacePage(t->name, t->name, n, page_type::texture, 0))
        QMessageBox::critical(this, "Error!", ErrorString);
      else {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Texture checked in.");
        Q_ASSERT(mng_DeletePage(t->name, page_type::texture, 1) == 1);
        mng_EraseLocker();
        const int p = mng_FindTrackLock(t->name, page_type::texture).value_or(-1);
        Q_ASSERT(p != -1);
        mng_FreeTrackLock(p);
      }
    }
    mng_EraseLocker();
    updateDialog();
  }
}

void WorldTexturesDialog::onCheckedOut() {
  QString str = QString("User %1 has these textures held locally:\n\n").arg(TableUser);
  int total = 0;
  for (int i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used && GlobalTrackLocks[i].pagetype == page_type::texture) {
      str += QString::fromStdString(GlobalTrackLocks[i].name);
      str += "\n";
      total++;
    }
  }
  if (total != 0)
    QMessageBox::information(this, "Textures", str);
}

void WorldTexturesDialog::onOverride() {
  if (auto t = data()) {
    mngs_Pagelock temp_pl;
    temp_pl.name = t->name;
    temp_pl.pagetype = page_type::texture;
    mng_OverrideToUnlocked(&temp_pl);
  }
}

void WorldTexturesDialog::onChangeName() {
  if (auto t = data()) {
    const index_t p = mng_FindTrackLock(t->name, page_type::texture);
    if (!p) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You must lock this texture if you wish to change its name.");
      return;
    }
    bool ok = false;
    const QString name = QInputDialog::getText(this, "Texture name", "Enter a new name for this texture:",
                                               QLineEdit::Normal, QString::fromStdString(t->name), &ok);
    if (!ok || name.isEmpty())
      return;
    if (FindTextureName(name.toStdString())) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "That name is taken, please choose another.");
      return;
    }
    t->name = name.toStdString();
    GlobalTrackLocks[*p].name = t->name;
    updateDialog();
  }
}

void WorldTexturesDialog::onLoadBitmap() {
  if (auto t = data()) {
    QString Current_bitmap_dir; // get from settings
    const QString pathname =
        QFileDialog::getOpenFileName(this, "Load bitmap", Current_bitmap_dir, "Images (*.pcx *.tga *.bmp)");
    if (pathname.isEmpty())
      return;
    const std::filesystem::path pathFs(pathname.toStdString());
    const int bm = LoadTextureImage(pathFs, std::nullopt, texture_size_type::none, 0);
    if (bm < 0)
      return;
    t->bm_handle = bm;
    updateDialog();
  }
}

void WorldTexturesDialog::onNext() {
  if (app.texdlg_texture) {
    if (const index_t next = GetNextTexture(*app.texdlg_texture))
      app.texdlg_texture = *next;
  }
  updateDialog();
}
void WorldTexturesDialog::onPrev() {
  if (app.texdlg_texture) {
    if (const index_t prev = GetPreviousTexture(*app.texdlg_texture))
      app.texdlg_texture = *prev;
  }
  updateDialog();
}

void WorldTexturesDialog::onTexListChanged()
{
  if (const index_t i = FindTextureName(ui->IDC_TEX_LIST->currentText().toStdString()); i)
  {
    app.texdlg_texture = *i;
    updateDialog();
  }
}
