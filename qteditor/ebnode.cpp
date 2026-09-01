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

// Qt port of editor/ebnode.cpp — BOA Helper Node graph management.
// Drawing functions (EBNode_Draw, EBNode_DrawRoom) are not ported here;
// BNode rendering is handled by EditorView::renderBNodes() in OpenGL.

#include <QtGlobal>
#include <QMessageBox>

#include <limits.h>
#include "logger/log.h"

#include "bnode.h"
#include "room.h"
#include "mem/mem.h"

#include "vecmat.h"
#include "object.h"
#include "ebnode.h"
#include "findintersection.h"

#include "terrain.h"
#include "BOA.h"
#include "AIMain.h"
#include "d3edit.h"

char EBN_draw_type = EBDRAW_NONE;
#define EBN_MAX_NEXT_ROOMS 200
#define BNODE_VERY_CLOSE_DIST 5.0f

bool EBNode_VerifyGraph() {
  bool f_verified = true;

  if (!BNode_allocated) {
    LOG_INFO("EBNode Verify: No BNodes for this level\n");
    return false;
  }

  MakeBOA();

  for (int i = Highest_room_index + 1; i <= Highest_room_index + BOA_num_terrain_regions; i++) {
    bn_list *nlist = BNode_GetBNListPtr(i);
    int cur_region = i - Highest_room_index - 1;

    for (int j = nlist->num_nodes - 1; j >= 0; j--) {
      int cell = GetTerrainRoomFromPos(&nlist->nodes[j].pos);
      if (cur_region != TERRAIN_REGION(cell)) {
        for (int k = 0; k < nlist->nodes[j].num_edges; k++) {
          if (BOA_INDEX(nlist->nodes[j].edges[k].end_room) >= 0 &&
              BOA_INDEX(nlist->nodes[j].edges[k].end_room) <= Highest_room_index) {
            int r = nlist->nodes[j].edges[k].end_room;
            int p = nlist->nodes[j].edges[k].end_index;

            for (int x = 0; x < Rooms[r].num_portals; x++) {
              if (Rooms[r].portals[x].bnode_index == p) {
                int cr = Rooms[r].portals[x].croom;
                int cp = Rooms[r].portals[x].cportal;
                Rooms[cr].portals[cp].bnode_index = -1;
              }
            }
          }
        }
        EBNode_RemoveNode(i, j);
      }
    }
  }

  for (int i = 0; i <= Highest_room_index + BOA_num_terrain_regions; i++) {
    bn_list *nlist;

    if (i >= 0 && i <= Highest_room_index && !Rooms[i].used)
      continue;
    if (i <= Highest_room_index && Rooms[i].flags.external)
      continue;

    nlist = BNode_GetBNListPtr(i);

    for (int j = 0; j < nlist->num_nodes; j++) {
      for (int k = 0; k < nlist->nodes[j].num_edges; k++) {
        if (nlist->nodes[j].edges[k].max_rad < 5.0f) {
          LOG_INFO("EBNode Verify: Removed a skinny edge.\n");
          EBNode_RemoveEdge(j, i, nlist->nodes[j].edges[k].end_index, nlist->nodes[j].edges[k].end_room);
          k--;
        }
      }
    }
  }

  for (int i = 0; i <= Highest_room_index; i++) {
    if (Rooms[i].used) {
      room *rp = &Rooms[i];
      if (Rooms[i].flags.external)
        continue;

      for (int j = 0; j < Rooms[i].num_portals; j++) {
        if (Rooms[i].portals[j].bnode_index >= 0 && Rooms[i].portals[j].bnode_index >= Rooms[i].bn_info.num_nodes) {
          LOG_INFO("EBNode: Bashed an invalid node\n");
          Rooms[i].portals[j].bnode_index = -1;
        } else if (Rooms[i].portals[j].bnode_index < 0) {
          bool f_add = true;

          if (!((rp->portals[j].flags & PF_BLOCK) && !(rp->portals[j].flags & PF_BLOCK_REMOVABLE))) {
            f_add = false;
          }

          if ((rp->portals[j].flags & PF_RENDER_FACES) && !(rp->portals[j].flags & PF_RENDERED_FLYTHROUGH)) {
            if (!(GameTextures[rp->faces[rp->portals[j].portal_face].tmap].flags.breakable ||
              GameTextures[rp->faces[rp->portals[j].portal_face].tmap].flags.forcefield)) {
              f_add = false;
            }
          }

          if (rp->portals[j].flags & PF_TOO_SMALL_FOR_ROBOT) {
            f_add = false;
          }

          if (f_add) {
            vector3 pos;
            pos = rp->portals[j].path_pnt + rp->faces[rp->portals[j].portal_face].normal * 0.75f;
            rp->portals[j].bnode_index = EBNode_AddNode(i, &pos, false, false);
            LOG_INFO("EBNode Verify: Added a portal node\n");
          }
        }
      }
    }
  }

  for (int region = 0; region < BOA_num_terrain_regions; region++) {
    for (int i = 0; i < BOA_num_connect[region]; i++) {
      int end_room = BOA_connect[region][i].roomnum;
      room *rp = &Rooms[end_room];
      int p = BOA_connect[region][i].portal;

      vector3 pos;
      pos = rp->portals[p].path_pnt - rp->faces[rp->portals[p].portal_face].normal * 0.75f;

      int external_room = rp->portals[p].croom;
      int external_portal = rp->portals[p].cportal;
      Q_ASSERT(Rooms[external_room].flags.external);

      if (Rooms[external_room].portals[external_portal].bnode_index < 0) {
        Rooms[external_room].portals[external_portal].bnode_index =
            EBNode_AddNode(Highest_room_index + region + 1, &pos, false, false);
        if (Rooms[end_room].portals[p].bnode_index >= 0)
          EBNode_AddEdge(Rooms[external_room].portals[external_portal].bnode_index, Highest_room_index + region + 1,
                         Rooms[end_room].portals[p].bnode_index, end_room);
      }
    }
  }

  for (int i = 0; i <= Highest_room_index; i++) {
    if (Rooms[i].used) {
      room *rp = &Rooms[i];
      if (Rooms[i].flags.external)
        continue;

      for (int j = 0; j < Rooms[i].num_portals; j++) {
        if ((Rooms[i].portals[j].flags & PF_BLOCK) && !(Rooms[i].portals[j].flags & PF_BLOCK_REMOVABLE)) {
          if (Rooms[i].portals[j].bnode_index >= 0) {
            LOG_INFO("EBNode Verify: Removed a node.\n");
            EBNode_RemoveNode(i, Rooms[i].portals[j].bnode_index);
          }
          continue;
        }

        if ((Rooms[i].portals[j].flags & PF_RENDER_FACES) && !(Rooms[i].portals[j].flags & PF_RENDERED_FLYTHROUGH)) {
          if (!(GameTextures[Rooms[i].faces[Rooms[i].portals[j].portal_face].tmap].flags.breakable ||
                GameTextures[Rooms[i].faces[Rooms[i].portals[j].portal_face].tmap].flags.forcefield)) {
            if (Rooms[i].portals[j].bnode_index >= 0) {
              LOG_INFO("EBNode Verify: Removed a node.\n");
              EBNode_RemoveNode(i, Rooms[i].portals[j].bnode_index);
            }
            continue;
          }
        }

        if (Rooms[i].portals[j].flags & PF_TOO_SMALL_FOR_ROBOT) {
          if (Rooms[i].portals[j].bnode_index >= 0) {
            LOG_INFO("EBNode Verify: Removed a node.\n");
            EBNode_RemoveNode(i, Rooms[i].portals[j].bnode_index);
          }
          continue;
        }

        if (Rooms[i].portals[j].bnode_index < 0) {
          if (rp->flags.external) {
            int cr = rp->portals[j].croom;
            int ci = Rooms[cr].portals[rp->portals[j].cportal].bnode_index;

            if (Rooms[cr].flags.external)
              continue;

            vector3 pos;
            pos = rp->portals[j].path_pnt + rp->faces[rp->portals[j].portal_face].normal * 0.75f;
            int roomnum = BOA_INDEX(GetTerrainRoomFromPos(&pos));

            int xxx;
            for (xxx = 0; xxx < BOA_num_connect[TERRAIN_REGION(roomnum)]; xxx++) {
              if (BOA_connect[TERRAIN_REGION(roomnum)][xxx].roomnum == cr &&
                  BOA_connect[TERRAIN_REGION(roomnum)][xxx].portal == rp->portals[j].cportal) {
                break;
              }
            }

            if (xxx >= BOA_num_connect[TERRAIN_REGION(roomnum)]) {
              LOG_INFO("EBNode Verify:  External room isn't in terrain region list\n");
              f_verified = false;
              continue;
            }

            rp->portals[j].bnode_index = EBNode_AddNode(roomnum, &pos, false, false);
            Q_ASSERT(rp->portals[j].bnode_index >= 0);
            EBNode_AutoEdgeNode(rp->portals[j].bnode_index, roomnum);
            LOG_INFO("EBNode Verify: Added a node and autoedged it.\n");

            if (ci >= 0) {
              EBNode_AddEdge(rp->portals[j].bnode_index, roomnum, ci, cr);
            }
          } else {
            vector3 pos;
            pos = rp->portals[j].path_pnt + rp->faces[rp->portals[j].portal_face].normal * 0.75f;
            rp->portals[j].bnode_index = EBNode_AddNode(i, &pos, false, false);
            Q_ASSERT(rp->portals[j].bnode_index >= 0);
            EBNode_AutoEdgeNode(rp->portals[j].bnode_index, i);
            LOG_INFO("EBNode Verify: Added a node and autoedged it.\n");

            int cr = rp->portals[j].croom;
            int ci = Rooms[cr].portals[rp->portals[j].cportal].bnode_index;

            if (ci < 0)
              continue;

            if (Rooms[cr].flags.external) {
              vector3 pos;
              pos = rp->portals[j].path_pnt - rp->faces[rp->portals[j].portal_face].normal * 0.75f;
              int roomnum = BOA_INDEX(GetTerrainRoomFromPos(&pos));
              EBNode_AddEdge(rp->portals[j].bnode_index, i, ci, roomnum);
            } else {
              if (ci >= 0)
                EBNode_AddEdge(rp->portals[j].bnode_index, i, ci, cr);
            }
          }
        }
      }
    }
  }

  for (int i = 0; i <= Highest_room_index + BOA_num_terrain_regions; i++) {
    bn_list *nlist;

    if (i >= 0 && i <= Highest_room_index && !Rooms[i].used)
      continue;
    if (i >= 0 && i <= Highest_room_index && Rooms[i].flags.external)
      continue;

    nlist = BNode_GetBNListPtr(i);

    for (int j = 0; j < nlist->num_nodes; j++) {
      for (int k = 0; k < nlist->nodes[j].num_edges; k++) {
        if (nlist->nodes[j].edges[k].max_rad < 5.0f) {
          LOG_INFO("EBNode Verify: Removed a skinny edge.\n");
          EBNode_RemoveEdge(j, i, nlist->nodes[j].edges[k].end_index, nlist->nodes[j].edges[k].end_room);
          k--;
        }
      }

      for (int k = 0; k < nlist->nodes[j].num_edges; k++) {
        if (nlist->nodes[j].edges[k].end_room <= Highest_room_index &&
            !Rooms[nlist->nodes[j].edges[k].end_room].used) {
          LOG_INFO("EBNode Verify: Removed a edge to a non-existant room. Room %d, node %d, edge %d\n", i, j, k);
          EBNode_RemoveEdge(j, i, nlist->nodes[j].edges[k].end_index, nlist->nodes[j].edges[k].end_room);
          k--;
        }
      }
    }
  }

  for (int i = 0; i <= Highest_room_index + BOA_num_terrain_regions; i++) {
    bn_list *nlist;

    if (i >= 0 && i <= Highest_room_index && !Rooms[i].used)
      continue;
    if (i >= 0 && i <= Highest_room_index && Rooms[i].flags.external)
      continue;

    nlist = BNode_GetBNListPtr(i);

    for (int j = 0; j < nlist->num_nodes; j++) {
      for (int k = 0; k < nlist->nodes[j].num_edges; k++) {
        if (nlist->nodes[j].edges[k].max_rad < 5.0f) {
          LOG_INFO("Skinny Edge - from r%d n%d to r%d n%d\n", i, j, nlist->nodes[j].edges[k].end_room,
                  nlist->nodes[j].edges[k].end_index);
          f_verified = false;
        }
      }
    }
  }

  for (int i = 0; i <= Highest_room_index + BOA_num_terrain_regions; i++) {
    if (i >= 0 && i <= Highest_room_index && !Rooms[i].used)
      continue;
    if (i >= 0 && i <= Highest_room_index && Rooms[i].flags.external)
      continue;

    bn_list *nlist = BNode_GetBNListPtr(i);
    Q_ASSERT(nlist);

    for (int j = 0; j < nlist->num_nodes; j++) {
      for (int k = j + 1; k < nlist->num_nodes; k++) {
        if (!BNode_FindPath(i, j, k, 0.0f)) {
          LOG_INFO("BNODE ERROR: No path from %d to %d in room %d\n", j + 1, k + 1, i);
          f_verified = false;
        }
      }
    }
  }

  BNode_verified = f_verified;

  if (f_verified)
    LOG_INFO("EBNode:  VERIFY OK!\n");
  else
    LOG_INFO("EBNode:  VERIFY FAILED!\n");

  return f_verified;
}

