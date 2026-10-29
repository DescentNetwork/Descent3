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

#ifndef EBNODE_H
#define EBNODE_H

#include "pstypes.h"
#include "bnode.h"
#include "d3edit.h"

extern char EBN_draw_type;

void EBNode_MakeDefaultIntraRoomNodes(index_t roomnum);
void EBNode_MakeDefaultInterRoomEdges(index_t roomnum);
void EBNode_MakeFirstPass(void);
void EBNode_Draw(char draw_type, grViewport *vp, vector3 *viewer_eye, matrix *viewer_orient, float zoom);
void EBNode_Move(bool f_offset, index_t roomnum, index_t pnt, const vector3 &pos);
index_t EBNode_AddNode(index_t roomnum, const vector3 &pnt, bool f_from_editor, bool f_check_for_close_nodes);
// Note the 2 last parameters are for internal use ONLY never set them
void EBNode_AddEdge(index_t spnt, index_t sroom, index_t epnt, index_t eroom, bool f_make_reverse = true,
                    float computed_max_rad = -1.0f);
void EBNode_RemoveEdge(index_t spnt, index_t sroom, index_t epnt, index_t eroom, bool f_remove_reverse = true);
index_t EBNode_InsertNodeOnEdge(index_t spnt, index_t sroom, index_t epnt, index_t eroom);
void EBNode_RemoveNode(index_t roomnum, index_t pnt);
void EBNode_ClearLevel();
bool EBNode_VerifyGraph();
void EBNode_AutoEdgeNode(index_t spnt, index_t sroom);

#endif
