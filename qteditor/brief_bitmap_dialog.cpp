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

#include "brief_bitmap_dialog.h"
#include "ui_brief_addbitmap.h"

#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

#include <array>
#include <cstring>
#include <string>

#include "brief_mission_flags_dialog.h"

namespace {

void copyToFilename(char *dst, size_t dstLen, const std::string &src) {
  std::strncpy(dst, src.c_str(), dstLen - 1);
  dst[dstLen - 1] = '\0';
}

// Bitmap effect radios indexed by m_iEffectType (0..8), mirroring win32.
static const std::array<const char *, 9> effectRadios = {
    "IDC_BRIEF_B_STATIC",     "IDC_BRIEF_B_BLURIN",      "IDC_BRIEF_B_BLUROUT",
    "IDC_BRIEF_B_SCANIN",     "IDC_BRIEF_B_SCANOUT",     "IDC_BRIEF_B_INVIN",
    "IDC_BRIEF_B_INVOUT",     "IDC_BRIEF_B_STRETCHIN",   "IDC_BRIEF_B_STRETCHOUT"};

int effectTypeFromDesc(const TCBMPDESC &desc) {
  switch (desc.type) {
  case TC_BMP_STATIC:
    return 0;
  case TC_BMP_BLUR:
    return desc.flags == TC_BMPF_IN ? 1 : 2;
  case TC_BMP_SCANLINE:
    return desc.flags == TC_BMPF_IN ? 3 : 4;
  case TC_BMP_INVERT:
    return desc.flags == TC_BMPF_IN ? 5 : 6;
  case TC_BMP_STRETCH:
    return desc.flags == TC_BMPF_IN ? 7 : 8;
  default:
    return 0;
  }
}

void effectTypeToDesc(int effectType, TCBMPDESC *desc) {
  switch (effectType) {
  case 0:
    desc->type = TC_BMP_STATIC;
    desc->flags = TC_BMPF_IN;
    break;
  case 1:
    desc->type = TC_BMP_BLUR;
    desc->flags = TC_BMPF_IN;
    break;
  case 2:
    desc->type = TC_BMP_BLUR;
    desc->flags = TC_BMPF_OUT;
    break;
  case 3:
    desc->type = TC_BMP_SCANLINE;
    desc->flags = TC_BMPF_IN;
    break;
  case 4:
    desc->type = TC_BMP_SCANLINE;
    desc->flags = TC_BMPF_OUT;
    break;
  case 5:
    desc->type = TC_BMP_INVERT;
    desc->flags = TC_BMPF_IN;
    break;
  case 6:
    desc->type = TC_BMP_INVERT;
    desc->flags = TC_BMPF_OUT;
    break;
  case 7:
    desc->type = TC_BMP_STRETCH;
    desc->flags = TC_BMPF_IN;
    break;
  case 8:
    desc->type = TC_BMP_STRETCH;
    desc->flags = TC_BMPF_OUT;
    break;
  }
}

} // namespace

BriefBitmapDialog::BriefBitmapDialog(TCBMPDESC *desc, QWidget *parent)
    : QDialog(parent), ui(new Ui::BriefBitmapDialog), m_effectType(0), m_screen(0)
{
  ui->setupUi(this);
  std::memset(&m_desc, 0, sizeof(TCBMPDESC));
  m_desc.type = TC_BMP_STATIC;

  if (desc) {
    m_desc.caps = desc->caps;
    if (desc->caps & TCBD_XY) {
      m_desc.x = desc->x;
      m_desc.y = desc->y;
    }
    if (desc->caps & TCBD_LOOPING)
      m_desc.looping = desc->looping;
    if (desc->caps & TCBD_WAITTIME)
      m_desc.waittime = desc->waittime;
    if (desc->caps & TCBD_SPEED)
      m_desc.speed = desc->speed;
    m_desc.type = desc->type;
    m_desc.flags = desc->flags;
    copyToFilename(m_desc.filename, MAX_FILELEN, desc->filename);
    m_desc.mission_mask_set = desc->mission_mask_set;
    m_desc.mission_mask_unset = desc->mission_mask_unset;
  }
  m_effectType = effectTypeFromDesc(m_desc);

  ui->IDC_BRIEF_B_FILENAME->setText(QString::fromStdString(m_desc.filename));
  ui->IDC_BRIEF_B_SPEED->setText(QString::number(m_desc.speed));
  ui->IDC_BRIEF_B_STARTTIME->setText(QString::number(m_desc.waittime));
  ui->IDC_BRIEF_B_X->setText(QString::number(m_desc.x));
  ui->IDC_BRIEF_B_Y->setText(QString::number(m_desc.y));
  ui->IDC_BRIEF_B_NORENDER->setChecked((m_desc.flags & TC_NOEARLYRENDER) != 0);

  if (m_effectType >= 0 && m_effectType < (int)effectRadios.size())
    if (auto *rb = findChild<QRadioButton*>(effectRadios[m_effectType]))
      rb->setChecked(true);

  if (auto *combo = ui->IDC_BRIEF_B_PREDEF) {
    combo->clear();
    combo->addItem("<Raw>");
    int layout = -1;
    if (PBlayouts && PBnum_layouts) {
      for (int i = 0; i < *PBnum_layouts; i++) {
        if (Briefing_screens[m_screen].layout == PBlayouts[i].filename)
          layout = i;
      }
      if (layout != -1) {
        for (int j = 0; j < PBlayouts[layout].num_bmps; j++)
          combo->addItem(QString("(%1,%2)").arg(PBlayouts[layout].bmps[j].x)
                                .arg(PBlayouts[layout].bmps[j].y));
      }
    }
    combo->setCurrentIndex(0);
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &BriefBitmapDialog::onPredefChanged);
  }

  if (auto *btn = ui->IDC_BRIEF_B_CHOOSE)
    connect(btn, &QPushButton::clicked, this, &BriefBitmapDialog::onChoose);
  if (auto *btn = ui->IDC_MISSIONFLAGS)
    connect(btn, &QPushButton::clicked, this, &BriefBitmapDialog::onMissionFlags);

  connect(this, &QDialog::accept, this, &BriefBitmapDialog::onOk);
}