void EBNode_ClearLevel() {
  for (int i = 0; i <= Highest_room_index + BOA_num_terrain_regions; i++) {
    if (i >= 0 && i <= Highest_room_index) {
      if (!Rooms[i].used) {
        continue;
      } else {
        for (int j = 0; j < Rooms[i].num_portals; j++)
          Rooms[i].portals[j].bnode_index = -1;
      }
    }

    bn_list *nlist = BNode_GetBNListPtr(i);
    Q_ASSERT(nlist);

    for (int j = nlist->num_nodes - 1; j >= 0; j--) {
      if (nlist->nodes[j].edges)
        mem_free(nlist->nodes[j].edges);
      nlist->nodes[j].edges = NULL;
      nlist->nodes[j].num_edges = 0;
    }

    if (nlist->nodes)
      mem_free(nlist->nodes);
    nlist->nodes = NULL;
    nlist->num_nodes = 0;
  }

  BNode_allocated = false;
  BNode_verified = false;
}

static void RemapEdgeNodesEqualAndAbove(int croom, int sroom, int spnt) {
  bn_list *cnlist = BNode_GetBNListPtr(croom);
  if (!cnlist)
    return;

  for (int i = 0; i < cnlist->num_nodes; i++) {
    for (int j = 0; j < cnlist->nodes[i].num_edges; j++) {
      if (cnlist->nodes[i].edges[j].end_room == sroom && cnlist->nodes[i].edges[j].end_index >= spnt) {
        Q_ASSERT(cnlist->nodes[i].edges[j].end_index != spnt);
        cnlist->nodes[i].edges[j].end_index--;
      }
    }
  }
}

