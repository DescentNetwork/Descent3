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

#pragma once

#include <QDialog>

class QLabel;

QT_BEGIN_NAMESPACE
namespace Ui { class MegacellDialog; }
QT_END_NAMESPACE


// Port of CMegacellDialog (IDD_MEGACELL): the megacell table editor
// (new/delete/lock/checkin/prev/next, import).
class MegacellDialog : public QDialog {
  Q_OBJECT
public:
  explicit MegacellDialog(QWidget *parent = nullptr);
  ~MegacellDialog();

private slots:
  void onNew();
  void onDelete();
  void onLock();
  void onCheckin();
  void onPrev();
  void onNext();

private:
  void updateDialog();
private:
  Ui::MegacellDialog *ui;
};

