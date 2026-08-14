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

#include "keypad_dialog.h"

#include <QTabWidget>

namespace QtEditor {

bool Keypad::m_active = true;

Keypad::Keypad(const QString &uiResource, QWidget *parent) : Widget(uiResource, parent) {}

Keypad::~Keypad() = default;

KeypadBar::KeypadBar(QWidget *parent) : Widget(":/editor/keypad_dlgbar.ui", parent) {
  m_tabs = find<QTabWidget>("IDC_KEYPADS");
}

KeypadBar::~KeypadBar() = default;

}
