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

#include "bnode.h"

#define EBDRAW_NONE 0
#define EBDRAW_ROOM 1
#define EBDRAW_ROOM_AND_NEXT_ROOMS 2
#define EBDRAW_LEVEL 3

extern char EBN_draw_type;

bool EBNode_VerifyGraph();
void EBNode_ClearLevel();
void EBNode_MakeDefaultIntraRoomNodes(int roomnum);
void EBNode_MakeDefaultInterRoomEdges(int roomnum);
void EBNode_MakeFirstPass();
void EBNode_Move(bool f_offset, int roomnum, int pnt, vector *pos);
int EBNode_AddNode(int roomnum, vector *pnt, bool f_from_editor, bool f_check_for_close_nodes);
void EBNode_AddEdge(int spnt, int sroom, int epnt, int eroom, bool f_make_reverse = true,
                    float computed_max_rad = -1.0f);
void EBNode_RemoveEdge(int spnt, int sroom, int epnt, int eroom, bool f_remove_reverse = true);
int EBNode_InsertNodeOnEdge(int spnt, int sroom, int epnt, int eroom);
void EBNode_RemoveNode(int roomnum, int pnt);
void EBNode_AutoEdgeNode(int spnt, int sroom);