static void RemapPortalNodeIndices(int roomnum, int pnt) {
  if (roomnum >= 0 && roomnum <= Highest_room_index) {
    Q_ASSERT(Rooms[roomnum].used);

    for (int i = 0; i < Rooms[roomnum].num_portals; i++) {
      if (Rooms[roomnum].portals[i].bnode_index == pnt) {
        Rooms[roomnum].portals[i].bnode_index = -1;
      } else if (Rooms[roomnum].portals[i].bnode_index > pnt) {
        Rooms[roomnum].portals[i].bnode_index--;
      }
    }
  } else {
    int region = BOA_INDEX(roomnum) - Highest_room_index - 1;

    for (int i = 0; i < BOA_num_connect[region]; i++) {
      int r = BOA_connect[region][i].roomnum;
      int p = BOA_connect[region][i].portal;

      int cr = Rooms[r].portals[p].croom;
      int cp = Rooms[r].portals[p].cportal;

      vector3 pos = Rooms[cr].portals[cp].path_pnt + Rooms[cr].faces[Rooms[cr].portals[cp].portal_face].normal * 0.75f;
      int cell = GetTerrainRoomFromPos(&pos);

      if (region == TERRAIN_REGION(cell) && Rooms[cr].portals[cp].bnode_index == pnt) {
        Rooms[cr].portals[cp].bnode_index = -1;
      }
    }
  }
}

