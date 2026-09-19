/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
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

#ifndef ROBOTFIRESTRUCT_EXTERNAL_H_
#define ROBOTFIRESTRUCT_EXTERNAL_H_

#define MAX_WB_GUNPOINTS 8    // Maximum gun points for a gun battery   // Limitted by interface in Editor
#define MAX_WB_FIRING_MASKS 8 // Maximum firing masks (firing sequence) // Limitted by interface in Editor
#define MAX_WB_TURRETS 8      // Maximum number of turret// Arbitary
#define MAX_WB_UPGRADES 5

#define DWBF_ENABLED 1
#define DWBF_AUTOMATIC 2
#define DWBF_ANIMATING 4
#define DWBF_ANIM_FIRED 8
#define DWBF_QUAD 16
#define DWBF_UPGRADED 32

#endif
