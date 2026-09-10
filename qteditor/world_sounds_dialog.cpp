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

#include "world_sounds_dialog.h"
#include "ui_worldsounds.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QFile>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

#include <QFileInfo>

#include "cfile.h"

#include "d3edit.h"

#include "sndlib/hlsoundlib.h"
#include "manage.h"
#include "soundpage.h"
#include "soundload.h"
#include "ssl_lib.h"


namespace {
// Writes a single float/int field back into the current sound.
} // namespace

WorldSoundsDialog::WorldSoundsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::WorldSoundsDialog) {
  ui->setupUi(this);
  connect(ui->IDC_ADD_SOUND, &QPushButton::clicked, this, &WorldSoundsDialog::onAddSound);
  connect(ui->IDC_LOAD_SOUND, &QPushButton::clicked, this, &WorldSoundsDialog::onLoadSound);
  connect(ui->IDC_NEXT_SOUND, &QPushButton::clicked, this, &WorldSoundsDialog::onNextSound);
  connect(ui->IDC_PREV_SOUND, &QPushButton::clicked, this, &WorldSoundsDialog::onPrevSound);
  connect(ui->IDC_DELETE_SOUND, &QPushButton::clicked, this, &WorldSoundsDialog::onDeleteSound);
  connect(ui->IDC_LOCK_SOUND, &QPushButton::clicked, this, &WorldSoundsDialog::onLockSound);
  connect(ui->IDC_CHECKIN_SOUND, &QPushButton::clicked, this, &WorldSoundsDialog::onCheckinSound);
  connect(ui->IDC_PLAYSOUND, &QPushButton::clicked, this, &WorldSoundsDialog::onPlaysound);
  connect(ui->IDC_KILLSOUNDS, &QPushButton::clicked, this, &WorldSoundsDialog::onKillsounds);
  connect(ui->IDC_OVERRIDE, &QPushButton::clicked, this, &WorldSoundsDialog::onOverride);
  connect(ui->IDC_SOUND_CHANGE_NAME, &QPushButton::clicked, this, &WorldSoundsDialog::onChangeName);

      connect(ui->IDC_SOUND_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), this,
    &WorldSoundsDialog::onSoundPulldownChanged);

  connect(ui->IDC_SOUNDMAXDIST_EDIT, &QLineEdit::editingFinished, this,&WorldSoundsDialog::onMaxDistEdited);
  connect(ui->IDC_SOUNDMINDIST_EDIT, &QLineEdit::editingFinished, this,&WorldSoundsDialog::onMinDistEdited);
  connect(ui->IDC_SOUNDINNERCONEANGLE_EDIT,&QLineEdit::editingFinished,this,&WorldSoundsDialog::onInnerConeEdited);
  connect(ui->IDC_SOUNDOUTERCONEANGLE_EDIT,&QLineEdit::editingFinished,this,&WorldSoundsDialog::onOuterConeAngleEdited);
  connect(ui->IDC_SOUNDOUTERCONEVOL_EDIT,&QLineEdit::editingFinished,this,&WorldSoundsDialog::onOuterConeVolEdited);
  connect(ui->IDC_SOUNDLOOPSTART_EDIT,&QLineEdit::editingFinished,this,&WorldSoundsDialog::onLoopStartEdited);
  connect(ui->IDC_SOUNDLOOPEND_EDIT,&QLineEdit::editingFinished,this,&WorldSoundsDialog::onLoopEndEdited);
  connect(ui->IDC_SOUND_IMPORT_VOLUME_EDIT,&QLineEdit::editingFinished,this,&WorldSoundsDialog::onImportVolumeEdited);

  connect(ui->IDC_SOUNDHALLEFFECT_CHECK, &QCheckBox::toggled, this, &WorldSoundsDialog::onHallEffectToggled);
  connect(ui->IDC_LOOPING_CHECK, &QCheckBox::toggled, this, &WorldSoundsDialog::onLoopingToggled);
  connect(ui->IDC_SOUNDFOREVER_CHECK, &QCheckBox::toggled, this, &WorldSoundsDialog::onForeverToggled);
  connect(ui->IDC_SOUNDEXCLUSIVE_CHECK, &QCheckBox::toggled, this, &WorldSoundsDialog::onExclusiveToggled);
  connect(ui->IDC_SOUNDONCE_CHECK, &QCheckBox::toggled, this, &WorldSoundsDialog::onOnceToggled);
  connect(ui->IDC_SOUND_ONCE_PER_OBJ_CHECK, &QCheckBox::toggled, this, &WorldSoundsDialog::onOncePerObjToggled);
  connect(ui->IDC_SOUND_NO_UPDATE, &QCheckBox::toggled, this, &WorldSoundsDialog::onNoUpdateToggled);

  connect(ui->IDC_SOUNDOBJATTACH_RADIO, &QRadioButton::clicked, this, &WorldSoundsDialog::onObjAttach);
  connect(ui->IDC_SOUNDPOSATTACH_RADIO, &QRadioButton::clicked, this, &WorldSoundsDialog::onPosAttach);
  connect(ui->IDC_SOUNDOBJECT_RADIO, &QRadioButton::clicked, this, &WorldSoundsDialog::onConeLinkObject);
  connect(ui->IDC_SOUNDTURRET1_RADIO, &QRadioButton::clicked, this, &WorldSoundsDialog::onConeLinkTurret1);
  connect(ui->IDC_SOUNDTURRET2_RADIO, &QRadioButton::clicked, this, &WorldSoundsDialog::onConeLinkTurret2);
  connect(ui->IDC_SOUNDTURRET3_RADIO, &QRadioButton::clicked, this, &WorldSoundsDialog::onConeLinkTurret3);
  connect(ui->IDC_SOUNDFORWARD_RADIO, &QRadioButton::clicked, this, &WorldSoundsDialog::onConeDirForward);
  connect(ui->IDC_SOUNDBACKWARD_RADIO, &QRadioButton::clicked, this, &WorldSoundsDialog::onConeDirBackward);
  connect(ui->IDC_SOUNDUPWARD_RADIO, &QRadioButton::clicked, this, &WorldSoundsDialog::onConeDirUpward);
  connect(ui->IDC_SOUNDDOWNWARD_RADIO, &QRadioButton::clicked, this, &WorldSoundsDialog::onConeDirDownward);

  updateDialog();
}

