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

#include "status_dialog.h"
#include "ui_statusdlg.h"

#include <QLabel>
#include <QProgressBar>


StatusDialog::StatusDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::StatusDialog) {
  ui->setupUi(this);
}

StatusDialog::~StatusDialog() { delete ui; }

int StatusDialog::step() {
  QProgressBar *progress = ui->IDC_STATUSPROGRESS;
  if (!progress)
    return 0;
  const int value = progress->value();
  progress->setValue(value + m_step);
  return value;
}

void StatusDialog::init(int min, int max, int delta) {
  if (QProgressBar *progress = ui->IDC_STATUSPROGRESS) {
    progress->setRange(min, max);
    progress->setValue(min);
    m_step = delta;
  }
}

void StatusDialog::text(const QString &string) {
  if (auto *label = ui->IDC_STATUSTEXT)
    label->setText(string);
}

void StatusDialog::setTo(int value) {
  if (QProgressBar *progress = ui->IDC_STATUSPROGRESS)
    progress->setValue(value);
}

Progress::~Progress() { destroyProgress(); }

bool Progress::initProgress(fix min, fix max, int32_t iterations, QWidget *parent) {
  int nmin = FixToInt(min);
  int nmax = FixToInt(max);
  if (iterations == 0)
    return false;

  float delta = (float)((float)(nmax - nmin)) / (float)iterations;
  while (delta < 1.0f) {
    delta *= 10.0f;
    nmax *= 10;
  }
  int step = (int)delta;
  m_Max = nmax;
  m_Min = nmin;

  m_statusDlg = new StatusDialog(parent);
  m_statusDlg->show();
  m_statusDlg->init(nmin, nmax, step);
  return true;
}

bool Progress::initProgress(QWidget *parent) {
  m_Max = 100;
  m_Min = 0;
  m_statusDlg = new StatusDialog(parent);
  m_statusDlg->show();
  m_statusDlg->init(0, 100, 1);
  return true;
}

void Progress::destroyProgress() {
  if (m_statusDlg) {
    delete m_statusDlg;
    m_statusDlg = nullptr;
  }
}

void Progress::setProgressText(const QString &string) {
  if (m_statusDlg)
    m_statusDlg->text(string);
}

bool Progress::increaseProgress() {
  if (!m_statusDlg)
    return true;
  const int ret = m_statusDlg->step();
  return ret >= m_Max;
}

void Progress::setProgressPercentage(int percent) {
  if (!m_statusDlg)
    return;
  const float per = (float)percent / 100.0f;
  m_statusDlg->setTo(m_Min + (int)(per * (m_Max - m_Min)));
}

void Progress::setProgressPercentage(float percent) {
  if (!m_statusDlg)
    return;
  m_statusDlg->setTo(m_Min + (int)(percent * (m_Max - m_Min)));
}

