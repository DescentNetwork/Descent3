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

#ifndef GAMEEVENT_H
#define GAMEEVENT_H

#include <cstdint>
#include "game.h"
#include "object.h"

#define MAX_EVENTS 500

// Game event types
enum class game_event_type : uint8_t {
  object = 1,
  render = 2,
};

// IDs
enum class game_event_id : uint16_t {
  unknown = 0,
  fusion_effect = 1,
  damage_effect = 2,
  screen_blend = 3,
  blast_ring_event = 4,
  fov_change_event = 5,
  edrain_effect = 6,
  timed = 256,
};

struct game_event {
  game_event_type type;
  game_event_id id;
  int objhandle_detonator; // watch this object, if it dies/gets killed than cancel this game event
  uint8_t used;
  float start_time, end_time;
  int frame_born;

  void *data;

  void (*subfunction)(int, void *);
};

// Adds and event to the list. The event will trigger at Gametime+length
int CreateNewEvent(int type, int id, float length, void *data, int size, void (*subfunction)(int eventnum, void *data),
                   int objhandle_detonator = OBJECT_HANDLE_NONE);

// Processes all pending events, removing the ones that are expired
void ProcessNormalEvents();
void ProcessRenderEvents();

// Does a specific action according to an event
void HandleEvent(game_event *);

// Clears the event list
void ClearAllEvents();

// Returns -1 if can't find event, else index of event int GameEvent array
int FindEventID(int id);

// Frees an event for use by others
void FreeEvent(int index);



#endif
