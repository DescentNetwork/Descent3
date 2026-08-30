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

#include "brief_movie_dialog.h"
#include "ui_brief_addmovie.h"

#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include <cstring>
#include <string>

#include "brief_mission_flags_dialog.h"

namespace {

void copyToFilename(char *dst, size_t dstLen, const std::string &src) {
  std::strncpy(dst, src.c_str(), dstLen - 1);
  dst[dstLen - 1] = '\0';
}

} // namespace

BriefMovieDialog::BriefMovieDialog(TCMOVIEDESC *desc, QWidget *parent)
    : QDialog(parent), ui(new Ui::BriefMovieDialog)
{
  ui->setupUi(this);
  std::memset(&m_desc, 0, sizeof(TCMOVIEDESC));
  m_desc.fps = 20.0f;

  if (desc) {
    m_desc.caps = desc->caps;
    if (desc->caps & TCMD_XY) {
      m_desc.x = desc->x;
      m_desc.y = desc->y;
    }
    if (desc->caps & TCMD_LOOPING)
      m_desc.looping = desc->looping;
    if (desc->caps & TCMD_WAITTIME)
      m_desc.waittime = desc->waittime;
    if (desc->caps & TCMD_FPS)
      m_desc.fps = desc->fps;
    m_desc.mission_mask_set = desc->mission_mask_set;
    m_desc.mission_mask_unset = desc->mission_mask_unset;
    copyToFilename(m_desc.filename, MAX_FILELEN, desc->filename);
  }

  ui->IDC_BRIEF_M_FILENAME->setText(QString::fromStdString(m_desc.filename));
  ui->IDC_BRIEF_M_FPS->setText(QString::number(m_desc.fps));
  ui->IDC_BRIEF_M_LOOPING->setChecked(m_desc.looping);
  ui->IDC_BRIEF_M_STARTTIME->setText(QString::number(m_desc.waittime));
  ui->IDC_BRIEF_M_X->setText(QString::number(m_desc.x));
  ui->IDC_BRIEF_M_Y->setText(QString::number(m_desc.y));

  if (auto *btn = ui->IDC_BRIEF_M_CHOOSE)
    connect(btn, &QPushButton::clicked, this, &BriefMovieDialog::onChoose);
  if (auto *btn = ui->IDC_MISSIONFLAGS)
    connect(btn, &QPushButton::clicked, this, &BriefMovieDialog::onMissionFlags);

  connect(this, &QDialog::accept, this, &BriefMovieDialog::onOk);
}

BriefMovieDialog::~BriefMovieDialog() { delete ui; }

void BriefMovieDialog::onChoose() {
  const QString file = QFileDialog::getOpenFileName(this, tr("Select Movie File"), {},
                                                    tr("Outrage Movie Files (*.mve)"));
  if (file.isEmpty())
    return;
  const QString base = QFileInfo(file).fileName();
  if (!QFile::exists(file) && !QFile::exists(base)) {
    QMessageBox::warning(this, tr("Error"), tr("Invalid Filename"));
    return;
  }
  ui->IDC_BRIEF_M_FILENAME->setText(base);
}

void BriefMovieDialog::onMissionFlags() {
  BriefMissionFlagsDialog dlg(m_desc.mission_mask_set, m_desc.mission_mask_unset, this);
  if (dlg.exec() == QDialog::Accepted) {
    m_desc.mission_mask_set = dlg.setFlags();
    m_desc.mission_mask_unset = dlg.unsetFlags();
  }
}

void BriefMovieDialog::onOk() {
  const QString filename = ui->IDC_BRIEF_M_FILENAME->text();
  if (!QFile::exists(filename)) {
    QMessageBox::warning(this, tr("Error"), tr("Invalid Filename"));
    return;
  }

  m_desc.caps = TCMD_XY | TCMD_LOOPING | TCMD_WAITTIME | TCMD_FPS;
  copyToFilename(m_desc.filename, MAX_FILELEN, filename.toStdString());
  m_desc.fps = ui->IDC_BRIEF_M_FPS->text().toFloat();
  m_desc.looping = ui->IDC_BRIEF_M_LOOPING->isChecked();
  m_desc.waittime = ui->IDC_BRIEF_M_STARTTIME->text().toFloat();
  m_desc.x = ui->IDC_BRIEF_M_X->text().toInt();
  m_desc.y = ui->IDC_BRIEF_M_Y->text().toInt();
  accept();
}