void EBNode_RemoveNode(int roomnum, int pnt) {
  BNode_verified = false;

  bn_list *nlist = BNode_GetBNListPtr(roomnum);
  if (!nlist)
    return;

  Q_ASSERT(pnt >= 0 && pnt < nlist->num_nodes);

  for (int i = nlist->nodes[pnt].num_edges - 1; i >= 0; i--)
    EBNode_RemoveEdge(pnt, roomnum, nlist->nodes[pnt].edges[i].end_index, nlist->nodes[pnt].edges[i].end_room);

  for (int i = pnt; i < nlist->num_nodes - 1; i++)
    nlist->nodes[i] = nlist->nodes[i + 1];

  nlist->num_nodes--;

  if (nlist->num_nodes == 0) {
    mem_free(nlist->nodes);
    nlist->nodes = NULL;
  } else {
    nlist->nodes = (bn_node *)mem_realloc(nlist->nodes, sizeof(bn_node) * nlist->num_nodes);
  }

  int next_rooms[1000];
  int num_next_rooms = AIMakeNextRoomList(roomnum, next_rooms, 1000);

  for (int i = 0; i < num_next_rooms; i++)
    RemapEdgeNodesEqualAndAbove(next_rooms[i], roomnum, pnt);
  RemapEdgeNodesEqualAndAbove(roomnum, roomnum, pnt);
  RemapPortalNodeIndices(roomnum, pnt);
}

void EBNode_RemoveEdge(int spnt, int sroom, int epnt, int eroom, bool f_remove_reverse) {
  BNode_verified = false;

  if (sroom == eroom && spnt == epnt)
    return;

  bn_list *snlist = BNode_GetBNListPtr(sroom);
  bn_list *enlist = BNode_GetBNListPtr(eroom);

  if (!snlist)
    return;

  bool f_exists = false;
  int e_index = 0;

  for (int i = 0; i < snlist->nodes[spnt].num_edges; i++) {
    if (snlist->nodes[spnt].edges[i].end_index == epnt && snlist->nodes[spnt].edges[i].end_room == eroom) {
      e_index = i;
      f_exists = true;
      break;
    }
  }

  Q_ASSERT(f_exists);

  for (int i = e_index; i < snlist->nodes[spnt].num_edges - 1; i++)
    snlist->nodes[spnt].edges[i] = snlist->nodes[spnt].edges[i + 1];

  snlist->nodes[spnt].num_edges--;

  if (snlist->nodes[spnt].num_edges == 0) {
    mem_free(snlist->nodes[spnt].edges);
    snlist->nodes[spnt].edges = NULL;
  } else {
    snlist->nodes[spnt].edges =
        (bn_edge *)mem_realloc(snlist->nodes[spnt].edges, sizeof(bn_edge) * snlist->nodes[spnt].num_edges);
  }

  if (f_remove_reverse && enlist)
    EBNode_RemoveEdge(epnt, eroom, spnt, sroom, false);
}