WorldSoundsDialog::~WorldSoundsDialog() { saveSoundsOnClose(); }

void WorldSoundsDialog::saveSoundsOnClose() {
  if (!Network_up)
    return;
  for (int i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used == 1 && GlobalTrackLocks[i].pagetype == PAGETYPE_SOUND) {
      const int t = FindSoundName(GlobalTrackLocks[i].name);
      if (t != -1)
        mng_ReplacePage(Sounds[t].name, Sounds[t].name, t, PAGETYPE_SOUND, 1);
    }
  }
}

void WorldSoundsDialog::setFlag(uint32_t flag, const char *checkName, bool checked) {
  const int n = app.current_sound;
  if (n < 0 || n >= MAX_SOUNDS || !Sounds[n].used)
    return;
  if (checked)
    Sounds[n].flags |= flag;
  else
    Sounds[n].flags &= ~flag;
}

void WorldSoundsDialog::setConeLink(int value) {
  setFlag(SPFT_CONE_LINK_MASK, "", true);
  const int n = app.current_sound;
  if (n >= 0 && n < MAX_SOUNDS)
    Sounds[n].flags = (Sounds[n].flags & ~SPFT_CONE_LINK_MASK) | value;
}

void WorldSoundsDialog::setConeDir(int value) {
  const int n = app.current_sound;
  if (n >= 0 && n < MAX_SOUNDS)
    Sounds[n].flags = (Sounds[n].flags & ~SPFT_CONE_DIR_MASK) | value;
}

