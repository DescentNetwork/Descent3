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
namespace Ui { class BriefButtonDialog; }
QT_END_NAMESPACE


// Port of CBriefButtonEdit (IDD_BRIEF_ADDBUTTON): edits a briefing button effect.
class BriefButtonDialog : public QDialog {
  Q_OBJECT
public:
  explicit BriefButtonDialog(TCBUTTONDESC *desc = nullptr, QWidget *parent = nullptr);
  ~BriefButtonDialog();

  TCBUTTONDESC result() const { return m_desc; }
  QString description() const { return {}; }
  QString buttonDescription() const { return {}; }

private slots:
  void onOk();
  void onChoose();
  void onChooseFocus();
  void onFlashChoose();
  void onFlashChooseFocus();
  void onMissionFlags();
  void onFlasherToggled(bool);
  void onGlowToggled(bool);
  void updateStates();

private:
  Ui::BriefButtonDialog *ui;
  TCBUTTONDESC m_desc;
};