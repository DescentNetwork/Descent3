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

#include "d3edit.h"

#include "manage.h"
#include "hlsoundlib.h"
#include "soundpage.h"
#include "soundload.h"


// Writes a single float/int field back into the current sound.
sound_info* soundPtr(int n) {
  return (n >= 0 && n < static_cast<int>(Sounds.size())) ? &Sounds[n] : nullptr;
}


WorldSoundsDialog::WorldSoundsDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::WorldSoundsDialog)
{
  ui->setupUi(this);

  m_snd = soundPtr(app.current_sound);

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

  connect(ui->IDC_SOUND_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), this, &WorldSoundsDialog::onSoundPulldownChanged);

  connect(ui->IDC_SOUNDMAXDIST_EDIT, &QLineEdit::editingFinished, this,&WorldSoundsDialog::onMaxDistEdited);
  connect(ui->IDC_SOUNDMINDIST_EDIT, &QLineEdit::editingFinished, this,&WorldSoundsDialog::onMinDistEdited);
  connect(ui->IDC_SOUNDINNERCONEANGLE_EDIT,&QLineEdit::editingFinished,this,&WorldSoundsDialog::onInnerConeEdited);
  connect(ui->IDC_SOUNDOUTERCONEANGLE_EDIT,&QLineEdit::editingFinished,this,&WorldSoundsDialog::onOuterConeAngleEdited);
  connect(ui->IDC_SOUNDOUTERCONEVOL_EDIT,&QLineEdit::editingFinished,this,&WorldSoundsDialog::onOuterConeVolEdited);
  connect(ui->IDC_SOUNDLOOPSTART_EDIT,&QLineEdit::editingFinished,this,&WorldSoundsDialog::onLoopStartEdited);
  connect(ui->IDC_SOUNDLOOPEND_EDIT,&QLineEdit::editingFinished,this,&WorldSoundsDialog::onLoopEndEdited);
  connect(ui->IDC_SOUND_IMPORT_VOLUME_EDIT,&QLineEdit::editingFinished,this,&WorldSoundsDialog::onImportVolumeEdited);

  connect(ui->IDC_SOUNDHALLEFFECT_CHECK,    &QCheckBox::toggled, [this](bool checked){ if(m_snd) { m_snd->flags.fixed_freq        = !checked; } });
  connect(ui->IDC_LOOPING_CHECK,            &QCheckBox::toggled, [this](bool checked){ if(m_snd) { m_snd->flags.looped            = checked; } });
  connect(ui->IDC_SOUNDFOREVER_CHECK,       &QCheckBox::toggled, [this](bool checked){ if(m_snd) { m_snd->flags.plays_forever     = checked; } });
  connect(ui->IDC_SOUNDEXCLUSIVE_CHECK,     &QCheckBox::toggled, [this](bool checked){ if(m_snd) { m_snd->flags.plays_exclusively = checked; } });
  connect(ui->IDC_SOUNDONCE_CHECK,          &QCheckBox::toggled, [this](bool checked){ if(m_snd) { m_snd->flags.plays_once        = checked; } });
  connect(ui->IDC_SOUND_ONCE_PER_OBJ_CHECK, &QCheckBox::toggled, [this](bool checked){ if(m_snd) { m_snd->flags.once_per_obj      = checked; } });
  connect(ui->IDC_SOUND_NO_UPDATE,          &QCheckBox::toggled, [this](bool checked){ if(m_snd) { m_snd->flags.listener_update   = checked; } });

  connect(ui->IDC_SOUNDOBJATTACH_RADIO, &QRadioButton::clicked, [this](){ if(m_snd) { m_snd->flags.obj_update = true; } });
  connect(ui->IDC_SOUNDPOSATTACH_RADIO, &QRadioButton::clicked, [this](){ if(m_snd) { m_snd->flags.obj_update = false; } });
  connect(ui->IDC_SOUNDOBJECT_RADIO,    &QRadioButton::clicked, [this](){ if(m_snd) { m_snd->flags.cone_link = 0; m_snd->flags.once_per_obj = 0; } });
  connect(ui->IDC_SOUNDTURRET1_RADIO,   &QRadioButton::clicked, [this](){ if(m_snd) { m_snd->flags.cone_link = 0; m_snd->flags.once_per_obj = 1; } });
  connect(ui->IDC_SOUNDTURRET2_RADIO,   &QRadioButton::clicked, [this](){ if(m_snd) { m_snd->flags.cone_link = 1; m_snd->flags.once_per_obj = 0; } });
  connect(ui->IDC_SOUNDTURRET3_RADIO,   &QRadioButton::clicked, [this](){ if(m_snd) { m_snd->flags.cone_link = 1; m_snd->flags.once_per_obj = 1; } });
  connect(ui->IDC_SOUNDFORWARD_RADIO,   &QRadioButton::clicked, [this](){ if(m_snd) { m_snd->flags.cone_dir = 0; } });
  connect(ui->IDC_SOUNDBACKWARD_RADIO,  &QRadioButton::clicked, [this](){ if(m_snd) { m_snd->flags.cone_dir = 1; } });
  connect(ui->IDC_SOUNDUPWARD_RADIO,    &QRadioButton::clicked, [this](){ if(m_snd) { m_snd->flags.cone_dir = 2; } });
  connect(ui->IDC_SOUNDDOWNWARD_RADIO,  &QRadioButton::clicked, [this](){ if(m_snd) { m_snd->flags.cone_dir = 3; } });

  updateDialog();
}