int EBNode_AddNode(int roomnum, vector3 *pnt, bool f_from_editor, bool f_check_for_close_nodes) {
  bn_list *nlist = BNode_GetBNListPtr(roomnum);
  if (!nlist)
    return -1;

  BNode_verified = false;

  if (nlist->num_nodes >= MAX_BNODES_PER_ROOM) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Too many BOA Nodes for this room/region.");
    return -1;
  }

  bool f_really_close_neighbor = false;

  if (f_check_for_close_nodes) {
    for (int i = 0; i < nlist->num_nodes; i++) {
      if (vm_VectorDistance(&nlist->nodes[i].pos, pnt) <= BNODE_VERY_CLOSE_DIST) {
        f_really_close_neighbor = true;
        break;
      }
    }

    if (f_really_close_neighbor) {
      if (f_from_editor)
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "This node is really close to another one and isn't needed.");
      return -1;
    }
  }

  Q_ASSERT(!((nlist->num_nodes == 0) ^ (nlist->nodes == NULL)));

  int new_node = nlist->num_nodes;
  nlist->num_nodes++;

  if (new_node != 0)
    nlist->nodes = (bn_node *)mem_realloc(nlist->nodes, sizeof(bn_node) * nlist->num_nodes);
  else
    nlist->nodes = mem_rmalloc<bn_node>();

  nlist->nodes[new_node].edges = NULL;
  nlist->nodes[new_node].num_edges = 0;
  nlist->nodes[new_node].pos = *pnt;

  return new_node;
}

static float EBNode_DetermineMaxSizeForEdge(int spnt, int sroom, int epnt, int eroom) {
  bn_list *snlist = BNode_GetBNListPtr(sroom);
  bn_list *enlist = BNode_GetBNListPtr(eroom);

  float size = 0.0f;
  fvi_info hit_info;
  fvi_query fq;
  int fate;

  fq.flags = FQ_IGNORE_RENDER_THROUGH_PORTALS;
  fq.thisobjnum = -1;
  fq.ignore_obj_list = NULL;

  do {
    fq.p0 = &snlist->nodes[spnt].pos;
    fq.p1 = &enlist->nodes[epnt].pos;
    fq.startroom = (sroom > Highest_room_index && sroom <= Highest_room_index + 8)
                       ? GetTerrainRoomFromPos(&snlist->nodes[spnt].pos)
                       : sroom;
    fq.rad = size;

    fate = fvi_FindIntersection(&fq, &hit_info);

    if (fate == HIT_NONE) {
      fq.p0 = &enlist->nodes[epnt].pos;
      fq.p1 = &snlist->nodes[spnt].pos;
      fq.startroom = (eroom > Highest_room_index && eroom <= Highest_room_index + 8)
                         ? GetTerrainRoomFromPos(&enlist->nodes[epnt].pos)
                         : eroom;
      fq.rad = size;

      fate = fvi_FindIntersection(&fq, &hit_info);
    }

    if (fate == HIT_NONE)
      size += 1.0f;

  } while (fate == HIT_NONE && size < MAX_BNODE_SIZE + 1.0f);

  return (size - 1.0f);
}

void EBNode_AutoEdgeNode(int spnt, int sroom) {
  BNode_verified = false;

  bn_list *snlist = BNode_GetBNListPtr(sroom);

  for (int i = 0; i < snlist->num_nodes; i++) {
    if (i != spnt) {
      fvi_info hit_info;
      fvi_query fq;

      fq.p0 = &snlist->nodes[spnt].pos;
      fq.p1 = &snlist->nodes[i].pos;
      fq.startroom = (sroom > Highest_room_index && sroom <= Highest_room_index + 8)
                         ? GetTerrainRoomFromPos(&snlist->nodes[spnt].pos)
                         : sroom;

      fq.rad = 3.0f;
      fq.flags = FQ_NO_RELINK;
      fq.thisobjnum = -1;
      fq.ignore_obj_list = NULL;

      if (fvi_FindIntersection(&fq, &hit_info) == HIT_NONE)
        EBNode_AddEdge(spnt, sroom, i, sroom);
    }
  }
}

