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

// Port of editor/RoomListCombo.cpp: fills a QComboBox with a leading
// "<none>" item (user data -1) followed by all named rooms.
void populateRoomCombo(QComboBox *combo, int selected);
void setRoomComboSelected(QComboBox *combo, int selected);
int roomComboSelected(QComboBox *combo);
