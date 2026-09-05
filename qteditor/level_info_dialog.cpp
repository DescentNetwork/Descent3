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

#include "level_info_dialog.h"
#include "ui_level_info.h"

#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <cstring>


LevelInfoDialog::LevelInfoDialog(level_info *li, QWidget *parent)
    : QDialog(parent), ui(new Ui::LevelInfoDialog), m_levelInfo(li)
{
  ui->setupUi(this);
  if (auto *edit = ui->IDC_LEVEL_NAME)
    edit->setText(QString::fromStdString(li->name));
  if (auto *edit = ui->IDC_DESIGNER)
    edit->setText(QString::fromStdString(li->designer));
  if (auto *edit = ui->IDC_COPYRIGHT)
    edit->setText(QString::fromStdString(li->copyright));
  if (auto *edit = ui->IDC_NOTES)
    edit->setPlainText(QString::fromStdString(li->notes));

  connect(this, &QDialog::accept, this, &LevelInfoDialog::onOk);
}

LevelInfoDialog::~LevelInfoDialog() { delete ui; }

void LevelInfoDialog::getLevelInfo(level_info *li) {
  li->name = ui->IDC_LEVEL_NAME->text().toStdString();
  li->designer = ui->IDC_DESIGNER->text().toStdString();
  li->copyright = ui->IDC_COPYRIGHT->text().toStdString();
  li->notes = ui->IDC_NOTES->toPlainText().toStdString();
}

void LevelInfoDialog::onOk() {
  getLevelInfo(m_levelInfo);
  accept();
}