void EBNode_AddEdge(int spnt, int sroom, int epnt, int eroom, bool f_add_reverse, float computed_max_rad) {
  BNode_verified = false;

  if (sroom == eroom && spnt == epnt)
    return;

  bn_list *snlist = BNode_GetBNListPtr(sroom);
  bn_list *enlist = BNode_GetBNListPtr(eroom);

  Q_ASSERT(snlist && enlist);

  bool f_exists = false;

  for (int i = 0; i < snlist->nodes[spnt].num_edges; i++) {
    if (snlist->nodes[spnt].edges[i].end_index == epnt && snlist->nodes[spnt].edges[i].end_room == eroom) {
      f_exists = true;
      break;
    }
  }

  if (!f_exists) {
    Q_ASSERT(!((snlist->nodes[spnt].num_edges == 0) ^ (snlist->nodes[spnt].edges == NULL)));

    int new_edge = snlist->nodes[spnt].num_edges;
    snlist->nodes[spnt].num_edges++;

    if (new_edge == 0) {
      snlist->nodes[spnt].edges = mem_rmalloc<bn_edge>();
    } else {
      snlist->nodes[spnt].edges =
          (bn_edge *)mem_realloc(snlist->nodes[spnt].edges, sizeof(bn_edge) * snlist->nodes[spnt].num_edges);
    }

    float cost = vm_VectorDistance(&snlist->nodes[spnt].pos, &enlist->nodes[epnt].pos);
    if (cost < 1.0f)
      cost = 1.0f;

    snlist->nodes[spnt].edges[new_edge].cost = (cost < 32767.0f) ? (int16_t)cost : (int16_t)32767;
    snlist->nodes[spnt].edges[new_edge].end_index = epnt;
    snlist->nodes[spnt].edges[new_edge].end_room = BOA_INDEX(eroom);
    snlist->nodes[spnt].edges[new_edge].flags = 0;

    if (f_add_reverse) {
      snlist->nodes[spnt].edges[new_edge].max_rad = EBNode_DetermineMaxSizeForEdge(spnt, sroom, epnt, eroom);
      EBNode_AddEdge(epnt, eroom, spnt, sroom, false, snlist->nodes[spnt].edges[new_edge].max_rad);
    } else {
      snlist->nodes[spnt].edges[new_edge].max_rad = computed_max_rad;
    }
  }
}

void EBNode_MakeDefaultIntraRoomNodes(int roomnum) {
  room *rp = &Rooms[roomnum];

  for (int i = 0; i < rp->num_portals; i++) {
    vector3 pos;
    pos = rp->portals[i].path_pnt + rp->faces[rp->portals[i].portal_face].normal * 0.75f;
    rp->portals[i].bnode_index = i;
    EBNode_AddNode(roomnum, &pos, false, false);
  }

  EBNode_AddNode(roomnum, &rp->path_pnt, false, false);

  for (int i = 0; i < rp->bn_info.num_nodes; i++) {
    for (int j = i + 1; j < rp->bn_info.num_nodes; j++) {
      if (i < rp->bn_info.num_nodes - 1 && j < rp->bn_info.num_nodes - 1) {
        vector3 vec = rp->portals[j].path_pnt - rp->portals[i].path_pnt;
        vector3 cvec = rp->path_pnt - rp->portals[i].path_pnt;

        scalar len = vm_NormalizeVector(&vec);
        scalar cproj = vm_Dot3Product(cvec, vec);

        if (len >= cproj && cproj >= 0.0f) {
          vector3 cxline = cproj * vec;
          vector3 dvec = rp->path_pnt - (rp->portals[i].path_pnt + cxline);

          if (vm_GetMagnitude(&dvec) < 3.0f)
            continue;
        }
      }

      fvi_info hit_info;
      fvi_query fq;

      fq.p0 = &Rooms[roomnum].bn_info.nodes[i].pos;
      fq.p1 = &Rooms[roomnum].bn_info.nodes[j].pos;
      fq.startroom = (roomnum > Highest_room_index && roomnum <= Highest_room_index + 8)
                         ? GetTerrainRoomFromPos(&Rooms[roomnum].bn_info.nodes[i].pos)
                         : roomnum;

      fq.rad = 0.1f;
      fq.flags = FQ_SOLID_PORTALS | FQ_NO_RELINK;
      fq.thisobjnum = -1;
      fq.ignore_obj_list = NULL;

      if (fvi_FindIntersection(&fq, &hit_info) != HIT_NONE)
        continue;

      EBNode_AddEdge(i, roomnum, j, roomnum);
    }
  }
}

void EBNode_MakeDefaultInterRoomEdges(int roomnum) {
  for (int i = 0; i < Rooms[roomnum].num_portals; i++) {
    if (Rooms[roomnum].portals[i].cportal >= 0 && Rooms[roomnum].portals[i].croom > roomnum &&
        !(Rooms[Rooms[roomnum].portals[i].croom].flags.external)) {
      if ((Rooms[roomnum].portals[i].flags & PF_RENDER_FACES) &&
          !(Rooms[roomnum].portals[i].flags & PF_RENDERED_FLYTHROUGH)) {
        if (!(GameTextures[Rooms[roomnum].faces[Rooms[roomnum].portals[i].portal_face].tmap].flags.breakable ||
              GameTextures[Rooms[roomnum].faces[Rooms[roomnum].portals[i].portal_face].tmap].flags.forcefield))
          continue;
      }

      EBNode_AddEdge(i, roomnum, Rooms[roomnum].portals[i].cportal, Rooms[roomnum].portals[i].croom);
    }
  }
}

