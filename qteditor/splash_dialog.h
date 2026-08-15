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

#include <QLabel>

#include "qteditor_dialog.h"

namespace QtEditor {

// Modeless splash dialog shown during editor startup. The splash bitmap comes
// from the compiled-in resource (IDB_D3SPLASH); up to three status lines are
// printed at the bottom of the image.
class SplashDialog : public Dialog {
  Q_OBJECT
public:
  explicit SplashDialog(QWidget *parent = nullptr);
  ~SplashDialog() override;

  void putText(const char *buf);

private:
  QLabel *m_textLabel;
  char m_textLines[3][256];
  int m_curLine;
};

extern SplashDialog *D3_splash_screen;

}