WorldSoundsDialog::~WorldSoundsDialog() { saveSoundsOnClose(); }

void WorldSoundsDialog::saveSoundsOnClose()
{
  if (!Network_up)
    return;
  for (int i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used == 1 && GlobalTrackLocks[i].pagetype == PAGETYPE_SOUND) {
      const int t = FindSoundName(GlobalTrackLocks[i].name).value_or(-1);
      if (t != -1)
        mng_ReplacePage(Sounds[t].name, Sounds[t].name, t, PAGETYPE_SOUND, 1);
    }
  }
}

void WorldSoundsDialog::updateDialog()
{
  ui->IDC_NEXT_SOUND->setEnabled(static_cast<int>(Sounds.size()));
  ui->IDC_PREV_SOUND->setEnabled(static_cast<int>(Sounds.size()));
  if (!Network_up) {
    ui->IDC_LOCK_SOUND->setEnabled(false);
    ui->IDC_CHECKIN_SOUND->setEnabled(false);
    ui->IDC_OVERRIDE->setEnabled(false);
    return;
  }
  if (!static_cast<int>(Sounds.size()))
    return;

  if(m_snd == nullptr || Sounds.is_unused(app.current_sound))
    app.current_sound = GetNextSound(app.current_sound);

  Sound_system.CheckAndForceSoundDataAlloc(app.current_sound);
  m_snd = soundPtr(app.current_sound);
  if(m_snd == nullptr)
    return;


  int total_memory = 0;
  for (int i = 0; i < static_cast<int>(Sounds.size()); i++)
    if (Sounds.is_used(i))
      total_memory += SoundFiles[Sounds[i].sample_index].sample_length * 2;

  ui->IDC_SOUNDMAXDIST_EDIT->setText(QString::number(m_snd->max_distance));
  ui->IDC_SOUNDMINDIST_EDIT->setText(QString::number(m_snd->min_distance));
  ui->IDC_SOUNDINNERCONEANGLE_EDIT->setText(QString::number(m_snd->inner_cone_angle));
  ui->IDC_SOUNDOUTERCONEANGLE_EDIT->setText(QString::number(m_snd->outer_cone_angle));
  ui->IDC_SOUNDOUTERCONEVOL_EDIT->setText(QString::number(m_snd->outer_cone_volume * 100.0f));
  ui->IDC_SOUNDLOOPSTART_EDIT->setText(QString::number(m_snd->loop_start));


  if (m_snd->loop_end >= SoundFiles[m_snd->sample_index].np_sample_length)
    m_snd->loop_end = SoundFiles[m_snd->sample_index].np_sample_length - 1;
  if (m_snd->loop_start > m_snd->loop_end)
    m_snd->loop_start = 0;
  ui->IDC_SOUNDLOOPEND_EDIT->setText(QString::number(m_snd->loop_end));

  ui->IDC_SOUND_IMPORT_VOLUME_EDIT->setText(QString::number(m_snd->import_volume * 100.0f));
  ui->IDC_RAW_NAME_EDIT->setText(QString::fromStdString(SoundFiles[m_snd->sample_index].name));

  ui->IDC_SOUND_MEMORY_STATIC->setText(QString("%1, %2 H, %3 Total")
    .arg(SoundFiles[m_snd->sample_index].sample_length / 512)
    .arg(0)
    .arg(total_memory / 1024));


  ui->IDC_SOUNDHALLEFFECT_CHECK->setChecked(!m_snd->flags.fixed_freq);
  ui->IDC_SOUNDFOREVER_CHECK->setChecked(m_snd->flags.plays_forever);
  ui->IDC_SOUND_ONCE_PER_OBJ_CHECK->setChecked(m_snd->flags.once_per_obj);
  ui->IDC_SOUNDEXCLUSIVE_CHECK->setChecked(m_snd->flags.plays_exclusively);
  ui->IDC_SOUND_NO_UPDATE->setChecked(m_snd->flags.listener_update);
  ui->IDC_SOUNDONCE_CHECK->setChecked(m_snd->flags.plays_once);
  ui->IDC_LOOPING_CHECK->setChecked(m_snd->flags.looped);

  ui->IDC_SOUNDOBJATTACH_RADIO->setChecked(m_snd->flags.obj_update);
  ui->IDC_SOUNDPOSATTACH_RADIO->setChecked(!m_snd->flags.obj_update);

  // The cone-link selector shares its low bit with once_per_obj; recompose
  // the 2-bit selector the same way SPFT_CONE_LINK_* did.
  if (m_snd->flags.cone_link && m_snd->flags.once_per_obj)
    ui->IDC_SOUNDTURRET3_RADIO->setChecked(true);
  else if (m_snd->flags.cone_link)
    ui->IDC_SOUNDTURRET2_RADIO->setChecked(true);
  else if (m_snd->flags.once_per_obj)
    ui->IDC_SOUNDTURRET1_RADIO->setChecked(true);
  else
    ui->IDC_SOUNDOBJECT_RADIO->setChecked(true);

  switch (m_snd->flags.cone_dir) {
  case 1: // SPFT_CONE_DIR_BACKWARD
    ui->IDC_SOUNDBACKWARD_RADIO->setChecked(true);
    break;
  case 2: // SPFT_CONE_DIR_UPWARD
    ui->IDC_SOUNDUPWARD_RADIO->setChecked(true);
    break;
  case 3: // SPFT_CONE_DIR_DOWNWARD
    ui->IDC_SOUNDDOWNWARD_RADIO->setChecked(true);
    break;
  default: // SPFT_CONE_DIR_FORWARD
    ui->IDC_SOUNDFORWARD_RADIO->setChecked(true);
    break;
  }

  {
    QPushButton *checkin = ui->IDC_CHECKIN_SOUND;
    if (!mng_FindTrackLock(m_snd->name, PAGETYPE_SOUND)) {
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
    QSignalBlocker blocker(ui->IDC_SOUND_PULLDOWN);
    ui->IDC_SOUND_PULLDOWN->clear();
    for (int i = 0; i < static_cast<int>(Sounds.size()); i++)
      if (Sounds.is_used(i))
        ui->IDC_SOUND_PULLDOWN->addItem(QString::fromStdString(Sounds[i].name));
    ui->IDC_SOUND_PULLDOWN->setCurrentText(QString::fromStdString(m_snd->name));
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
    if (!FindSoundName(std::string(cur_name)))
      c++;
    else
      finding_name = false;
  }

  m_snd = soundPtr(sound_handle);
  m_snd->name = cur_name;
  m_snd->sample_index = raw_handle;

  std::filesystem::path destname = LocalSoundsDir / SoundFiles[m_snd->sample_index].name;
  std::filesystem::copy(std::filesystem::path(pathname.toStdString()), (destname), std::filesystem::copy_options::overwrite_existing);

  mng_AllocTrackLock(cur_name, PAGETYPE_SOUND);
  app.current_sound = sound_handle;
  RemapSounds();
  Sound_system.CheckAndForceSoundDataAlloc(app.current_sound);
  m_snd->loop_end = SoundFiles[m_snd->sample_index].np_sample_length - 1;
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
  if (n < 0 || n >= static_cast<int>(Sounds.size()) || !m_snd || Sounds.is_unused(n))
    return;
  const QByteArray pathBytes = pathname.toLocal8Bit();
  const int raw_handle = LoadSoundFile(pathBytes.constData(), m_snd->import_volume, false);
  if (raw_handle < 0)
    return;
  m_snd->sample_index = raw_handle;
  m_snd->loop_end = SoundFiles[raw_handle].np_sample_length - 1;
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
  if (!static_cast<int>(Sounds.size()))
    return;
  const int tl = mng_FindTrackLock(m_snd->name, PAGETYPE_SOUND).value_or(-1);
  if (tl == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "This sound is not yours to delete.  Lock first.");
    return;
  }
  if (QMessageBox::question(this, "Delete sound",
                            QString("Are you sure you want to delete this sound? %1").arg(QString::fromStdString(m_snd->name))) != QMessageBox::Yes)
    return;
  if (!mng_MakeLocker())
    return;

  mngs_Pagelock pl;
  pl.name = m_snd->name;
  pl.pagetype = PAGETYPE_SOUND;

  if (mng_CheckIfPageOwned(&pl, TableUser.toStdString()) != 1) {
    mng_FreeTrackLock(tl);
    Q_ASSERT(mng_DeletePage(m_snd->name, PAGETYPE_SOUND, 1));
  } else {
    mng_FreeTrackLock(tl);
    mng_DeletePage(m_snd->name, PAGETYPE_SOUND, 1);
    mng_DeletePage(m_snd->name, PAGETYPE_SOUND, 0);
    mng_DeletePagelock(m_snd->name, PAGETYPE_SOUND);
  }

  {
    auto d = app.current_sound;
    app.current_sound = GetNextSound(d);
    FreeSound(d);
  }
  mng_EraseLocker();
  QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Sound deleted.");
  RemapSounds();
  updateDialog();
}