static void EBNode_RemoveNodesAtUnopenablePortals(int roomnum) {
  Q_ASSERT(Rooms[roomnum].num_portals + 1 == Rooms[roomnum].bn_info.num_nodes);

  for (int i = Rooms[roomnum].num_portals - 1; i >= 0; i--) {
    if ((Rooms[roomnum].portals[i].flags & PF_BLOCK) && !(Rooms[roomnum].portals[i].flags & PF_BLOCK_REMOVABLE)) {
      EBNode_RemoveNode(roomnum, i);
      continue;
    }

    if ((Rooms[roomnum].portals[i].flags & PF_RENDER_FACES) &&
        !(Rooms[roomnum].portals[i].flags & PF_RENDERED_FLYTHROUGH)) {
      if (!(GameTextures[Rooms[roomnum].faces[Rooms[roomnum].portals[i].portal_face].tmap].flags.breakable ||
            GameTextures[Rooms[roomnum].faces[Rooms[roomnum].portals[i].portal_face].tmap].flags.forcefield)) {
        EBNode_RemoveNode(roomnum, i);
        continue;
      }
    }

    if (Rooms[roomnum].portals[i].flags & PF_TOO_SMALL_FOR_ROBOT) {
      EBNode_RemoveNode(roomnum, i);
      continue;
    }
  }
}

static void EBNode_MakeDefaultTerrainNodes(int region) {
  Q_ASSERT(region >= 0 || region < BOA_num_terrain_regions);

  LOG_INFO("TR %d has %d nodes\n", region, BOA_num_connect[region]);

  for (int i = 0; i < BOA_num_connect[region]; i++) {
    int end_room = BOA_connect[region][i].roomnum;
    room *rp = &Rooms[end_room];
    int p = BOA_connect[region][i].portal;

    vector3 pos;
    pos = rp->portals[p].path_pnt - rp->faces[rp->portals[p].portal_face].normal * 0.75f;

    int external_room = rp->portals[p].croom;
    int external_portal = rp->portals[p].cportal;
    Q_ASSERT(Rooms[external_room].flags.external);
    Rooms[external_room].portals[external_portal].bnode_index = i;

    EBNode_AddNode(Highest_room_index + region + 1, &pos, false, false);
    EBNode_AddEdge(i, Highest_room_index + region + 1, p, end_room);
  }

  for (int i = 0; i < BOA_num_connect[region]; i++) {
    for (int j = i + 1; j < BOA_num_connect[region]; j++)
      EBNode_AddEdge(i, Highest_room_index + region + 1, j, Highest_room_index + region + 1);
  }
}

void EBNode_MakeFirstPass() {
  if (BNode_allocated) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "The BNode system is already made.\nUse the other functions to modify the graph.");
    return;
  }

  for (int i = 0; i <= Highest_room_index; i++) {
    Q_ASSERT(Rooms[i].bn_info.num_nodes == 0);
    if (Rooms[i].used && !Rooms[i].flags.external)
      EBNode_MakeDefaultIntraRoomNodes(i);
  }

  for (int i = 0; i < BOA_num_terrain_regions; i++) {
    Q_ASSERT(BNode_terrain_list[i].num_nodes == 0);
    EBNode_MakeDefaultTerrainNodes(i);
  }

  for (int i = 0; i <= Highest_room_index; i++) {
    if (Rooms[i].used && !Rooms[i].flags.external)
      EBNode_MakeDefaultInterRoomEdges(i);
  }

  for (int i = 0; i <= Highest_room_index; i++) {
    if (Rooms[i].used && !Rooms[i].flags.external)
      EBNode_RemoveNodesAtUnopenablePortals(i);
  }

  for (int i = 0; i <= Highest_room_index + BOA_num_terrain_regions; i++) {
    bn_list *nlist;

    if (i >= 0 && i <= Highest_room_index && !Rooms[i].used)
      continue;

    nlist = BNode_GetBNListPtr(i);

    for (int j = 0; j < nlist->num_nodes; j++) {
      for (int k = 0; k < nlist->nodes[j].num_edges; k++) {
        if (nlist->nodes[j].edges[k].max_rad < 5.0f) {
          EBNode_RemoveEdge(j, i, nlist->nodes[j].edges[k].end_index, nlist->nodes[j].edges[k].end_room);
          k--;
        }
      }
    }
  }

  BNode_allocated = true;
  EBNode_VerifyGraph();
}

