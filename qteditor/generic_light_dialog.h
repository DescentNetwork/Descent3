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

#include "object_external_struct.h"
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class GenericLightDialog; }
QT_END_NAMESPACE


// Port of CGenericLightDialog (IDD_GENERICLIGHT): object lighting settings
// (colors, distance, pulse/flicker/timebits, render type).
class GenericLightDialog : public QDialog {
  Q_OBJECT
public:
  explicit GenericLightDialog(light_info *lightinfo, QWidget *parent = nullptr);
  ~GenericLightDialog();

private slots:
  void onFlickerRadio();
  void onAlwaysOnRadio();
  void onUseTimebitsRadio();
  void onPulseToggled(bool checked);
  void onPulseToSecondToggled(bool checked);
  void onFlickerSlightlyToggled(bool checked);
  void onDirectionalToggled(bool checked);
  void onNoSpecularToggled(bool checked);
  void onTimeCheck();
  void onRenderStatic();
  void onRenderGouraud();
  void onRenderLightmaps();
  void onFieldEdited();

private:
  void updateDialog();
  void setFlag(int32_t flag, const char *checkName, bool checked);

  Ui::GenericLightDialog *ui;
  light_info *m_lightinfo;
};

