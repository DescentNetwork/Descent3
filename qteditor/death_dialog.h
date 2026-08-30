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

#include "objinfo.h"
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class DeathDialog; }
QT_END_NAMESPACE


// Port of CDeathDialog (IDD_DEATH_DIALOG): edits a death_info struct (death
// flags and delay range) for an object type.
class DeathDialog : public QDialog {
  Q_OBJECT
public:
  explicit DeathDialog(death_info *info, QWidget *parent = nullptr);
  ~DeathDialog();

private slots:
  void onOk();
  //void onExplosionSmall();
  //void onExplosionMedium();
  //void onExplosionLarge();
  void onReset(void);
  void onCommonDescent();
  void onCommonQuickExplosion();
  void onCommonShootUp();
  void onCommonR2D2();
  void onCommonAnimal();
  void onCopy();
  void onPaste();

private:
  void updateDialog();

  Ui::DeathDialog *ui;
  death_info *m_info;
  death_flags_t m_flags;
  float m_delayMin;
  float m_delayMax;
};