void WorldSoundsDialog::onLockSound() {
  const int n = app.current_sound;
  if (!static_cast<int>(Sounds.size()))
    return;
  if (!mng_MakeLocker())
    return;

  mngs_Pagelock temp_pl;
  mngs_sound_page soundpage;
  temp_pl.name = m_snd->name;
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
        if (!mng_ReplacePage(m_snd->name, m_snd->name, n, PAGETYPE_SOUND, 1)) {
          QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was problem writing that page locally!");
          mng_EraseLocker();
          return;
        }
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Sound locked.");
      } else {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was a problem loading this sound.");
      }
      mng_AllocTrackLock(m_snd->name, PAGETYPE_SOUND);
    } else {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't find that sound in the table file!");
    }
  }
  mng_EraseLocker();
  updateDialog();
}

void WorldSoundsDialog::onCheckinSound() {
  const int n = app.current_sound;
  if (!static_cast<int>(Sounds.size()))
    return;
  if (!mng_MakeLocker())
    return;

  mngs_Pagelock temp_pl;
  temp_pl.name = m_snd->name;
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
    if (!mng_ReplacePage(m_snd->name, m_snd->name, n, PAGETYPE_SOUND, 0))
      QMessageBox::critical(this, "Error!", ErrorString);
    else {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Sound checked in.");
      Q_ASSERT(mng_DeletePage(m_snd->name, PAGETYPE_SOUND, 1) == 1);
      mng_EraseLocker();
      const int p = mng_FindTrackLock(m_snd->name, PAGETYPE_SOUND).value_or(-1);
      Q_ASSERT(p != -1);
      mng_FreeTrackLock(p);
    }
  }
  mng_EraseLocker();
  updateDialog();
}

