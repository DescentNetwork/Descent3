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
#include <string>

#include "brief_mission_flags_dialog.h"

namespace {

int effectTypeFromDesc(const TCBMPDESC &desc) {
  switch (desc.type) {
  case TC_BMP_STATIC:
    return 0;
  case TC_BMP_BLUR:
    return desc.mode == tc_bmp_mode::in ? 1 : 2;
  case TC_BMP_SCANLINE:
    return desc.mode == tc_bmp_mode::in ? 3 : 4;
  case TC_BMP_INVERT:
    return desc.mode == tc_bmp_mode::in ? 5 : 6;
  case TC_BMP_STRETCH:
    return desc.mode == tc_bmp_mode::in ? 7 : 8;
  default:
    return 0;
  }
}

void effectTypeToDesc(int effectType, TCBMPDESC *desc) {
  switch (effectType) {
  case 0:
    desc->type = TC_BMP_STATIC;
    desc->mode = tc_bmp_mode::in;
    break;
  case 1:
    desc->type = TC_BMP_BLUR;
    desc->mode = tc_bmp_mode::in;
    break;
  case 2:
    desc->type = TC_BMP_BLUR;
    desc->mode = tc_bmp_mode::out;
    break;
  case 3:
    desc->type = TC_BMP_SCANLINE;
    desc->mode = tc_bmp_mode::in;
    break;
  case 4:
    desc->type = TC_BMP_SCANLINE;
    desc->mode = tc_bmp_mode::out;
    break;
  case 5:
    desc->type = TC_BMP_INVERT;
    desc->mode = tc_bmp_mode::in;
    break;
  case 6:
    desc->type = TC_BMP_INVERT;
    desc->mode = tc_bmp_mode::out;
    break;
  case 7:
    desc->type = TC_BMP_STRETCH;
    desc->mode = tc_bmp_mode::in;
    break;
  case 8:
    desc->type = TC_BMP_STRETCH;
    desc->mode = tc_bmp_mode::out;
    break;
  }
}

} // namespace

BriefBitmapDialog::BriefBitmapDialog(TCBMPDESC *desc, QWidget *parent)
    : QDialog(parent), ui(new Ui::BriefBitmapDialog), m_effectType(0), m_screen(0)
{
  ui->setupUi(this);
  m_desc = TCBMPDESC{};
  m_desc.type = TC_BMP_STATIC;

  if (desc) {
    m_desc.caps = desc->caps;
    if (desc->caps.xy) {
      m_desc.x = desc->x;
      m_desc.y = desc->y;
    }
    if (desc->caps.looping)
      m_desc.looping = desc->looping;
    if (desc->caps.waittime)
      m_desc.waittime = desc->waittime;
    if (desc->caps.speed)
      m_desc.speed = desc->speed;
    m_desc.type = desc->type;
    m_desc.mode = desc->mode;
    m_desc.no_early_render = desc->no_early_render;
    m_desc.filename = desc->filename;
    m_desc.mission_mask_set = desc->mission_mask_set;
    m_desc.mission_mask_unset = desc->mission_mask_unset;
  }
  m_effectType = effectTypeFromDesc(m_desc);

  ui->IDC_BRIEF_B_FILENAME->setText(QString::fromStdString(m_desc.filename));
  ui->IDC_BRIEF_B_SPEED->setText(QString::number(m_desc.speed));
  ui->IDC_BRIEF_B_STARTTIME->setText(QString::number(m_desc.waittime));
  ui->IDC_BRIEF_B_X->setText(QString::number(m_desc.x));
  ui->IDC_BRIEF_B_Y->setText(QString::number(m_desc.y));
  ui->IDC_BRIEF_B_NORENDER->setChecked(m_desc.no_early_render);

  ui->IDC_BRIEF_B_STATIC->setChecked(m_effectType == 0);
  ui->IDC_BRIEF_B_BLURIN->setChecked(m_effectType == 1);
  ui->IDC_BRIEF_B_BLUROUT->setChecked(m_effectType == 2);
  ui->IDC_BRIEF_B_SCANIN->setChecked(m_effectType == 3);
  ui->IDC_BRIEF_B_SCANOUT->setChecked(m_effectType == 4);
  ui->IDC_BRIEF_B_INVIN->setChecked(m_effectType == 5);
  ui->IDC_BRIEF_B_INVOUT->setChecked(m_effectType == 6);
  ui->IDC_BRIEF_B_STRETCHIN->setChecked(m_effectType == 7);
  ui->IDC_BRIEF_B_STRETCHOUT->setChecked(m_effectType == 8);

  if (auto *combo = ui->IDC_BRIEF_B_PREDEF) {
    combo->clear();
    combo->addItem("<Raw>");
    int layout = -1;
    if (!PBlayouts.empty()) {
      for (size_t i = 0; i < PBlayouts.size(); i++) {
        if (Briefing_screens[m_screen].layout == PBlayouts[i].filename)
          layout = (int)i;
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

  connect(this, &QDialog::accepted, this, &BriefBitmapDialog::onOk);
}

BriefBitmapDialog::~BriefBitmapDialog() { delete ui; }

void BriefBitmapDialog::onPredefChanged(int index) {
  if (index <= 0)
    return;
  int layout = -1;
  if (PBlayouts.empty())
    return;
  for (size_t i = 0; i < PBlayouts.size(); i++) {
    if (Briefing_screens[m_screen].layout == PBlayouts[i].filename)
      layout = (int)i;
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

  m_desc.caps.xy = true;
  m_desc.caps.looping = true;
  m_desc.caps.waittime = true;
  m_desc.caps.speed = true;
  m_desc.filename = filename.toStdString();
  m_desc.speed = ui->IDC_BRIEF_B_SPEED->text().toFloat();
  m_desc.waittime = ui->IDC_BRIEF_B_STARTTIME->text().toFloat();
  m_desc.x = ui->IDC_BRIEF_B_X->text().toInt();
  m_desc.y = ui->IDC_BRIEF_B_Y->text().toInt();

  int effectType = 0;
  if (ui->IDC_BRIEF_B_STATIC->isChecked())
    effectType = 0;
  else if (ui->IDC_BRIEF_B_BLURIN->isChecked())
    effectType = 1;
  else if (ui->IDC_BRIEF_B_BLUROUT->isChecked())
    effectType = 2;
  else if (ui->IDC_BRIEF_B_SCANIN->isChecked())
    effectType = 3;
  else if (ui->IDC_BRIEF_B_SCANOUT->isChecked())
    effectType = 4;
  else if (ui->IDC_BRIEF_B_INVIN->isChecked())
    effectType = 5;
  else if (ui->IDC_BRIEF_B_INVOUT->isChecked())
    effectType = 6;
  else if (ui->IDC_BRIEF_B_STRETCHIN->isChecked())
    effectType = 7;
  else if (ui->IDC_BRIEF_B_STRETCHOUT->isChecked())
    effectType = 8;
  effectTypeToDesc(effectType, &m_desc);

  if (ui->IDC_BRIEF_B_NORENDER->isChecked())
    m_desc.no_early_render = true;
  else
    m_desc.no_early_render = false;

  accept();
}