BriefBitmapDialog::~BriefBitmapDialog() { delete ui; }

void BriefBitmapDialog::onPredefChanged(int index) {
  if (index <= 0)
    return;
  int layout = -1;
  if (!PBlayouts || !PBnum_layouts)
    return;
  for (int i = 0; i < *PBnum_layouts; i++) {
    if (Briefing_screens[m_screen].layout == PBlayouts[i].filename)
      layout = i;
  }
  if (layout != -1 && index - 1 < PBlayouts[layout].num_bmps) {
    ui->IDC_BRIEF_B_X->setText(QString::number(PBlayouts[layout].bmps[index - 1].x));
    ui->IDC_BRIEF_B_Y->setText(QString::number(PBlayouts[layout].bmps[index - 1].y));
  }
}

void BriefBitmapDialog::onChoose() {
  const QString file = QFileDialog::getOpenFileName(
      this, tr("Select Bitmap File"), {},
      tr("Outrage Graphic Files (*.ogf);;32bit Uncompressed Targa (*.tga);;PCX (*.pcx);;All Files (*.*)"));
  if (file.isEmpty())
    return;
  const QString base = QFileInfo(file).fileName();
  if (!QFile::exists(file) && !QFile::exists(base)) {
    QMessageBox::warning(this, tr("Error"), tr("Not A Valid Filename"));
    return;
  }
  ui->IDC_BRIEF_B_FILENAME->setText(base);
}

void BriefBitmapDialog::onMissionFlags() {
  BriefMissionFlagsDialog dlg(m_desc.mission_mask_set, m_desc.mission_mask_unset, this);
  if (dlg.exec() == QDialog::Accepted) {
    m_desc.mission_mask_set = dlg.setFlags();
    m_desc.mission_mask_unset = dlg.unsetFlags();
  }
}

void BriefBitmapDialog::onOk() {
  const QString filename = ui->IDC_BRIEF_B_FILENAME->text();
  if (!QFile::exists(filename)) {
    QMessageBox::warning(this, tr("Error"), tr("Not A Valid Filename"));
    return;
  }

  m_desc.caps = TCBD_XY | TCBD_LOOPING | TCBD_WAITTIME | TCBD_SPEED;
  copyToFilename(m_desc.filename, MAX_FILELEN, filename.toStdString());
  m_desc.speed = ui->IDC_BRIEF_B_SPEED->text().toFloat();
  m_desc.waittime = ui->IDC_BRIEF_B_STARTTIME->text().toFloat();
  m_desc.x = ui->IDC_BRIEF_B_X->text().toInt();
  m_desc.y = ui->IDC_BRIEF_B_Y->text().toInt();

  int effectType = 0;
  for (int i = 0; i < (int)effectRadios.size(); i++) {
    if (auto *rb = findChild<QRadioButton*>(effectRadios[i]); rb && rb->isChecked()) {
      effectType = i;
      break;
    }
  }
  effectTypeToDesc(effectType, &m_desc);

  if (ui->IDC_BRIEF_B_NORENDER->isChecked())
    m_desc.flags |= TC_NOEARLYRENDER;
  else
    m_desc.flags &= ~TC_NOEARLYRENDER;

  accept();
}