void WorldSoundsDialog::onPlaysound() {
  const int n = app.current_sound;
  if (!static_cast<int>(Sounds.size()))
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
  temp_pl.name = m_snd->name;
  temp_pl.pagetype = PAGETYPE_SOUND;
  mng_OverrideToUnlocked(&temp_pl);
}

void WorldSoundsDialog::onChangeName() {
  const std::optional<uint32_t> p = mng_FindTrackLock(m_snd->name, PAGETYPE_SOUND);
  if (!p) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You must lock this sound if you wish to change its name.");
    return;
  }
  bool ok = false;
  QString name = QInputDialog::getText(this, "Sound name", "Enter a new name for this sound:",
                                       QLineEdit::Normal, QString::fromStdString(m_snd->name), &ok);
  if (!ok)
    return;
  if (!FindSoundName(name.toStdString())) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "That name is taken, please choose another.");
    return;
  }
  if (!mng_MakeLocker())
    return;
  mngs_Pagelock pl;
  pl.name = m_snd->name;
  pl.pagetype = PAGETYPE_SOUND;
  const int ret = mng_CheckIfPageOwned(&pl, TableUser.toStdString());
  if (ret < 0)
    QMessageBox::critical(this, "Error!", ErrorString);
  else if (ret == 1)
    mng_RenamePage(m_snd->name, name.toStdString(), PAGETYPE_SOUND);
  else if (ret == 2) {
    GlobalTrackLocks[*p].name = name.toStdString();
    mng_ReplacePage(GlobalTrackLocks[*p].name, name.toStdString(), app.current_sound, PAGETYPE_SOUND, 1);
  } else if (ret == 0) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You don't own this page.  Get Jason now!");
    mng_FreeTrackLock(*p);
    mng_EraseLocker();
    return;
  }
  GlobalTrackLocks[*p].name = name.toStdString();
  m_snd->name = name.toStdString();
  mng_EraseLocker();
  RemapSounds();
  updateDialog();
}