int EBNode_InsertNodeOnEdge(int spnt, int sroom, int epnt, int eroom) {
  BNode_verified = false;

  bn_list *snlist = BNode_GetBNListPtr(sroom);
  bn_list *enlist = BNode_GetBNListPtr(eroom);

  bool f_exists = false;

  for (int i = 0; i < snlist->nodes[spnt].num_edges; i++) {
    if (snlist->nodes[spnt].edges[i].end_index == epnt && snlist->nodes[spnt].edges[i].end_room == eroom) {
      f_exists = true;
      break;
    }
  }

  Q_ASSERT(f_exists);

  vector3 new_pos = (snlist->nodes[spnt].pos + enlist->nodes[epnt].pos) / 2.0f;
  fvi_info hit_info;
  hit_info.hit_room = sroom;

  if (sroom != eroom) {
    fvi_query fq;
    int fate;

    fq.p0 = &snlist->nodes[spnt].pos;
    fq.p1 = &enlist->nodes[epnt].pos;
    fq.startroom = (sroom > Highest_room_index && sroom <= Highest_room_index + 8)
                       ? GetTerrainRoomFromPos(&snlist->nodes[spnt].pos)
                       : sroom;

    fq.rad = 0.0f;
    fq.flags = FQ_IGNORE_RENDER_THROUGH_PORTALS;
    fq.thisobjnum = -1;
    fq.ignore_obj_list = NULL;

    fate = fvi_FindIntersection(&fq, &hit_info);

    if (fate != HIT_NONE) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You can only do this function if the 2 nodes can\nsee each other or are in the same room.\n");
      return -1;
    }
  }

  int n_index = EBNode_AddNode(hit_info.hit_room, &new_pos, false, false);
  EBNode_AddEdge(n_index, hit_info.hit_room, spnt, sroom);
  EBNode_AddEdge(n_index, hit_info.hit_room, epnt, eroom);
  EBNode_RemoveEdge(epnt, eroom, spnt, sroom);

  return n_index;
}

static void EBNode_ComputeEdgeCosts(int sroom, int spnt, int eroom, int epnt) {
  bool f_found = false;

  bn_list *snlist = BNode_GetBNListPtr(sroom);
  bn_list *enlist = BNode_GetBNListPtr(eroom);

  for (int i = 0; i < snlist->nodes[spnt].num_edges; i++) {
    for (int j = 0; j < enlist->nodes[epnt].num_edges; j++) {
      if ((snlist->nodes[spnt].edges[i].end_index == epnt && snlist->nodes[spnt].edges[i].end_room == eroom) &&
          (enlist->nodes[epnt].edges[j].end_index == spnt && enlist->nodes[epnt].edges[j].end_room == sroom)) {
        f_found = true;

        float cost = vm_VectorDistance(&snlist->nodes[spnt].pos, &enlist->nodes[epnt].pos);
        if (cost < 1.0f)
          cost = 1.0f;

        int16_t scost = (cost < 32767.0f) ? (int16_t)cost : (int16_t)32767;

        snlist->nodes[spnt].edges[i].cost = scost;
        enlist->nodes[epnt].edges[j].cost = scost;

        snlist->nodes[spnt].edges[i].max_rad = enlist->nodes[epnt].edges[j].max_rad =
            EBNode_DetermineMaxSizeForEdge(spnt, sroom, epnt, eroom);
      }
    }
  }

  Q_ASSERT(f_found == true);
}

void EBNode_Move(bool f_offset, int roomnum, int pnt, vector3 *pos) {
  bn_list *nlist = BNode_GetBNListPtr(roomnum);

  vector3 npos = *pos;

  if (f_offset) {
    npos += nlist->nodes[pnt].pos;

    fvi_info hit_info;
    fvi_query fq;

    fq.p0 = &nlist->nodes[pnt].pos;
    fq.p1 = &npos;
    fq.startroom = (roomnum > Highest_room_index && roomnum <= Highest_room_index + 8)
                       ? GetTerrainRoomFromPos(&nlist->nodes[pnt].pos)
                       : roomnum;

    fq.rad = 0.25f;
    fq.flags = FQ_SOLID_PORTALS | FQ_NO_RELINK;
    fq.thisobjnum = -1;
    fq.ignore_obj_list = NULL;

    fvi_FindIntersection(&fq, &hit_info);

    npos = hit_info.hit_pnt;
  }

  nlist->nodes[pnt].pos = npos;

  for (int i = 0; i < nlist->nodes[pnt].num_edges; i++) {
    EBNode_ComputeEdgeCosts(roomnum, pnt, nlist->nodes[pnt].edges[i].end_room, nlist->nodes[pnt].edges[i].end_index);
    EBNode_ComputeEdgeCosts(nlist->nodes[pnt].edges[i].end_room, nlist->nodes[pnt].edges[i].end_index, roomnum, pnt);
  }
}
