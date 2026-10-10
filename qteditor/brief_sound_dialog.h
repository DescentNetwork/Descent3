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

QT_BEGIN_NAMESPACE
namespace Ui { class BriefSoundDialog; }
QT_END_NAMESPACE


// Port of CBriefSoundEdit (IDD_BRIEF_ADDSOUND): edits a briefing sound effect.
class BriefSoundDialog : public QDialog {
  Q_OBJECT
public:
  explicit BriefSoundDialog(TCSNDDESC *desc = nullptr, QWidget *parent = nullptr);
  ~BriefSoundDialog();

  TCSNDDESC result() const { return m_desc; }
  QString description() const { return {}; }

private slots:
  void onOk();
  void onChoose();
  void onMissionFlags();

private:
  Ui::BriefSoundDialog *ui;
  TCSNDDESC m_desc;
};