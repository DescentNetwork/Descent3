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

#include "TelComEfxStructs.h"
#include "brief_model.h"
#include <QDialog>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class BriefTextDialog; }
QT_END_NAMESPACE

class QPlainTextEdit;


// Port of CBriefTextEdit (IDD_BRIEF_ADDTEXT): edits a briefing text effect.
class BriefTextEditDialog : public QDialog {
  Q_OBJECT
public:
  explicit BriefTextEditDialog(int currScreen, TCTEXTDESC *desc = nullptr,
                               const std::string &text = {}, int id = 0,
                               QWidget *parent = nullptr);
  ~BriefTextEditDialog();

  TCTEXTDESC result() const { return m_desc; }
  QString descText() const { return m_text; }
  int id() const { return m_id; }
  std::string textBuffer() const { return m_text.toStdString(); }

private slots:
  void onOk();
  void onColorPal();
  void onPredefChanged(int index);
  void onMissionFlags();

private:
  Ui::BriefTextDialog *ui;

  void populatePredefs();
  int m_screen;
  TCTEXTDESC m_desc;
  QString m_text;
  int m_id;
  int m_effectType;
  QPlainTextEdit *m_richEdit;
};