void WorldSoundsDialog::updateDialog() {
  const int n = app.current_sound;

  ui->IDC_NEXT_SOUND->setEnabled(Num_sounds >= 1);
  ui->IDC_PREV_SOUND->setEnabled(Num_sounds >= 1);
  if (!Network_up) {
    ui->IDC_LOCK_SOUND->setEnabled(false);
    ui->IDC_CHECKIN_SOUND->setEnabled(false);
    ui->IDC_OVERRIDE->setEnabled(false);
    return;
  }
  if (Num_sounds < 1)
    return;

  if (!Sounds[n].used)
    app.current_sound = GetNextSound(n);

  Sound_system.CheckAndForceSoundDataAlloc(app.current_sound);
  const int s = app.current_sound;

  int total_memory = 0;
  for (int i = 0; i < MAX_SOUNDS; i++)
    if (Sounds[i].used)
      total_memory += SoundFiles[Sounds[i].sample_index].sample_length * 2;

  ui->IDC_SOUNDMAXDIST_EDIT->setText(QString::number(Sounds[s].max_distance));
  ui->IDC_SOUNDMINDIST_EDIT->setText(QString::number(Sounds[s].min_distance));
  ui->IDC_SOUNDINNERCONEANGLE_EDIT->setText(QString::number(Sounds[s].inner_cone_angle));
  ui->IDC_SOUNDOUTERCONEANGLE_EDIT->setText(QString::number(Sounds[s].outer_cone_angle));
  ui->IDC_SOUNDOUTERCONEVOL_EDIT->setText(QString::number(Sounds[s].outer_cone_volume * 100.0f));
  ui->IDC_SOUNDLOOPSTART_EDIT->setText(QString::number(Sounds[s].loop_start));

  if (Sounds[s].loop_end >= SoundFiles[Sounds[s].sample_index].np_sample_length)
    Sounds[s].loop_end = SoundFiles[Sounds[s].sample_index].np_sample_length - 1;
  if (Sounds[s].loop_start > Sounds[s].loop_end)
    Sounds[s].loop_start = 0;
  ui->IDC_SOUNDLOOPEND_EDIT->setText(QString::number(Sounds[s].loop_end));

  ui->IDC_SOUND_IMPORT_VOLUME_EDIT->setText(QString::number(Sounds[s].import_volume * 100.0f));
  ui->IDC_RAW_NAME_EDIT->setText(QString::fromStdString(SoundFiles[Sounds[s].sample_index].name));

  ui->IDC_SOUND_MEMORY_STATIC->setText(QString("%1, %2 H, %3 Total")
    .arg(SoundFiles[Sounds[s].sample_index].sample_length / 512)
    .arg(0)
    .arg(total_memory / 1024));

  ui->IDC_SOUNDHALLEFFECT_CHECK->setChecked(!(Sounds[s].flags & SPF_FIXED_FREQ));
  ui->IDC_SOUNDFOREVER_CHECK->setChecked(Sounds[s].flags & SPF_FOREVER);
  ui->IDC_SOUND_ONCE_PER_OBJ_CHECK->setChecked(Sounds[s].flags & SPF_ONCE_PER_OBJ);
  ui->IDC_SOUNDEXCLUSIVE_CHECK->setChecked(Sounds[s].flags & SPF_PLAYS_EXCLUSIVELY);
  ui->IDC_SOUND_NO_UPDATE->setChecked(Sounds[s].flags & SPF_LISTENER_UPDATE);
  ui->IDC_SOUNDONCE_CHECK->setChecked(Sounds[s].flags & SPF_PLAYS_ONCE);
  ui->IDC_LOOPING_CHECK->setChecked(Sounds[s].flags & SPF_LOOPED);

  ui->IDC_SOUNDOBJATTACH_RADIO->setChecked(Sounds[s].flags & SPF_OBJ_UPDATE);
  ui->IDC_SOUNDPOSATTACH_RADIO->setChecked(!(Sounds[s].flags & SPF_OBJ_UPDATE));

  switch (SPFT_CONE_LINK_MASK & Sounds[s].flags) {
  case SPFT_CONE_LINK_TURRET1:
    ui->IDC_SOUNDTURRET1_RADIO->setChecked(true);
    break;
  case SPFT_CONE_LINK_TURRET2:
    ui->IDC_SOUNDTURRET2_RADIO->setChecked(true);
    break;
  case SPFT_CONE_LINK_TURRET3:
    ui->IDC_SOUNDTURRET3_RADIO->setChecked(true);
    break;
  default:
    ui->IDC_SOUNDOBJECT_RADIO->setChecked(true);
    break;
  }

  switch (SPFT_CONE_DIR_MASK & Sounds[s].flags) {
  case SPFT_CONE_DIR_BACKWARD:
    ui->IDC_SOUNDBACKWARD_RADIO->setChecked(true);
    break;
  case SPFT_CONE_DIR_UPWARD:
    ui->IDC_SOUNDUPWARD_RADIO->setChecked(true);
    break;
  case SPFT_CONE_DIR_DOWNWARD:
    ui->IDC_SOUNDDOWNWARD_RADIO->setChecked(true);
    break;
  default:
    ui->IDC_SOUNDFORWARD_RADIO->setChecked(true);
    break;
  }

  {
    QPushButton *checkin = ui->IDC_CHECKIN_SOUND;
    if (mng_FindTrackLock(Sounds[s].name, PAGETYPE_SOUND) == -1) {
      checkin->setEnabled(false);
      ui->IDC_LOCK_SOUND->setEnabled(true);
      ui->IDC_SOUND_CHANGE_NAME->setEnabled(false);
    } else {
      checkin->setEnabled(true);
      ui->IDC_LOCK_SOUND->setEnabled(false);
      ui->IDC_SOUND_CHANGE_NAME->setEnabled(true);
    }
  }

  {
    QComboBox *combo = ui->IDC_SOUND_PULLDOWN;
    QSignalBlocker blocker(combo);
    combo->clear();
    for (int i = 0; i < MAX_SOUNDS; i++)
      if (Sounds[i].used)
        combo->addItem(QString::fromStdString(Sounds[i].name));
    combo->setCurrentText(QString::fromStdString(Sounds[s].name));
  }
}

