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

class QComboBox;

// Port of editor/ObjectListCombo.cpp: fills a QComboBox with a leading
// "<none>" item (user data OBJECT_HANDLE_NONE) followed by all named
// objects of the given type (or all types if type == OBJ_NONE).
void populateObjectCombo(QComboBox *combo, int type, int selected_handle);
void setObjectComboSelected(QComboBox *combo, int selected_handle);
int objectComboSelected(QComboBox *combo);
