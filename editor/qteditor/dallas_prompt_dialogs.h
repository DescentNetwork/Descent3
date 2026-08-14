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

#include "qteditor_dialog.h"

namespace QtEditor {

// Port of CDallasGenericPromptDlg (IDD_DALLAS_GENERIC_PROMPT_DIALOG): a
// one-line text prompt with an optional max length.
class DallasGenericPromptDialog : public Dialog {
  Q_OBJECT
public:
  explicit DallasGenericPromptDialog(QWidget *parent = nullptr);
  ~DallasGenericPromptDialog() override;

  void setDialogTitle(const QString &title);
  void setPromptText(const QString &text);
  void setPromptData(const QString &data);
  void setMaxDataLength(int length);
  QString promptData() const;
};

// Port of CDallasVectorPromptDialog (IDD_DALLAS_VECTOR_DIALOG): three float
// fields.
class DallasVectorPromptDialog : public Dialog {
  Q_OBJECT
public:
  explicit DallasVectorPromptDialog(QWidget *parent = nullptr);
  ~DallasVectorPromptDialog() override;

  void setPromptData(float v1, float v2, float v3);
  void getPromptData(float *v1, float *v2, float *v3);
};

}