void WorldSoundsDialog::onAddSound() {
  if (!Network_up) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Sorry babe, the network is down.  This action is a no-no.\n");
    return;
  }

  QString Current_sounds_dir; // get from settings

  const QString pathname =
      QFileDialog::getOpenFileName(this, "Select sound", Current_sounds_dir, "Descent III files (*.wav)");
  if (pathname.isEmpty())
    return;

  QFileInfo fileInfo(pathname);
  const QByteArray pathBytes = pathname.toLocal8Bit();
  const char *fname = fileInfo.baseName().toLocal8Bit().constData();

  const int raw_handle = LoadSoundFile(pathBytes.constData(), 1.0f, true);
  if (raw_handle < 0) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Invalid sound file.");
    return;
  }

  const int sound_handle = AllocSound();
  int c = 1;
  bool finding_name = true;
  char cur_name[100];
  while (finding_name) {
    if (c == 1)
      snprintf(cur_name, sizeof(cur_name), "%s", fname);
    else
      snprintf(cur_name, sizeof(cur_name), "%s%d", fname, c);
    if (FindSoundName(std::string(cur_name)) != -1)
      c++;
    else
      finding_name = false;
  }

  Sounds[sound_handle].name = cur_name;
  Sounds[sound_handle].sample_index = raw_handle;

  std::filesystem::path destname = LocalSoundsDir / SoundFiles[Sounds[sound_handle].sample_index].name;
  std::filesystem::copy(std::filesystem::path(pathname.toStdString()), (destname), std::filesystem::copy_options::overwrite_existing);

  mng_AllocTrackLock(cur_name, PAGETYPE_SOUND);
  app.current_sound = sound_handle;
  RemapSounds();
  Sound_system.CheckAndForceSoundDataAlloc(app.current_sound);
  Sounds[sound_handle].loop_end = SoundFiles[Sounds[sound_handle].sample_index].np_sample_length - 1;
  updateDialog();
}