void WorldSoundsDialog::onSoundPulldownChanged() {
  QComboBox *combo = ui->IDC_SOUND_PULLDOWN;
  const int i = FindSoundName(combo->currentText().toStdString()).value_or(-1);
  if (i == -1)
    return;
  app.current_sound = i;
  updateDialog();
}

void WorldSoundsDialog::onMaxDistEdited() {  
  m_snd->max_distance = ui->IDC_SOUNDMAXDIST_EDIT->text().toFloat();
}
void WorldSoundsDialog::onMinDistEdited() {
  m_snd->min_distance = ui->IDC_SOUNDMINDIST_EDIT->text().toFloat();
}
void WorldSoundsDialog::onInnerConeEdited() {
  m_snd->inner_cone_angle = ui->IDC_SOUNDINNERCONEANGLE_EDIT->text().toInt();
}
void WorldSoundsDialog::onOuterConeAngleEdited() {
  m_snd->outer_cone_angle = ui->IDC_SOUNDOUTERCONEANGLE_EDIT->text().toInt();
}
void WorldSoundsDialog::onOuterConeVolEdited() {
  m_snd->outer_cone_volume = ui->IDC_SOUNDOUTERCONEVOL_EDIT->text().toFloat() / 100.0f;
}
void WorldSoundsDialog::onLoopStartEdited() {
  m_snd->loop_start = ui->IDC_SOUNDLOOPSTART_EDIT->text().toInt();
}
void WorldSoundsDialog::onLoopEndEdited() {
  m_snd->loop_end = ui->IDC_SOUNDLOOPEND_EDIT->text().toInt();
}
void WorldSoundsDialog::onImportVolumeEdited() {
  m_snd->import_volume = ui->IDC_SOUND_IMPORT_VOLUME_EDIT->text().toFloat() / 100.0f;
}

