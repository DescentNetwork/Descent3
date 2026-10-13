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

#ifndef WEAPON_EXTERNAL_H_
#define WEAPON_EXTERNAL_H_

#include <cstdint>

// Player weapon ids (also index Weapon_info for these weapons)
enum class weapon_index : uint8_t {
  laser = 0,
  vauss = 1,
  microwave = 2,
  plasma = 3,
  fusion = 4,
  super_laser = 5,
  massdriver = 6,
  napalm = 7,
  emd = 8,
  omega = 9,
  concussion = 10,
  homing = 11,
  impactmortar = 12,
  smart = 13,
  mega = 14,
  frag = 15,
  guided = 16,
  napalmrocket = 17,
  cyclone = 18,
  blackshark = 19,
  flare = 20,
};

#endif
