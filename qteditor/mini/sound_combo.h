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


// Port of the original CSoundListCombo: fills a QComboBox with a leading
// "<none>" item (user data -1) followed by every currently-used game sound
// (user data = sound index).
void populateSoundCombo(QComboBox *combo, int selected);

// Selects the item whose user data equals selected.
void setSoundComboSelected(QComboBox *combo, int selected);

// Returns the user data of the current item (-1 for "<none>").
int soundComboSelected(QComboBox *combo);

