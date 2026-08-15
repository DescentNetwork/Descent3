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

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

#include "cfile.h"
#include "qt_messagebox.h"
#include "d3edit.h"
#include "ddio.h"
#include "hlsoundlib.h"
#include "manage.h"
#include "qt_debug.h"
#include "soundpage.h"
#include "soundload.h"
#include "ssl_lib.h"

extern char Current_sounds_dir[_MAX_PATH];

namespace QtEditor {

namespace {
// Writes a single float/int field back into the current sound.
} // namespace

WorldSoundsDialog::WorldSoundsDialog(QWidget *parent) : Dialog(":/ui/worldsounds.ui", parent) {
  if (QPushButton *b = find<QPushButton>("IDC_ADD_SOUND"))
    connect(b, &QPushButton::clicked, this, &WorldSoundsDialog::onAddSound);
  if (QPushButton *b = find<QPushButton>("IDC_LOAD_SOUND"))
    connect(b, &QPushButton::clicked, this, &WorldSoundsDialog::onLoadSound);
  if (QPushButton *b = find<QPushButton>("IDC_NEXT_SOUND"))
    connect(b, &QPushButton::clicked, this, &WorldSoundsDialog::onNextSound);
  if (QPushButton *b = find<QPushButton>("IDC_PREV_SOUND"))
    connect(b, &QPushButton::clicked, this, &WorldSoundsDialog::onPrevSound);
  if (QPushButton *b = find<QPushButton>("IDC_DELETE_SOUND"))
    connect(b, &QPushButton::clicked, this, &WorldSoundsDialog::onDeleteSound);
  if (QPushButton *b = find<QPushButton>("IDC_LOCK_SOUND"))
    connect(b, &QPushButton::clicked, this, &WorldSoundsDialog::onLockSound);
  if (QPushButton *b = find<QPushButton>("IDC_CHECKIN_SOUND"))
    connect(b, &QPushButton::clicked, this, &WorldSoundsDialog::onCheckinSound);
  if (QPushButton *b = find<QPushButton>("IDC_PLAYSOUND"))
    connect(b, &QPushButton::clicked, this, &WorldSoundsDialog::onPlaysound);
  if (QPushButton *b = find<QPushButton>("IDC_KILLSOUNDS"))
    connect(b, &QPushButton::clicked, this, &WorldSoundsDialog::onKillsounds);
  if (QPushButton *b = find<QPushButton>("IDC_OVERRIDE"))
    connect(b, &QPushButton::clicked, this, &WorldSoundsDialog::onOverride);
  if (QPushButton *b = find<QPushButton>("IDC_SOUND_CHANGE_NAME"))
    connect(b, &QPushButton::clicked, this, &WorldSoundsDialog::onChangeName);

  if (QComboBox *combo = find<QComboBox>("IDC_SOUND_PULLDOWN"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &WorldSoundsDialog::onSoundPulldownChanged);

  const char *edits[] = {"IDC_SOUNDMAXDIST_EDIT",    "IDC_SOUNDMINDIST_EDIT",   "IDC_SOUNDINNERCONEANGLE_EDIT",
                         "IDC_SOUNDOUTERCONEANGLE_EDIT", "IDC_SOUNDOUTERCONEVOL_EDIT", "IDC_SOUNDLOOPSTART_EDIT",
                         "IDC_SOUNDLOOPEND_EDIT",    "IDC_SOUND_IMPORT_VOLUME_EDIT"};
  const auto slotFor = [&](const char *name) {
    if (QString::compare(name, "IDC_SOUNDMAXDIST_EDIT") == 0)
      return &WorldSoundsDialog::onMaxDistEdited;
    if (QString::compare(name, "IDC_SOUNDMINDIST_EDIT") == 0)
      return &WorldSoundsDialog::onMinDistEdited;
    if (QString::compare(name, "IDC_SOUNDINNERCONEANGLE_EDIT") == 0)
      return &WorldSoundsDialog::onInnerConeEdited;
    if (QString::compare(name, "IDC_SOUNDOUTERCONEANGLE_EDIT") == 0)
      return &WorldSoundsDialog::onOuterConeAngleEdited;
    if (QString::compare(name, "IDC_SOUNDOUTERCONEVOL_EDIT") == 0)
      return &WorldSoundsDialog::onOuterConeVolEdited;
    if (QString::compare(name, "IDC_SOUNDLOOPSTART_EDIT") == 0)
      return &WorldSoundsDialog::onLoopStartEdited;
    if (QString::compare(name, "IDC_SOUNDLOOPEND_EDIT") == 0)
      return &WorldSoundsDialog::onLoopEndEdited;
    return &WorldSoundsDialog::onImportVolumeEdited;
  };
  for (const char *name : edits) {
    if (QLineEdit *edit = find<QLineEdit>(name))
      connect(edit, &QLineEdit::editingFinished, this, slotFor(name));
  }

  struct {
    const char *name;
    void (WorldSoundsDialog::*slot)(bool);
  } checks[] = {
      {"IDC_SOUNDHALLEFFECT_CHECK", &WorldSoundsDialog::onHallEffectToggled},
      {"IDC_LOOPING_CHECK", &WorldSoundsDialog::onLoopingToggled},
      {"IDC_SOUNDFOREVER_CHECK", &WorldSoundsDialog::onForeverToggled},
      {"IDC_SOUNDEXCLUSIVE_CHECK", &WorldSoundsDialog::onExclusiveToggled},
      {"IDC_SOUNDONCE_CHECK", &WorldSoundsDialog::onOnceToggled},
      {"IDC_SOUND_ONCE_PER_OBJ_CHECK", &WorldSoundsDialog::onOncePerObjToggled},
      {"IDC_SOUND_NO_UPDATE", &WorldSoundsDialog::onNoUpdateToggled},
  };
  for (const auto &c : checks)
    if (QCheckBox *cb = find<QCheckBox>(c.name))
      connect(cb, &QCheckBox::toggled, this, c.slot);

  if (QRadioButton *rb = find<QRadioButton>("IDC_SOUNDOBJATTACH_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &WorldSoundsDialog::onObjAttach);
  if (QRadioButton *rb = find<QRadioButton>("IDC_SOUNDPOSATTACH_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &WorldSoundsDialog::onPosAttach);
  if (QRadioButton *rb = find<QRadioButton>("IDC_SOUNDOBJECT_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &WorldSoundsDialog::onConeLinkObject);
  if (QRadioButton *rb = find<QRadioButton>("IDC_SOUNDTURRET1_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &WorldSoundsDialog::onConeLinkTurret1);
  if (QRadioButton *rb = find<QRadioButton>("IDC_SOUNDTURRET2_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &WorldSoundsDialog::onConeLinkTurret2);
  if (QRadioButton *rb = find<QRadioButton>("IDC_SOUNDTURRET3_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &WorldSoundsDialog::onConeLinkTurret3);
  if (QRadioButton *rb = find<QRadioButton>("IDC_SOUNDFORWARD_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &WorldSoundsDialog::onConeDirForward);
  if (QRadioButton *rb = find<QRadioButton>("IDC_SOUNDBACKWARD_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &WorldSoundsDialog::onConeDirBackward);
  if (QRadioButton *rb = find<QRadioButton>("IDC_SOUNDUPWARD_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &WorldSoundsDialog::onConeDirUpward);
  if (QRadioButton *rb = find<QRadioButton>("IDC_SOUNDDOWNWARD_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &WorldSoundsDialog::onConeDirDownward);

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
  const int n = D3EditState.current_sound;
  if (n < 0 || n >= MAX_SOUNDS || !Sounds[n].used)
    return;
  if (checked)
    Sounds[n].flags |= flag;
  else
    Sounds[n].flags &= ~flag;
}

void WorldSoundsDialog::setConeLink(int value) {
  setFlag(SPFT_CONE_LINK_MASK, "", true);
  const int n = D3EditState.current_sound;
  if (n >= 0 && n < MAX_SOUNDS)
    Sounds[n].flags = (Sounds[n].flags & ~SPFT_CONE_LINK_MASK) | value;
}

void WorldSoundsDialog::setConeDir(int value) {
  const int n = D3EditState.current_sound;
  if (n >= 0 && n < MAX_SOUNDS)
    Sounds[n].flags = (Sounds[n].flags & ~SPFT_CONE_DIR_MASK) | value;
}

void WorldSoundsDialog::updateDialog() {
  const int n = D3EditState.current_sound;

  if (QPushButton *next = find<QPushButton>("IDC_NEXT_SOUND"))
    next->setEnabled(Num_sounds >= 1);
  if (QPushButton *prev = find<QPushButton>("IDC_PREV_SOUND"))
    prev->setEnabled(Num_sounds >= 1);
  if (!Network_up) {
    for (const char *name : {"IDC_LOCK_SOUND", "IDC_CHECKIN_SOUND", "IDC_OVERRIDE"}) {
      if (auto *w = find<QPushButton>(name))
        w->setEnabled(false);
    }
    return;
  }
  if (Num_sounds < 1)
    return;

  if (!Sounds[n].used)
    D3EditState.current_sound = GetNextSound(n);

  Sound_system.CheckAndForceSoundDataAlloc(D3EditState.current_sound);
  const int s = D3EditState.current_sound;

  int total_memory = 0;
  for (int i = 0; i < MAX_SOUNDS; i++)
    if (Sounds[i].used)
      total_memory += SoundFiles[Sounds[i].sample_index].sample_length * 2;

  if (QLineEdit *edit = find<QLineEdit>("IDC_SOUNDMAXDIST_EDIT"))
    edit->setText(QString::number(Sounds[s].max_distance));
  if (QLineEdit *edit = find<QLineEdit>("IDC_SOUNDMINDIST_EDIT"))
    edit->setText(QString::number(Sounds[s].min_distance));
  if (QLineEdit *edit = find<QLineEdit>("IDC_SOUNDINNERCONEANGLE_EDIT"))
    edit->setText(QString::number(Sounds[s].inner_cone_angle));
  if (QLineEdit *edit = find<QLineEdit>("IDC_SOUNDOUTERCONEANGLE_EDIT"))
    edit->setText(QString::number(Sounds[s].outer_cone_angle));
  if (QLineEdit *edit = find<QLineEdit>("IDC_SOUNDOUTERCONEVOL_EDIT"))
    edit->setText(QString::number(Sounds[s].outer_cone_volume * 100.0f));
  if (QLineEdit *edit = find<QLineEdit>("IDC_SOUNDLOOPSTART_EDIT"))
    edit->setText(QString::number(Sounds[s].loop_start));

  if (Sounds[s].loop_end >= SoundFiles[Sounds[s].sample_index].np_sample_length)
    Sounds[s].loop_end = SoundFiles[Sounds[s].sample_index].np_sample_length - 1;
  if (Sounds[s].loop_start > Sounds[s].loop_end)
    Sounds[s].loop_start = 0;
  if (QLineEdit *edit = find<QLineEdit>("IDC_SOUNDLOOPEND_EDIT"))
    edit->setText(QString::number(Sounds[s].loop_end));

  if (QLineEdit *edit = find<QLineEdit>("IDC_SOUND_IMPORT_VOLUME_EDIT"))
    edit->setText(QString::number(Sounds[s].import_volume * 100.0f));
  if (QLineEdit *edit = find<QLineEdit>("IDC_RAW_NAME_EDIT"))
    edit->setText(SoundFiles[Sounds[s].sample_index].name);

  if (QLabel *label = find<QLabel>("IDC_SOUND_MEMORY_STATIC"))
    label->setText(QString("%1, %2 H, %3 Total")
                       .arg(SoundFiles[Sounds[s].sample_index].sample_length / 512)
                       .arg(0)
                       .arg(total_memory / 1024));

  if (QCheckBox *cb = find<QCheckBox>("IDC_SOUNDHALLEFFECT_CHECK"))
    cb->setChecked(!(Sounds[s].flags & SPF_FIXED_FREQ));
  if (QCheckBox *cb = find<QCheckBox>("IDC_SOUNDFOREVER_CHECK"))
    cb->setChecked(Sounds[s].flags & SPF_FOREVER);
  if (QCheckBox *cb = find<QCheckBox>("IDC_SOUND_ONCE_PER_OBJ_CHECK"))
    cb->setChecked(Sounds[s].flags & SPF_ONCE_PER_OBJ);
  if (QCheckBox *cb = find<QCheckBox>("IDC_SOUNDEXCLUSIVE_CHECK"))
    cb->setChecked(Sounds[s].flags & SPF_PLAYS_EXCLUSIVELY);
  if (QCheckBox *cb = find<QCheckBox>("IDC_SOUND_NO_UPDATE"))
    cb->setChecked(Sounds[s].flags & SPF_LISTENER_UPDATE);
  if (QCheckBox *cb = find<QCheckBox>("IDC_SOUNDONCE_CHECK"))
    cb->setChecked(Sounds[s].flags & SPF_PLAYS_ONCE);
  if (QCheckBox *cb = find<QCheckBox>("IDC_LOOPING_CHECK"))
    cb->setChecked(Sounds[s].flags & SPF_LOOPED);

  if (QRadioButton *rb = find<QRadioButton>("IDC_SOUNDOBJATTACH_RADIO"))
    rb->setChecked(Sounds[s].flags & SPF_OBJ_UPDATE);
  if (QRadioButton *rb = find<QRadioButton>("IDC_SOUNDPOSATTACH_RADIO"))
    rb->setChecked(!(Sounds[s].flags & SPF_OBJ_UPDATE));

  switch (SPFT_CONE_LINK_MASK & Sounds[s].flags) {
  case SPFT_CONE_LINK_TURRET1:
    find<QRadioButton>("IDC_SOUNDTURRET1_RADIO")->setChecked(true);
    break;
  case SPFT_CONE_LINK_TURRET2:
    find<QRadioButton>("IDC_SOUNDTURRET2_RADIO")->setChecked(true);
    break;
  case SPFT_CONE_LINK_TURRET3:
    find<QRadioButton>("IDC_SOUNDTURRET3_RADIO")->setChecked(true);
    break;
  default:
    find<QRadioButton>("IDC_SOUNDOBJECT_RADIO")->setChecked(true);
    break;
  }

  switch (SPFT_CONE_DIR_MASK & Sounds[s].flags) {
  case SPFT_CONE_DIR_BACKWARD:
    find<QRadioButton>("IDC_SOUNDBACKWARD_RADIO")->setChecked(true);
    break;
  case SPFT_CONE_DIR_UPWARD:
    find<QRadioButton>("IDC_SOUNDUPWARD_RADIO")->setChecked(true);
    break;
  case SPFT_CONE_DIR_DOWNWARD:
    find<QRadioButton>("IDC_SOUNDDOWNWARD_RADIO")->setChecked(true);
    break;
  default:
    find<QRadioButton>("IDC_SOUNDFORWARD_RADIO")->setChecked(true);
    break;
  }

  if (QPushButton *checkin = find<QPushButton>("IDC_CHECKIN_SOUND")) {
    if (mng_FindTrackLock(Sounds[s].name, PAGETYPE_SOUND) == -1) {
      checkin->setEnabled(false);
      if (QPushButton *lock = find<QPushButton>("IDC_LOCK_SOUND"))
        lock->setEnabled(true);
      if (QPushButton *rename = find<QPushButton>("IDC_SOUND_CHANGE_NAME"))
        rename->setEnabled(false);
    } else {
      checkin->setEnabled(true);
      if (QPushButton *lock = find<QPushButton>("IDC_LOCK_SOUND"))
        lock->setEnabled(false);
      if (QPushButton *rename = find<QPushButton>("IDC_SOUND_CHANGE_NAME"))
        rename->setEnabled(true);
    }
  }

  if (QComboBox *combo = find<QComboBox>("IDC_SOUND_PULLDOWN")) {
    QSignalBlocker blocker(combo);
    combo->clear();
    for (int i = 0; i < MAX_SOUNDS; i++)
      if (Sounds[i].used)
        combo->addItem(Sounds[i].name);
    combo->setCurrentText(Sounds[s].name);
  }
}

void WorldSoundsDialog::onAddSound() {
  if (!Network_up) {
    OutrageMessageBox("Sorry babe, the network is down.  This action is a no-no.\n");
    return;
  }

  const QString pathname =
      QFileDialog::getOpenFileName(m_dialog, "Select sound", Current_sounds_dir, "Descent III files (*.wav)");
  if (pathname.isEmpty())
    return;

  const QByteArray pathBytes = pathname.toLocal8Bit();
  char dir[260], fname[128], ext[32];
  ddio_SplitPath(pathBytes.constData(), dir, fname, ext);

  const int raw_handle = LoadSoundFile(pathBytes.constData(), 1.0f, true);
  if (raw_handle < 0) {
    OutrageMessageBox("Invalid sound file.");
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
    if (FindSoundName(cur_name) != -1)
      c++;
    else
      finding_name = false;
  }

  snprintf(Sounds[sound_handle].name, sizeof(Sounds[sound_handle].name), "%s", cur_name);
  Sounds[sound_handle].sample_index = raw_handle;

  std::filesystem::path destname = LocalSoundsDir / SoundFiles[Sounds[sound_handle].sample_index].name;
  cf_CopyFile(destname, pathname.toStdString());

  mng_AllocTrackLock(cur_name, PAGETYPE_SOUND);
  D3EditState.current_sound = sound_handle;
  RemapSounds();
  Sound_system.CheckAndForceSoundDataAlloc(D3EditState.current_sound);
  Sounds[sound_handle].loop_end = SoundFiles[Sounds[sound_handle].sample_index].np_sample_length - 1;
  updateDialog();
}

void WorldSoundsDialog::onLoadSound() {
  if (!Network_up)
    return;
  const QString pathname =
      QFileDialog::getOpenFileName(m_dialog, "Load sound", Current_sounds_dir, "Descent III files (*.wav)");
  if (pathname.isEmpty())
    return;
  const int n = D3EditState.current_sound;
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
  D3EditState.current_sound = GetNextSound(D3EditState.current_sound);
  updateDialog();
}
void WorldSoundsDialog::onPrevSound() {
  D3EditState.current_sound = GetPrevSound(D3EditState.current_sound);
  updateDialog();
}

void WorldSoundsDialog::onDeleteSound() {
  const int n = D3EditState.current_sound;
  if (Num_sounds < 1)
    return;
  const int tl = mng_FindTrackLock(Sounds[n].name, PAGETYPE_SOUND);
  if (tl == -1) {
    OutrageMessageBox("This sound is not yours to delete.  Lock first.");
    return;
  }
  if (QMessageBox::question(m_dialog, "Delete sound",
                            QString("Are you sure you want to delete this sound? %1").arg(Sounds[n].name)) !=
      QMessageBox::Yes)
    return;
  if (!mng_MakeLocker())
    return;

  mngs_Pagelock pl;
  snprintf(pl.name, sizeof(pl.name), "%s", Sounds[n].name);
  pl.pagetype = PAGETYPE_SOUND;

  if (mng_CheckIfPageOwned(&pl, TableUser) != 1) {
    mng_FreeTrackLock(tl);
    if (!mng_DeletePage(Sounds[n].name, PAGETYPE_SOUND, 1))
      Int3();
  } else {
    mng_FreeTrackLock(tl);
    mng_DeletePage(Sounds[n].name, PAGETYPE_SOUND, 1);
    mng_DeletePage(Sounds[n].name, PAGETYPE_SOUND, 0);
    mng_DeletePagelock(Sounds[n].name, PAGETYPE_SOUND);
  }

  D3EditState.current_sound = GetNextSound(n);
  FreeSound(n);
  mng_EraseLocker();
  OutrageMessageBox("Sound deleted.");
  RemapSounds();
  updateDialog();
}

void WorldSoundsDialog::onLockSound() {
  const int n = D3EditState.current_sound;
  if (Num_sounds < 1)
    return;
  if (!mng_MakeLocker())
    return;

  mngs_Pagelock temp_pl;
  mngs_sound_page soundpage;
  snprintf(temp_pl.name, sizeof(temp_pl.name), "%s", Sounds[n].name);
  temp_pl.pagetype = PAGETYPE_SOUND;

  const int r = mng_CheckIfPageLocked(&temp_pl);
  if (r == 2) {
    if (OutrageMessageBox(MBOX_YESNO,
                          "This page is not even in the table file, or the database maybe corrupt.  Override to "
                          "'Unlocked'? (Select NO if you don't know what you're doing)") == 1) {
      snprintf(temp_pl.holder, sizeof(temp_pl.holder), "UNLOCKED");
      if (!mng_ReplacePagelock(temp_pl.name, &temp_pl))
        QMessageBox::critical(m_dialog, "Error!", ErrorString);
    }
  } else if (r < 0) {
    OutrageMessageBox(ErrorString);
  } else if (r == 1) {
    OutrageMessageBox(InfoString);
  } else {
    snprintf(temp_pl.holder, sizeof(temp_pl.holder), "%s", TableUser);
    if (!mng_ReplacePagelock(temp_pl.name, &temp_pl)) {
      QMessageBox::critical(m_dialog, "Error!", ErrorString);
      mng_EraseLocker();
      return;
    }
    if (mng_FindSpecificSoundPage(temp_pl.name, &soundpage)) {
      if (mng_AssignSoundPageToSound(&soundpage, n)) {
        if (!mng_ReplacePage(Sounds[n].name, Sounds[n].name, n, PAGETYPE_SOUND, 1)) {
          OutrageMessageBox("There was problem writing that page locally!");
          mng_EraseLocker();
          return;
        }
        OutrageMessageBox("Sound locked.");
      } else {
        OutrageMessageBox("There was a problem loading this sound.");
      }
      mng_AllocTrackLock(Sounds[n].name, PAGETYPE_SOUND);
    } else {
      OutrageMessageBox("Couldn't find that sound in the table file!");
    }
  }
  mng_EraseLocker();
  updateDialog();
}

void WorldSoundsDialog::onCheckinSound() {
  const int n = D3EditState.current_sound;
  if (Num_sounds < 1)
    return;
  if (!mng_MakeLocker())
    return;

  mngs_Pagelock temp_pl;
  snprintf(temp_pl.name, sizeof(temp_pl.name), "%s", Sounds[n].name);
  temp_pl.pagetype = PAGETYPE_SOUND;

  const int r = mng_CheckIfPageOwned(&temp_pl, TableUser);
  if (r < 0)
    OutrageMessageBox(ErrorString);
  else if (r == 0)
    OutrageMessageBox(InfoString);
  else {
    snprintf(temp_pl.holder, sizeof(temp_pl.holder), "UNLOCKED");
    if (!mng_ReplacePagelock(temp_pl.name, &temp_pl)) {
      QMessageBox::critical(m_dialog, "Error!", ErrorString);
      mng_EraseLocker();
      return;
    }
    if (!mng_ReplacePage(Sounds[n].name, Sounds[n].name, n, PAGETYPE_SOUND, 0))
      OutrageMessageBox(ErrorString);
    else {
      OutrageMessageBox("Sound checked in.");
      ASSERT(mng_DeletePage(Sounds[n].name, PAGETYPE_SOUND, 1) == 1);
      mng_EraseLocker();
      const int p = mng_FindTrackLock(Sounds[n].name, PAGETYPE_SOUND);
      ASSERT(p != -1);
      mng_FreeTrackLock(p);
    }
  }
  mng_EraseLocker();
  updateDialog();
}

void WorldSoundsDialog::onPlaysound() {
  const int n = D3EditState.current_sound;
  if (Num_sounds < 1)
    return;
  Sound_system.BeginSoundFrame();
  Sound_system.Play2dSound(n);
  Sound_system.EndSoundFrame();
}

void WorldSoundsDialog::onKillsounds() {
  QMessageBox::information(m_dialog, "Kill sounds", "All sounds have been killed.");
  // The MFC original simply calls Sound_system.StopAllSounds(); the Qt port
  // stops any currently playing sound object.
  Sound_system.StopAllSounds();
}

void WorldSoundsDialog::onOverride() {
  const int n = D3EditState.current_sound;
  mngs_Pagelock temp_pl;
  snprintf(temp_pl.name, sizeof(temp_pl.name), "%s", Sounds[n].name);
  temp_pl.pagetype = PAGETYPE_SOUND;
  mng_OverrideToUnlocked(&temp_pl);
}

void WorldSoundsDialog::onChangeName() {
  const int n = D3EditState.current_sound;
  const int p = mng_FindTrackLock(Sounds[n].name, PAGETYPE_SOUND);
  if (p == -1) {
    OutrageMessageBox("You must lock this sound if you wish to change its name.");
    return;
  }
  bool ok = false;
  QString name = QInputDialog::getText(m_dialog, "Sound name", "Enter a new name for this sound:",
                                       QLineEdit::Normal, Sounds[n].name, &ok);
  if (!ok)
    return;
  if (FindSoundName(name.toLocal8Bit().constData()) != -1) {
    OutrageMessageBox("That name is taken, please choose another.");
    return;
  }
  if (!mng_MakeLocker())
    return;
  mngs_Pagelock pl;
  snprintf(pl.name, sizeof(pl.name), "%s", Sounds[n].name);
  pl.pagetype = PAGETYPE_SOUND;
  const QByteArray newName = name.toLocal8Bit();
  char newNameBuf[PAGENAME_LEN];
  snprintf(newNameBuf, sizeof(newNameBuf), "%s", newName.constData());
  const int ret = mng_CheckIfPageOwned(&pl, TableUser);
  if (ret < 0)
    OutrageMessageBox(ErrorString);
  else if (ret == 1)
    mng_RenamePage(Sounds[n].name, newNameBuf, PAGETYPE_SOUND);
  else if (ret == 2) {
    snprintf(GlobalTrackLocks[p].name, sizeof(GlobalTrackLocks[p].name), "%s", newName.constData());
    mng_ReplacePage(GlobalTrackLocks[p].name, newNameBuf, n, PAGETYPE_SOUND, 1);
  } else if (ret == 0) {
    OutrageMessageBox("You don't own this page.  Get Jason now!");
    mng_FreeTrackLock(p);
    mng_EraseLocker();
    return;
  }
  snprintf(GlobalTrackLocks[p].name, sizeof(GlobalTrackLocks[p].name), "%s", newName.constData());
  snprintf(Sounds[n].name, sizeof(Sounds[n].name), "%s", newName.constData());
  mng_EraseLocker();
  RemapSounds();
  updateDialog();
}

void WorldSoundsDialog::onSoundPulldownChanged() {
  QComboBox *combo = find<QComboBox>("IDC_SOUND_PULLDOWN");
  const int i = FindSoundName(combo->currentText().toLocal8Bit().constData());
  if (i == -1)
    return;
  D3EditState.current_sound = i;
  updateDialog();
}

void WorldSoundsDialog::onMaxDistEdited() {
  const int n = D3EditState.current_sound;
  if (QLineEdit *e = find<QLineEdit>("IDC_SOUNDMAXDIST_EDIT"))
    Sounds[n].max_distance = e->text().toFloat();
}
void WorldSoundsDialog::onMinDistEdited() {
  const int n = D3EditState.current_sound;
  if (QLineEdit *e = find<QLineEdit>("IDC_SOUNDMINDIST_EDIT"))
    Sounds[n].min_distance = e->text().toFloat();
}
void WorldSoundsDialog::onInnerConeEdited() {
  const int n = D3EditState.current_sound;
  if (QLineEdit *e = find<QLineEdit>("IDC_SOUNDINNERCONEANGLE_EDIT"))
    Sounds[n].inner_cone_angle = e->text().toInt();
}
void WorldSoundsDialog::onOuterConeAngleEdited() {
  const int n = D3EditState.current_sound;
  if (QLineEdit *e = find<QLineEdit>("IDC_SOUNDOUTERCONEANGLE_EDIT"))
    Sounds[n].outer_cone_angle = e->text().toInt();
}
void WorldSoundsDialog::onOuterConeVolEdited() {
  const int n = D3EditState.current_sound;
  if (QLineEdit *e = find<QLineEdit>("IDC_SOUNDOUTERCONEVOL_EDIT"))
    Sounds[n].outer_cone_volume = e->text().toFloat() / 100.0f;
}
void WorldSoundsDialog::onLoopStartEdited() {
  const int n = D3EditState.current_sound;
  if (QLineEdit *e = find<QLineEdit>("IDC_SOUNDLOOPSTART_EDIT"))
    Sounds[n].loop_start = e->text().toInt();
}
void WorldSoundsDialog::onLoopEndEdited() {
  const int n = D3EditState.current_sound;
  if (QLineEdit *e = find<QLineEdit>("IDC_SOUNDLOOPEND_EDIT"))
    Sounds[n].loop_end = e->text().toInt();
}
void WorldSoundsDialog::onImportVolumeEdited() {
  const int n = D3EditState.current_sound;
  if (QLineEdit *e = find<QLineEdit>("IDC_SOUND_IMPORT_VOLUME_EDIT"))
    Sounds[n].import_volume = e->text().toFloat() / 100.0f;
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

}