void WorldSoundsDialog::onLoadSound() {
  if (!Network_up)
    return;
  QString Current_sounds_dir; // get from settings
  const QString pathname =
      QFileDialog::getOpenFileName(this, "Load sound", Current_sounds_dir, "Descent III files (*.wav)");
  if (pathname.isEmpty())
    return;
  const int n = app.current_sound;
  if (n < 0 || n >= MAX_SOUNDS || !Sounds[n].used)
    return;
  const QByteArray pathBytes = pathname.toLocal8Bit();
  const int raw_handle = LoadSoundFile(pathBytes.constData(), Sounds[n].import_volume, false);
  if (raw_handle < 0)
    return;
  Sounds[n].sample_index = raw_handle;
  Sounds[n].loop_end = SoundFiles[raw_handle].np_sample_length - 1;
  updateDialog();
}

void WorldSoundsDialog::onNextSound() {
  app.current_sound = GetNextSound(app.current_sound);
  updateDialog();
}
void WorldSoundsDialog::onPrevSound() {
  app.current_sound = GetPrevSound(app.current_sound);
  updateDialog();
}

void WorldSoundsDialog::onDeleteSound() {
  const int n = app.current_sound;
  if (Num_sounds < 1)
    return;
  const int tl = mng_FindTrackLock(Sounds[n].name, PAGETYPE_SOUND);
  if (tl == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "This sound is not yours to delete.  Lock first.");
    return;
  }
  if (QMessageBox::question(this, "Delete sound",
                            QString("Are you sure you want to delete this sound? %1").arg(QString::fromStdString(Sounds[n].name))) !=
      QMessageBox::Yes)
    return;
  if (!mng_MakeLocker())
    return;

  mngs_Pagelock pl;
  pl.name = Sounds[n].name;
  pl.pagetype = PAGETYPE_SOUND;

  if (mng_CheckIfPageOwned(&pl, TableUser.toStdString()) != 1) {
    mng_FreeTrackLock(tl);
    Q_ASSERT(mng_DeletePage(Sounds[n].name, PAGETYPE_SOUND, 1));
  } else {
    mng_FreeTrackLock(tl);
    mng_DeletePage(Sounds[n].name, PAGETYPE_SOUND, 1);
    mng_DeletePage(Sounds[n].name, PAGETYPE_SOUND, 0);
    mng_DeletePagelock(Sounds[n].name, PAGETYPE_SOUND);
  }

  app.current_sound = GetNextSound(n);
  FreeSound(n);
  mng_EraseLocker();
  QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Sound deleted.");
  RemapSounds();
  updateDialog();
}

void WorldSoundsDialog::onLockSound() {
  const int n = app.current_sound;
  if (Num_sounds < 1)
    return;
  if (!mng_MakeLocker())
    return;

  mngs_Pagelock temp_pl;
  mngs_sound_page soundpage;
  temp_pl.name = Sounds[n].name;
  temp_pl.pagetype = PAGETYPE_SOUND;

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
    if (mng_FindSpecificSoundPage(temp_pl.name, &soundpage)) {
      if (mng_AssignSoundPageToSound(&soundpage, n)) {
        if (!mng_ReplacePage(Sounds[n].name, Sounds[n].name, n, PAGETYPE_SOUND, 1)) {
          QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was problem writing that page locally!");
          mng_EraseLocker();
          return;
        }
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Sound locked.");
      } else {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was a problem loading this sound.");
      }
      mng_AllocTrackLock(Sounds[n].name, PAGETYPE_SOUND);
    } else {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't find that sound in the table file!");
    }
  }
  mng_EraseLocker();
  updateDialog();
}

