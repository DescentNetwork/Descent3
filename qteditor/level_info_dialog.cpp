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

#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <cstring>

namespace QtEditor {

LevelInfoDialog::LevelInfoDialog(level_info *li, QWidget *parent)
    : Dialog(":/ui/level_info.ui", parent), m_levelInfo(li) {
  if (auto *edit = find<QLineEdit>("IDC_LEVEL_NAME"))
    edit->setText(li->name);
  if (auto *edit = find<QLineEdit>("IDC_DESIGNER"))
    edit->setText(li->designer);
  if (auto *edit = find<QLineEdit>("IDC_COPYRIGHT"))
    edit->setText(li->copyright);
  if (auto *edit = find<QTextEdit>("IDC_NOTES"))
    edit->setPlainText(li->notes);

  if (QPushButton *ok = find<QPushButton>("IDOK")) {
    disconnect(ok, &QPushButton::clicked, this, &QDialog::accept);
    connect(ok, &QPushButton::clicked, this, &LevelInfoDialog::onOk);
  }
}

LevelInfoDialog::~LevelInfoDialog() = default;

void LevelInfoDialog::getLevelInfo(level_info *li) {
  std::strcpy(li->name, find<QLineEdit>("IDC_LEVEL_NAME")->text().toLocal8Bit().constData());
  std::strcpy(li->designer, find<QLineEdit>("IDC_DESIGNER")->text().toLocal8Bit().constData());
  std::strcpy(li->copyright, find<QLineEdit>("IDC_COPYRIGHT")->text().toLocal8Bit().constData());
  std::strcpy(li->notes, find<QTextEdit>("IDC_NOTES")->toPlainText().toLocal8Bit().constData());
}

void LevelInfoDialog::onOk() {
  getLevelInfo(m_levelInfo);
  accept();
}

}
