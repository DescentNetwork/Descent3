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

#include "splash_dialog.h"

#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>

namespace QtEditor {

SplashDialog *D3_splash_screen = nullptr;

SplashDialog::SplashDialog(QWidget *parent) : Dialog(":/editor/d3splash.ui", parent), m_curLine(0) {
  m_textLines[0][0] = 0;
  m_textLines[1][0] = 0;
  m_textLines[2][0] = 0;

  QLabel *image = new QLabel(handle());
  image->setObjectName("SplashImage");
  image->setAlignment(Qt::AlignCenter);
  QPixmap splash(":/editor/IDB_D3SPLASH");
  if (!splash.isNull()) {
    image->setPixmap(splash.scaled(handle()->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
  }

  m_textLabel = new QLabel(handle());
  m_textLabel->setObjectName("SplashText");
  m_textLabel->setStyleSheet("color: white; font-weight: bold;");
  m_textLabel->setAlignment(Qt::AlignCenter);
  m_textLabel->setWordWrap(true);

  QVBoxLayout *layout = new QVBoxLayout(handle());
  layout->setContentsMargins(4, 4, 4, 4);
  layout->addWidget(image, 1);
  layout->addWidget(m_textLabel);

  D3_splash_screen = this;
}

SplashDialog::~SplashDialog() {
  if (D3_splash_screen == this) {
    D3_splash_screen = nullptr;
  }
}

void SplashDialog::putText(const char *buf) {
  if (m_curLine == 3) {
    for (int i = 0; i < 2; i++) {
      memcpy(m_textLines[i], m_textLines[i + 1], sizeof(m_textLines[i]));
    }
  } else {
    m_curLine++;
  }

  snprintf(m_textLines[m_curLine - 1], sizeof(m_textLines[m_curLine - 1]), "%s", buf);

  QString text;
  for (int i = 0; i < 3; i++) {
    if (m_textLines[i][0] != 0) {
      if (!text.isEmpty()) {
        text += "\n";
      }
      text += QString::fromLatin1(m_textLines[i]);
    }
  }
  m_textLabel->setText(text);
}

}
