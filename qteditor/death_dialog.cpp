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

#include "death_dialog.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

namespace QtEditor {

namespace {
death_info Paste_data{};
bool Paste_data_used = false;
} // namespace

DeathDialog::DeathDialog(death_info *info, QWidget *parent)
    : Dialog(":/ui/death_dialog.ui", parent), m_info(info), m_flags(info->flags),
      m_delayMin(info->delay_min), m_delayMax(info->delay_max) {
  auto *delayMax = find<QLineEdit>("IDC_DEATH_DELAY_MAX");
  auto *delayMin = find<QLineEdit>("IDC_DEATH_DELAY_MIN");
  delayMax->setText(QString::number(m_delayMax));
  delayMin->setText(QString::number(m_delayMin));

  struct FlagBox {
    const char *name;
    uint32_t flag;
  };
  const FlagBox flagBoxes[] = {
      {"IDC_DEATH_DELAY_FROM_ANIM", DF_DELAY_FROM_ANIM},
      {"IDC_DEATH_DELAY_LOSES_ANTIGRAV", DF_DELAY_LOSES_ANTIGRAV},
      {"IDC_DEATH_DELAY_SPARKS", DF_DELAY_SPARKS},
      {"IDC_DEATH_DELAY_SMOKE", DF_DELAY_SMOKES},
      {"IDC_DEATH_DELAY_FLYING", DF_DELAY_FLYING},
      {"IDC_DEATH_DELAY_FIREBALLS", DF_DELAY_FIREBALL},
      {"IDC_DEATH_DELAY_FADE_AWAY", DF_DELAY_FADE_AWAY},
      {"IDC_DEATH_DELAY_SOUND", DF_DELAY_SOUND},
      {"IDC_DEATH_DELAY_NO_TUMBLE", DF_DELAY_NO_TUMBLE_FLY},
      {"IDC_DEATH_FIREBALL", DF_FIREBALL},
      {"IDC_DEATH_BREAKS_APART", DF_BREAKS_APART},
      {"IDC_DEATH_FADES_AWAY", DF_FADE_AWAY},
      {"IDC_DEATH_BLAST_RING", DF_BLAST_RING},
      {"IDC_DEATH_REMAINS", DF_REMAINS},
      {"IDC_DEATH_LOSES_ANTIGRAV", DF_LOSES_ANTIGRAV},
      {"IDC_DEATH_CONTACT_FIREBALL", DF_CONTACT_FIREBALL},
      {"IDC_DEATH_CONTACT_BREAKS_APART", DF_CONTACT_BREAKS_APART},
      {"IDC_DEATH_CONTACT_BLAST_RING", DF_CONTACT_BLAST_RING},
      {"IDC_DEATH_CONTACT_REMAINS", DF_CONTACT_REMAINS},
      {"IDC_DEATH_DEBRIS_SMOKES", DF_DEBRIS_SMOKES},
      {"IDC_DEATH_DEBRIS_CONTACT_FIREBALL", DF_DEBRIS_FIREBALL},
      {"IDC_DEATH_DEBRIS_CONTACT_BLAST_RING", DF_DEBRIS_BLAST_RING},
      {"IDC_DEATH_DEBRIS_CONTACT_REMAINS", DF_DEBRIS_REMAINS},
  };
  for (const FlagBox &box : flagBoxes) {
    if (auto *cb = find<QCheckBox>(box.name))
      connect(cb, &QCheckBox::clicked, this, [this, box](bool checked) {
        if (checked)
          m_flags |= box.flag;
        else
          m_flags &= ~box.flag;
        updateDialog();
      });
  }

  if (auto *rb = find<QRadioButton>("IDC_DEATH_EXPLOSION_SMALL"))
    connect(rb, &QRadioButton::clicked, this, &DeathDialog::onExplosionSmall);
  if (auto *rb = find<QRadioButton>("IDC_DEATH_EXPLOSION_MEDIUM"))
    connect(rb, &QRadioButton::clicked, this, &DeathDialog::onExplosionMedium);
  if (auto *rb = find<QRadioButton>("IDC_DEATH_EXPLOSION_LARGE"))
    connect(rb, &QRadioButton::clicked, this, &DeathDialog::onExplosionLarge);

  connect(delayMax, &QLineEdit::textEdited, this, [this] { updateDialog(); });
  connect(delayMin, &QLineEdit::textEdited, this, [this] { updateDialog(); });

  if (QPushButton *ok = find<QPushButton>("IDOK")) {
    disconnect(ok, &QPushButton::clicked, m_dialog, &QDialog::accept);
    connect(ok, &QPushButton::clicked, this, &DeathDialog::onOk);
  }
  auto connectCommon = [this](const char *name, void (DeathDialog::*slot)()) {
    if (auto *btn = find<QPushButton>(name))
      connect(btn, &QPushButton::clicked, this, slot);
  };
  connectCommon("IDC_DEATH_COMMON_DESCENT", &DeathDialog::onCommonDescent);
  connectCommon("IDC_DEATH_COMMON_QUICK_EXPLOSION", &DeathDialog::onCommonQuickExplosion);
  connectCommon("IDC_DEATH_COMMON_SHOOT_UP", &DeathDialog::onCommonShootUp);
  connectCommon("IDC_DEATH_COMMON_R2D2", &DeathDialog::onCommonR2D2);
  connectCommon("IDC_DEATH_COMMON_ANIMAL", &DeathDialog::onCommonAnimal);
  connectCommon("IDC_DEATH_COPY", &DeathDialog::onCopy);
  connectCommon("IDC_DEATH_PASTE", &DeathDialog::onPaste);

  updateDialog();
}

DeathDialog::~DeathDialog() = default;

void DeathDialog::updateDialog() {
  m_delayMin = find<QLineEdit>("IDC_DEATH_DELAY_MIN")->text().toFloat();
  m_delayMax = find<QLineEdit>("IDC_DEATH_DELAY_MAX")->text().toFloat();

  const int explSize = (m_flags & DF_EXPL_SIZE_MASK) >> DF_EXPL_SIZE_SHIFT;
  find<QRadioButton>("IDC_DEATH_EXPLOSION_SMALL")->setChecked(explSize == 0);
  find<QRadioButton>("IDC_DEATH_EXPLOSION_MEDIUM")->setChecked(explSize == 1);
  find<QRadioButton>("IDC_DEATH_EXPLOSION_LARGE")->setChecked(explSize == 2);

  struct FlagBox {
    const char *name;
    uint32_t flag;
  };
  const FlagBox boxes[] = {
      {"IDC_DEATH_DELAY_FROM_ANIM", DF_DELAY_FROM_ANIM},
      {"IDC_DEATH_DELAY_LOSES_ANTIGRAV", DF_DELAY_LOSES_ANTIGRAV},
      {"IDC_DEATH_DELAY_SPARKS", DF_DELAY_SPARKS},
      {"IDC_DEATH_DELAY_SMOKE", DF_DELAY_SMOKES},
      {"IDC_DEATH_DELAY_FLYING", DF_DELAY_FLYING},
      {"IDC_DEATH_DELAY_FIREBALLS", DF_DELAY_FIREBALL},
      {"IDC_DEATH_DELAY_FADE_AWAY", DF_DELAY_FADE_AWAY},
      {"IDC_DEATH_DELAY_SOUND", DF_DELAY_SOUND},
      {"IDC_DEATH_DELAY_NO_TUMBLE", DF_DELAY_NO_TUMBLE_FLY},
      {"IDC_DEATH_FIREBALL", DF_FIREBALL},
      {"IDC_DEATH_BREAKS_APART", DF_BREAKS_APART},
      {"IDC_DEATH_FADES_AWAY", DF_FADE_AWAY},
      {"IDC_DEATH_BLAST_RING", DF_BLAST_RING},
      {"IDC_DEATH_REMAINS", DF_REMAINS},
      {"IDC_DEATH_LOSES_ANTIGRAV", DF_LOSES_ANTIGRAV},
      {"IDC_DEATH_CONTACT_FIREBALL", DF_CONTACT_FIREBALL},
      {"IDC_DEATH_CONTACT_BREAKS_APART", DF_CONTACT_BREAKS_APART},
      {"IDC_DEATH_CONTACT_BLAST_RING", DF_CONTACT_BLAST_RING},
      {"IDC_DEATH_CONTACT_REMAINS", DF_CONTACT_REMAINS},
      {"IDC_DEATH_DEBRIS_SMOKES", DF_DEBRIS_SMOKES},
      {"IDC_DEATH_DEBRIS_CONTACT_FIREBALL", DF_DEBRIS_FIREBALL},
      {"IDC_DEATH_DEBRIS_CONTACT_BLAST_RING", DF_DEBRIS_BLAST_RING},
      {"IDC_DEATH_DEBRIS_CONTACT_REMAINS", DF_DEBRIS_REMAINS},
  };
  for (const FlagBox &box : boxes)
    find<QCheckBox>(box.name)->setChecked((m_flags & box.flag) != 0);

  const bool delay = (m_flags & DF_DELAY_FROM_ANIM) != 0 || m_delayMin != 0.0f || m_delayMax != 0.0f;
  const char *delayWidgets[] = {"IDC_DEATH_DELAY_GROUP", "IDC_DEATH_DELAY_LOSES_ANTIGRAV",
                                "IDC_DEATH_DELAY_SPARKS", "IDC_DEATH_DELAY_SMOKE",
                                "IDC_DEATH_DELAY_FLYING", "IDC_DEATH_DELAY_FIREBALLS",
                                "IDC_DEATH_DELAY_FADE_AWAY", "IDC_DEATH_DELAY_SOUND",
                                "IDC_DEATH_DELAY_NO_TUMBLE"};
  for (const char *name : delayWidgets)
    find(name)->setEnabled(delay);
  find("IDC_DEATH_DELAY_NO_TUMBLE")->setEnabled(delay && (m_flags & DF_DELAY_FLYING) != 0);

  const bool userDelay = (m_flags & DF_DELAY_FROM_ANIM) == 0;
  find("IDC_DEATH_DELAY_MIN")->setEnabled(userDelay);
  find("IDC_DEATH_DELAY_MAX")->setEnabled(userDelay);
  find("IDC_DEATH_DELAY_MIN_LABEL")->setEnabled(userDelay);
  find("IDC_DEATH_DELAY_MAX_LABEL")->setEnabled(userDelay);

  const char *contactWidgets[] = {"IDC_DEATH_CONTACT_FIREBALL", "IDC_DEATH_CONTACT_BREAKS_APART",
                                  "IDC_DEATH_CONTACT_BLAST_RING", "IDC_DEATH_CONTACT_REMAINS",
                                  "IDC_DEATH_CONTACT_GROUP"};
  for (const char *name : contactWidgets)
    find(name)->setEnabled(delay);

  const bool explodes = (m_flags & DF_FIREBALL) != 0;
  const char *explosionWidgets[] = {"IDC_DEATH_EXPLOSION_SMALL", "IDC_DEATH_EXPLOSION_MEDIUM",
                                    "IDC_DEATH_EXPLOSION_LARGE", "IDC_DEATH_EXPLOSION_OPTIONS_GROUP"};
  for (const char *name : explosionWidgets)
    find(name)->setEnabled(explodes);

  const bool breaksApart = (m_flags & DF_BREAKS_APART) != 0;
  const char *debrisWidgets[] = {"IDC_DEATH_DEBRIS_CONTACT_FIREBALL",
                                 "IDC_DEATH_DEBRIS_CONTACT_BLAST_RING",
                                 "IDC_DEATH_DEBRIS_CONTACT_REMAINS", "IDC_DEATH_DEBRIS_CONTACT_GROUP",
                                 "IDC_DEATH_DEBRIS_SMOKES", "IDC_DEATH_DEBRIS_OPTIONS_GROUP"};
  for (const char *name : debrisWidgets)
    find(name)->setEnabled(breaksApart);

  if (QPushButton *paste = find<QPushButton>("IDC_DEATH_PASTE"))
    paste->setEnabled(Paste_data_used);
}

void DeathDialog::onExplosionSmall() {
  m_flags = (m_flags & ~DF_EXPL_SIZE_MASK) | DF_EXPL_SMALL;
  updateDialog();
}
void DeathDialog::onExplosionMedium() {
  m_flags = (m_flags & ~DF_EXPL_SIZE_MASK) | DF_EXPL_MEDIUM;
  updateDialog();
}
void DeathDialog::onExplosionLarge() {
  m_flags = (m_flags & ~DF_EXPL_SIZE_MASK) | DF_EXPL_LARGE;
  updateDialog();
}

void DeathDialog::onCommonDescent() {
  m_flags = DF_DELAY_FIREBALL + DF_FIREBALL + DF_BREAKS_APART + DF_DEBRIS_FIREBALL + DF_EXPL_MEDIUM;
  m_delayMin = m_delayMax = 2.0f;
  find<QLineEdit>("IDC_DEATH_DELAY_MIN")->setText("2");
  find<QLineEdit>("IDC_DEATH_DELAY_MAX")->setText("2");
  updateDialog();
}
void DeathDialog::onCommonQuickExplosion() {
  m_flags = DF_FIREBALL + DF_BREAKS_APART + DF_DEBRIS_FIREBALL + DF_EXPL_MEDIUM;
  m_delayMin = m_delayMax = 0.0f;
  find<QLineEdit>("IDC_DEATH_DELAY_MIN")->setText("0");
  find<QLineEdit>("IDC_DEATH_DELAY_MAX")->setText("0");
  updateDialog();
}
void DeathDialog::onCommonShootUp() {
  m_flags = DF_DELAY_FLYING + DF_FIREBALL + DF_BREAKS_APART + DF_DEBRIS_FIREBALL + DF_EXPL_MEDIUM;
  m_delayMin = 1.0f;
  m_delayMax = 3.0f;
  find<QLineEdit>("IDC_DEATH_DELAY_MIN")->setText("1");
  find<QLineEdit>("IDC_DEATH_DELAY_MAX")->setText("3");
  updateDialog();
}
void DeathDialog::onCommonR2D2() {
  m_flags = DF_DELAY_SPARKS + DF_FIREBALL + DF_BREAKS_APART + DF_DEBRIS_FIREBALL + DF_EXPL_MEDIUM;
  m_delayMin = 2.0f;
  m_delayMax = 4.0f;
  find<QLineEdit>("IDC_DEATH_DELAY_MIN")->setText("2");
  find<QLineEdit>("IDC_DEATH_DELAY_MAX")->setText("4");
  updateDialog();
}
void DeathDialog::onCommonAnimal() {
  m_flags = DF_CONTACT_REMAINS;
  m_delayMin = m_delayMax = 0.0f;
  find<QLineEdit>("IDC_DEATH_DELAY_MIN")->setText("0");
  find<QLineEdit>("IDC_DEATH_DELAY_MAX")->setText("0");
  updateDialog();
}

void DeathDialog::onCopy() {
  Paste_data.flags = m_flags;
  Paste_data.delay_min = m_delayMin;
  Paste_data.delay_max = m_delayMax;
  Paste_data_used = true;
  updateDialog();
}

void DeathDialog::onPaste() {
  m_flags = Paste_data.flags;
  m_delayMin = Paste_data.delay_min;
  m_delayMax = Paste_data.delay_max;
  find<QLineEdit>("IDC_DEATH_DELAY_MIN")->setText(QString::number(m_delayMin));
  find<QLineEdit>("IDC_DEATH_DELAY_MAX")->setText(QString::number(m_delayMax));
  updateDialog();
}

void DeathDialog::onOk() {
  if (m_delayMax < m_delayMin) {
    QMessageBox::warning(m_dialog, "Error", "Max delay must be greater than or equal to min delay.");
    return;
  }
  m_info->flags = m_flags;
  m_info->delay_min = m_delayMin;
  m_info->delay_max = m_delayMax;
  m_dialog->accept();
}

}