void WorldSoundsDialog::onCheckinSound() {
  const int n = app.current_sound;
  if (Num_sounds < 1)
    return;
  if (!mng_MakeLocker())
    return;

  mngs_Pagelock temp_pl;
  temp_pl.name = Sounds[n].name;
  temp_pl.pagetype = PAGETYPE_SOUND;

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
    if (!mng_ReplacePage(Sounds[n].name, Sounds[n].name, n, PAGETYPE_SOUND, 0))
      QMessageBox::critical(this, "Error!", ErrorString);
    else {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Sound checked in.");
      Q_ASSERT(mng_DeletePage(Sounds[n].name, PAGETYPE_SOUND, 1) == 1);
      mng_EraseLocker();
      const int p = mng_FindTrackLock(Sounds[n].name, PAGETYPE_SOUND);
      Q_ASSERT(p != -1);
      mng_FreeTrackLock(p);
    }
  }
  mng_EraseLocker();
  updateDialog();
}

void WorldSoundsDialog::onPlaysound() {
  const int n = app.current_sound;
  if (Num_sounds < 1)
    return;
  Sound_system.BeginSoundFrame();
  Sound_system.Play2dSound(n);
  Sound_system.EndSoundFrame();
}

void WorldSoundsDialog::onKillsounds() {
  QMessageBox::information(this, "Kill sounds", "All sounds have been killed.");
  // The MFC original simply calls Sound_system.StopAllSounds(); the Qt port
  // stops any currently playing sound object.
  Sound_system.StopAllSounds();
}

void WorldSoundsDialog::onOverride() {
  const int n = app.current_sound;
  mngs_Pagelock temp_pl;
  temp_pl.name = Sounds[n].name;
  temp_pl.pagetype = PAGETYPE_SOUND;
  mng_OverrideToUnlocked(&temp_pl);
}

void WorldSoundsDialog::onChangeName() {
  const int n = app.current_sound;
  const int p = mng_FindTrackLock(Sounds[n].name, PAGETYPE_SOUND);
  if (p == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You must lock this sound if you wish to change its name.");
    return;
  }
  bool ok = false;
  QString name = QInputDialog::getText(this, "Sound name", "Enter a new name for this sound:",
                                       QLineEdit::Normal, QString::fromStdString(Sounds[n].name), &ok);
  if (!ok)
    return;
  if (FindSoundName(name.toStdString()) != -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "That name is taken, please choose another.");
    return;
  }
  if (!mng_MakeLocker())
    return;
  mngs_Pagelock pl;
  pl.name = Sounds[n].name;
  pl.pagetype = PAGETYPE_SOUND;
  const QByteArray newName = name.toLocal8Bit();
  std::string newNameBuf;
  newNameBuf = std::string(newName.constData());
  const int ret = mng_CheckIfPageOwned(&pl, TableUser.toStdString());
  if (ret < 0)
    QMessageBox::critical(this, "Error!", ErrorString);
  else if (ret == 1)
    mng_RenamePage(Sounds[n].name, newNameBuf, PAGETYPE_SOUND);
  else if (ret == 2) {
    GlobalTrackLocks[p].name = newName.constData();
    mng_ReplacePage(GlobalTrackLocks[p].name, newNameBuf, n, PAGETYPE_SOUND, 1);
  } else if (ret == 0) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You don't own this page.  Get Jason now!");
    mng_FreeTrackLock(p);
    mng_EraseLocker();
    return;
  }
  GlobalTrackLocks[p].name = newName.constData();
  Sounds[n].name = newName.constData();
  mng_EraseLocker();
  RemapSounds();
  updateDialog();
}

void WorldSoundsDialog::onSoundPulldownChanged() {
  QComboBox *combo = ui->IDC_SOUND_PULLDOWN;
  const int i = FindSoundName(combo->currentText().toStdString());
  if (i == -1)
    return;
  app.current_sound = i;
  updateDialog();
}

void WorldSoundsDialog::onMaxDistEdited() {
  const int n = app.current_sound;
  Sounds[n].max_distance = ui->IDC_SOUNDMAXDIST_EDIT->text().toFloat();
}
void WorldSoundsDialog::onMinDistEdited() {
  const int n = app.current_sound;
  Sounds[n].min_distance = ui->IDC_SOUNDMINDIST_EDIT->text().toFloat();
}
void WorldSoundsDialog::onInnerConeEdited() {
  const int n = app.current_sound;
  Sounds[n].inner_cone_angle = ui->IDC_SOUNDINNERCONEANGLE_EDIT->text().toInt();
}
void WorldSoundsDialog::onOuterConeAngleEdited() {
  const int n = app.current_sound;
  Sounds[n].outer_cone_angle = ui->IDC_SOUNDOUTERCONEANGLE_EDIT->text().toInt();
}
void WorldSoundsDialog::onOuterConeVolEdited() {
  const int n = app.current_sound;
  Sounds[n].outer_cone_volume = ui->IDC_SOUNDOUTERCONEVOL_EDIT->text().toFloat() / 100.0f;
}
void WorldSoundsDialog::onLoopStartEdited() {
  const int n = app.current_sound;
  Sounds[n].loop_start = ui->IDC_SOUNDLOOPSTART_EDIT->text().toInt();
}
void WorldSoundsDialog::onLoopEndEdited() {
  const int n = app.current_sound;
  Sounds[n].loop_end = ui->IDC_SOUNDLOOPEND_EDIT->text().toInt();
}
void WorldSoundsDialog::onImportVolumeEdited() {
  const int n = app.current_sound;
  Sounds[n].import_volume = ui->IDC_SOUND_IMPORT_VOLUME_EDIT->text().toFloat() / 100.0f;
}

void WorldSoundsDialog::onHallEffectToggled(bool checked) { setFlag(SPF_FIXED_FREQ, "", !checked); }
void WorldSoundsDialog::onLoopingToggled(bool checked) { setFlag(SPF_LOOPED, "", checked); }
void WorldSoundsDialog::onForeverToggled(bool checked) { setFlag(SPF_FOREVER, "", checked); }
void WorldSoundsDialog::onExclusiveToggled(bool checked) { setFlag(SPF_PLAYS_EXCLUSIVELY, "", checked); }
void WorldSoundsDialog::onOnceToggled(bool checked) { setFlag(SPF_PLAYS_ONCE, "", checked); }
void WorldSoundsDialog::onOncePerObjToggled(bool checked) { setFlag(SPF_ONCE_PER_OBJ, "", checked); }
void WorldSoundsDialog::onNoUpdateToggled(bool checked) { setFlag(SPF_LISTENER_UPDATE, "", checked); }
void WorldSoundsDialog::onObjAttach() { setFlag(SPF_OBJ_UPDATE, "", true); }
void WorldSoundsDialog::onPosAttach() { setFlag(SPF_OBJ_UPDATE, "", false); }
void WorldSoundsDialog::onConeLinkObject() { setConeLink(SPFT_CONE_LINK_OBJECT); }
void WorldSoundsDialog::onConeLinkTurret1() { setConeLink(SPFT_CONE_LINK_TURRET1); }
void WorldSoundsDialog::onConeLinkTurret2() { setConeLink(SPFT_CONE_LINK_TURRET2); }
void WorldSoundsDialog::onConeLinkTurret3() { setConeLink(SPFT_CONE_LINK_TURRET3); }
void WorldSoundsDialog::onConeDirForward() { setConeDir(SPFT_CONE_DIR_FORWARD); }
void WorldSoundsDialog::onConeDirBackward() { setConeDir(SPFT_CONE_DIR_BACKWARD); }
void WorldSoundsDialog::onConeDirUpward() { setConeDir(SPFT_CONE_DIR_UPWARD); }
void WorldSoundsDialog::onConeDirDownward() { setConeDir(SPFT_CONE_DIR_DOWNWARD); }

