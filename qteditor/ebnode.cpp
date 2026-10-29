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

ebdraw EBN_draw_type = ebdraw::none;
#define EBN_MAX_NEXT_ROOMS 200
#define BNODE_VERY_CLOSE_DIST 5.0f

bool EBNode_VerifyGraph() {
  bool f_verified = true;

  if (!BNode_allocated) {
    LOG_INFO("EBNode Verify: No BNodes for this level\n");
    return false;
  }

  MakeBOA();

  for (size_t i = Rooms.size(); i < Rooms.size() + static_cast<size_t>(BOA_num_terrain_regions); i++) {
    bn_list *nlist = BNode_GetBNListPtr(i);
    int cur_region = i - static_cast<int>(Rooms.size()) - 2;

    for (int j = static_cast<int>(nlist->nodes.size()) - 1; j >= 0; j--) {
      int cell = GetTerrainRoomFromPos(nlist->nodes[j].pos).value_or(-1);
      if (cur_region != Terrain_seg[cell].flags.region) {
        for (size_t k = 0; k < (nlist->nodes[j].edges).size(); k++) {
          if (BOA_INDEX(nlist->nodes[j].edges[k].end_room) >= 0 &&
              BOA_INDEX(nlist->nodes[j].edges[k].end_room) < static_cast<int>(Rooms.size())) {
            int r = nlist->nodes[j].edges[k].end_room;
            int p = nlist->nodes[j].edges[k].end_index;

            for (int x = 0; x < Rooms[r].portals.size(); x++) {
              if (Rooms[r].portals[x].bnode_index && *Rooms[r].portals[x].bnode_index == p) {
                const index_t cr = Rooms[r].portals[x].connected_room;
                const index_t cp = Rooms[r].portals[x].connected_portal;
                if (cr && cp)
                  Rooms[*cr].portals[*cp].bnode_index = std::nullopt;
              }
            }
          }
        }
        EBNode_RemoveNode(to_roomnum(static_cast<int>(i)), to_index(j));
      }
    }
  }

  for (size_t i = 0; i < Rooms.size() + static_cast<size_t>(BOA_num_terrain_regions); i++) {
    bn_list *nlist;

    if (i < static_cast<int>(Rooms.size()) && !Rooms[i].used)
      continue;
    if (i < static_cast<int>(Rooms.size()) && Rooms[i].flags.external)
      continue;

    nlist = BNode_GetBNListPtr(i);

    for (size_t j = 0; j < (nlist->nodes).size(); j++) {
      for (size_t k = 0; k < (nlist->nodes[j].edges).size(); k++) {
        if (nlist->nodes[j].edges[k].max_rad < 5.0f) {
          LOG_INFO("EBNode Verify: Removed a skinny edge.\n");
          EBNode_RemoveEdge(to_index(static_cast<int>(j)), to_roomnum(static_cast<int>(i)),
                            to_index(nlist->nodes[j].edges[k].end_index),
                            to_roomnum(nlist->nodes[j].edges[k].end_room));
          k--;
        }
      }
    }
  }

  for (size_t i = 0; i < Rooms.size(); i++) {
    if (Rooms[i].used) {
      room_t &rp = Rooms[i];
      if (Rooms[i].flags.external)
        continue;

      for (int j = 0; j < Rooms[i].portals.size(); j++) {
        if (Rooms[i].portals[j].bnode_index && *Rooms[i].portals[j].bnode_index >= static_cast<int16_t>(Rooms[i].bn_info.nodes.size())) {
          LOG_INFO("EBNode: Bashed an invalid node\n");
          Rooms[i].portals[j].bnode_index = std::nullopt;
        } else if (!Rooms[i].portals[j].bnode_index) {
          bool f_add = true;

          if (!((rp.portals[j].flags.block) && !(rp.portals[j].flags.block_removable))) {
            f_add = false;
          }

          if ((rp.portals[j].flags.render_faces) && !(rp.portals[j].flags.rendered_flythrough)) {
            if (!(GameTextures[rp.faces[*rp.portals[j].portal_face].tmap].flags.breakable ||
              GameTextures[rp.faces[*rp.portals[j].portal_face].tmap].flags.forcefield)) {
              f_add = false;
            }
          }

          if (rp.portals[j].flags.too_small_for_robot) {
            f_add = false;
          }

          if (f_add) {
            vector3 pos;
            pos = rp.portals[j].path_pnt + rp.faces[*rp.portals[j].portal_face].normal * 0.75f;
            rp.portals[j].bnode_index = EBNode_AddNode(to_roomnum(static_cast<int>(i)), pos, false, false);
            LOG_INFO("EBNode Verify: Added a portal node\n");
          }
        }
      }
    }
  }

  for (int region = 0; region < static_cast<int>(BOA_num_terrain_regions); region++) {
    for (int i = 0; i < BOA_num_connect[region]; i++) {
      const index_t end_room = BOA_connect[region][i].roomnum;
      const index_t p = BOA_connect[region][i].portal;
      Q_ASSERT(end_room && p);
      const room_t &rp = Rooms[*end_room];

      vector3 pos;
      pos = rp.portals[*p].path_pnt - rp.faces[*rp.portals[*p].portal_face].normal * 0.75f;

      const index_t external_room = rp.portals[*p].connected_room;
      const index_t external_portal = rp.portals[*p].connected_portal;
      Q_ASSERT(external_room && external_portal && Rooms[*external_room].flags.external);

      if (!Rooms[*external_room].portals[*external_portal].bnode_index) {
        Rooms[*external_room].portals[*external_portal].bnode_index = EBNode_AddNode(to_roomnum(static_cast<int>(Rooms.size()) + region), pos, false, false);
        if (Rooms[*end_room].portals[*p].bnode_index.has_value())
          EBNode_AddEdge(*Rooms[*external_room].portals[*external_portal].bnode_index,
                         to_roomnum(static_cast<int>(Rooms.size()) + region),
                         *Rooms[*end_room].portals[*p].bnode_index, end_room);
      }
    }
  }

  for (size_t i = 0; i < Rooms.size(); i++) {
    if (Rooms[i].used) {
      room_t &rp = Rooms[i];
      if (Rooms[i].flags.external)
        continue;

      for (int j = 0; j < Rooms[i].portals.size(); j++) {
        if ((Rooms[i].portals[j].flags.block) && !(Rooms[i].portals[j].flags.block_removable)) {
          if (Rooms[i].portals[j].bnode_index.has_value()) {
            LOG_INFO("EBNode Verify: Removed a node.\n");
            EBNode_RemoveNode(to_roomnum(static_cast<int>(i)), Rooms[i].portals[j].bnode_index);
          }
          continue;
        }

        if ((Rooms[i].portals[j].flags.render_faces) && !(Rooms[i].portals[j].flags.rendered_flythrough)) {
          if (!(GameTextures[Rooms[i].faces[*Rooms[i].portals[j].portal_face].tmap].flags.breakable ||
                GameTextures[Rooms[i].faces[*Rooms[i].portals[j].portal_face].tmap].flags.forcefield)) {
            if (Rooms[i].portals[j].bnode_index.has_value()) {
              LOG_INFO("EBNode Verify: Removed a node.\n");
              EBNode_RemoveNode(to_roomnum(static_cast<int>(i)), Rooms[i].portals[j].bnode_index);
            }
            continue;
          }
        }

        if (Rooms[i].portals[j].flags.too_small_for_robot) {
          if (Rooms[i].portals[j].bnode_index.has_value()) {
            LOG_INFO("EBNode Verify: Removed a node.\n");
            EBNode_RemoveNode(to_roomnum(static_cast<int>(i)), Rooms[i].portals[j].bnode_index);
          }
          continue;
        }

        if (!Rooms[i].portals[j].bnode_index) {
          if (rp.flags.external) {
            const index_t cr = rp.portals[j].connected_room;
            const index_t cp = rp.portals[j].connected_portal;
            if (!cr || !cp)
              continue;
            int ci = Rooms[*cr].portals[*cp].bnode_index ? static_cast<int>(*Rooms[*cr].portals[*cp].bnode_index) : -1;

            if (Rooms[*cr].flags.external)
              continue;

            vector3 pos;
            pos = rp.portals[j].path_pnt + rp.faces[*rp.portals[j].portal_face].normal * 0.75f;
            index_t roomnum = BOA_INDEX(GetTerrainRoomFromPos(pos).has_value()? GetTerrainRoomFromPos(pos).value() : -1);

            int xxx;
            uint8_t region = Terrain_seg[*roomnum].flags.region;
            for (xxx = 0; xxx < BOA_num_connect[region]; xxx++) {
              if (BOA_connect[region][xxx].roomnum == cr &&
                  BOA_connect[region][xxx].portal == cp) {
                break;
              }
            }

            if (xxx >= BOA_num_connect[region]) {
              LOG_INFO("EBNode Verify:  External room isn't in terrain region list\n");
              f_verified = false;
              continue;
            }

            rp.portals[j].bnode_index = EBNode_AddNode(roomnum, pos, false, false);
            Q_ASSERT(rp.portals[j].bnode_index.has_value());
            EBNode_AutoEdgeNode(rp.portals[j].bnode_index, roomnum);
            LOG_INFO("EBNode Verify: Added a node and autoedged it.\n");

            if (ci >= 0) {
              EBNode_AddEdge(rp.portals[j].bnode_index, roomnum, to_index(ci), cr);
            }
          } else {
            vector3 pos;
            pos = rp.portals[j].path_pnt + rp.faces[*rp.portals[j].portal_face].normal * 0.75f;
            rp.portals[j].bnode_index = EBNode_AddNode(to_roomnum(static_cast<int>(i)), pos, false, false);
            Q_ASSERT(rp.portals[j].bnode_index.has_value());
            EBNode_AutoEdgeNode(rp.portals[j].bnode_index, to_roomnum(static_cast<int>(i)));
            LOG_INFO("EBNode Verify: Added a node and autoedged it.\n");

            const index_t cr = rp.portals[j].connected_room;
            const index_t cp = rp.portals[j].connected_portal;
            if (!cr || !cp)
              continue;
            int ci = Rooms[*cr].portals[*cp].bnode_index ? static_cast<int>(*Rooms[*cr].portals[*cp].bnode_index) : -1;

            if (ci < 0)
              continue;

            if (Rooms[*cr].flags.external) {
              vector3 pos;
              pos = rp.portals[j].path_pnt - rp.faces[*rp.portals[j].portal_face].normal * 0.75f;
              index_t roomnum = BOA_INDEX(GetTerrainRoomFromPos(pos).has_value()? GetTerrainRoomFromPos(pos).value() : -1);
              EBNode_AddEdge(rp.portals[j].bnode_index, to_roomnum(static_cast<int>(i)), to_index(ci), roomnum);
            } else {
              if (ci >= 0)
                EBNode_AddEdge(rp.portals[j].bnode_index, to_roomnum(static_cast<int>(i)), to_index(ci), cr);
            }
          }
        }
      }
    }
  }

  for (size_t i = 0; i < Rooms.size() + static_cast<size_t>(BOA_num_terrain_regions); i++) {
    bn_list *nlist;

    if (i < static_cast<int>(Rooms.size()) && !Rooms[i].used)
      continue;
    if (i < static_cast<int>(Rooms.size()) && Rooms[i].flags.external)
      continue;

    nlist = BNode_GetBNListPtr(i);

    for (size_t j = 0; j < (nlist->nodes).size(); j++) {
      for (size_t k = 0; k < (nlist->nodes[j].edges).size(); k++) {
        if (nlist->nodes[j].edges[k].max_rad < 5.0f) {
          LOG_INFO("EBNode Verify: Removed a skinny edge.\n");
          EBNode_RemoveEdge(to_index(static_cast<int>(j)), to_roomnum(static_cast<int>(i)),
                            to_index(nlist->nodes[j].edges[k].end_index),
                            to_roomnum(nlist->nodes[j].edges[k].end_room));
          k--;
        }
      }

      for (size_t k = 0; k < (nlist->nodes[j].edges).size(); k++) {
        if (nlist->nodes[j].edges[k].end_room < static_cast<int>(Rooms.size()) &&
            !Rooms[nlist->nodes[j].edges[k].end_room].used) {
          LOG_INFO("EBNode Verify: Removed a edge to a non-existant room. Room %d, node %d, edge %d\n", i, j, k);
          EBNode_RemoveEdge(to_index(static_cast<int>(j)), to_roomnum(static_cast<int>(i)),
                            to_index(nlist->nodes[j].edges[k].end_index),
                            to_roomnum(nlist->nodes[j].edges[k].end_room));
          k--;
        }
      }
    }
  }

  for (size_t i = 0; i < Rooms.size() + static_cast<size_t>(BOA_num_terrain_regions); i++) {
    bn_list *nlist;

    if (i < static_cast<int>(Rooms.size()) && !Rooms[i].used)
      continue;
    if (i < static_cast<int>(Rooms.size()) && Rooms[i].flags.external)
      continue;

    nlist = BNode_GetBNListPtr(i);

    for (size_t j = 0; j < (nlist->nodes).size(); j++) {
      for (size_t k = 0; k < (nlist->nodes[j].edges).size(); k++) {
        if (nlist->nodes[j].edges[k].max_rad < 5.0f) {
          LOG_INFO("Skinny Edge - from r%d n%d to r%d n%d\n", i, j, nlist->nodes[j].edges[k].end_room,
                  nlist->nodes[j].edges[k].end_index);
          f_verified = false;
        }
      }
    }
  }

  for (size_t i = 0; i < Rooms.size() + static_cast<size_t>(BOA_num_terrain_regions); i++) {
    if (i < static_cast<int>(Rooms.size()) && !Rooms[i].used)
      continue;
    if (i < static_cast<int>(Rooms.size()) && Rooms[i].flags.external)
      continue;

    bn_list *nlist = BNode_GetBNListPtr(i);
    Q_ASSERT(nlist);

    for (size_t j = 0; j < (nlist->nodes).size(); j++) {
      for (size_t k = static_cast<size_t>(j) + 1; k < nlist->nodes.size(); k++) {
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
  for (size_t i = 0; i < Rooms.size() + static_cast<size_t>(BOA_num_terrain_regions); i++) {
    if (i < static_cast<int>(Rooms.size())) {
      if (!Rooms[i].used) {
        continue;
      } else {
        for (int j = 0; j < Rooms[i].portals.size(); j++)
          Rooms[i].portals[j].bnode_index = std::nullopt;
      }
    }

    bn_list *nlist = BNode_GetBNListPtr(i);
    Q_ASSERT(nlist);

    nlist->nodes.clear();
  }

  BNode_allocated = false;
  BNode_verified = false;
}

static void RemapEdgeNodesEqualAndAbove(index_t croom, index_t sroom, index_t spnt) {
  bn_list *cnlist = BNode_GetBNListPtr(croom);
  if (!cnlist)
    return;

  const int sroom_index = index_to_int(sroom);
  const int spnt_index = index_to_int(spnt);

  for (size_t i = 0; i < (cnlist->nodes).size(); i++) {
    for (size_t j = 0; j < (cnlist->nodes[i].edges).size(); j++) {
      if (cnlist->nodes[i].edges[j].end_room == sroom_index &&
          cnlist->nodes[i].edges[j].end_index >= spnt_index) {
        Q_ASSERT(cnlist->nodes[i].edges[j].end_index != spnt_index);
        cnlist->nodes[i].edges[j].end_index--;
      }
    }
  }
}

static void RemapPortalNodeIndices(index_t roomnum, index_t pnt) {
  const int pnt_index = index_to_int(pnt);

  if (roomnum.has_value() && *roomnum < static_cast<int>(Rooms.size())) {
    Q_ASSERT(Rooms[*roomnum].used);

    for (int i = 0; i < Rooms[*roomnum].portals.size(); i++) {
      if (Rooms[*roomnum].portals[i].bnode_index &&
          *Rooms[*roomnum].portals[i].bnode_index == static_cast<uint32_t>(pnt_index)) {
        Rooms[*roomnum].portals[i].bnode_index = std::nullopt;
      } else if (Rooms[*roomnum].portals[i].bnode_index &&
                 *Rooms[*roomnum].portals[i].bnode_index > static_cast<uint32_t>(pnt_index)) {
        Rooms[*roomnum].portals[i].bnode_index = *Rooms[*roomnum].portals[i].bnode_index - 1;
      }
    }
  } else {
    int region = BOA_INDEX(*roomnum) - static_cast<int>(Rooms.size()) - 2;

    for (int i = 0; i < BOA_num_connect[region]; i++) {
      const index_t r = BOA_connect[region][i].roomnum;
      const index_t p = BOA_connect[region][i].portal;
      if (!r || !p)
        continue;

      const index_t cr = Rooms[*r].portals[*p].connected_room;
      const index_t cp = Rooms[*r].portals[*p].connected_portal;
      if (!cr || !cp)
        continue;

      vector3 pos =
          Rooms[*cr].portals[*cp].path_pnt + Rooms[*cr].faces[*Rooms[*cr].portals[*cp].portal_face].normal * 0.75f;
      int cell = GetTerrainRoomFromPos(pos).value_or(-1);

      if (region == Terrain_seg[cell].flags.region && Rooms[*cr].portals[*cp].bnode_index == pnt) {
        Rooms[*cr].portals[*cp].bnode_index = std::nullopt;
      }
    }
  }
}

void EBNode_RemoveNode(index_t roomnum, index_t pnt) {
  BNode_verified = false;

  const int node_index = index_to_int(pnt);

  bn_list *nlist = BNode_GetBNListPtr(roomnum);
  if (!nlist)
    return;

  Q_ASSERT(node_index >= 0 && node_index < static_cast<int>(nlist->nodes.size()));

  for (int i = static_cast<int>(nlist->nodes[node_index].edges.size()) - 1; i >= 0; i--)
    EBNode_RemoveEdge(pnt, roomnum, to_index(nlist->nodes[node_index].edges[i].end_index),
                      to_roomnum(nlist->nodes[node_index].edges[i].end_room));

  nlist->nodes.erase(nlist->nodes.begin() + node_index);

  int next_rooms[1000];
  int num_next_rooms = AIMakeNextRoomList(roomnum, next_rooms, 1000);

  for (int i = 0; i < num_next_rooms; i++)
    RemapEdgeNodesEqualAndAbove(to_roomnum(next_rooms[i]), roomnum, pnt);
  RemapEdgeNodesEqualAndAbove(roomnum, roomnum, pnt);
  RemapPortalNodeIndices(roomnum, pnt);
}

void EBNode_RemoveEdge(index_t spnt, index_t sroom, index_t epnt, index_t eroom, bool f_remove_reverse) {
  BNode_verified = false;

  if (sroom == eroom && spnt == epnt)
    return;

  bn_list *snlist = BNode_GetBNListPtr(sroom);
  bn_list *enlist = BNode_GetBNListPtr(eroom);

  if (!snlist)
    return;

  const int spnt_index = index_to_int(spnt);
  const int epnt_index = index_to_int(epnt);
  const int eroom_index = index_to_int(eroom);

  bool f_exists = false;
  int e_index = 0;

  for (size_t i = 0; i < (snlist->nodes[spnt_index].edges).size(); i++) {
    if (snlist->nodes[spnt_index].edges[i].end_index == epnt_index &&
        snlist->nodes[spnt_index].edges[i].end_room == eroom_index) {
      e_index = static_cast<int>(i);
      f_exists = true;
      break;
    }
  }

  Q_ASSERT(f_exists);

  snlist->nodes[spnt_index].edges.erase(snlist->nodes[spnt_index].edges.begin() + e_index);

  if (f_remove_reverse && enlist)
    EBNode_RemoveEdge(epnt, eroom, spnt, sroom, false);
}

index_t EBNode_AddNode(index_t roomnum, const vector3 &pnt, bool f_from_editor, bool f_check_for_close_nodes) {
  bn_list *nlist = BNode_GetBNListPtr(roomnum);
  if (!nlist)
    return std::nullopt;

  BNode_verified = false;

  if (nlist->nodes.size() >= MAX_BNODES_PER_ROOM) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Too many BOA Nodes for this room/region.");
    return std::nullopt;
  }

  bool f_really_close_neighbor = false;

  if (f_check_for_close_nodes) {
    for (size_t i = 0; i < (nlist->nodes).size(); i++) {
      if (vm_VectorDistance(nlist->nodes[i].pos, pnt) <= BNODE_VERY_CLOSE_DIST) {
        f_really_close_neighbor = true;
        break;
      }
    }

    if (f_really_close_neighbor) {
      if (f_from_editor)
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "This node is really close to another one and isn't needed.");
      return std::nullopt;
    }
  }

  int new_node = static_cast<int>(nlist->nodes.size());
  nlist->nodes.push_back(bn_node{});
  nlist->nodes[new_node].pos = pnt;

  return to_index(new_node);
}

static float EBNode_DetermineMaxSizeForEdge(index_t spnt, index_t sroom, index_t epnt, index_t eroom) {
  bn_list *snlist = BNode_GetBNListPtr(sroom);
  bn_list *enlist = BNode_GetBNListPtr(eroom);

  const int spnt_index = index_to_int(spnt);
  const int epnt_index = index_to_int(epnt);
  const int sroom_index = index_to_int(sroom);
  const int eroom_index = index_to_int(eroom);

  float size = 0.0f;
  fvi_info hit_info;
  fvi_query fq;
  fvi_hit_type fate;

  fq.flags = fvi_query_flags_t{};
  fq.flags.ignore_render_through_portals = true;
  fq.thisobjnum.reset();
  fq.ignore_obj_list = NULL;

  do {
    fq.p0 = &snlist->nodes[spnt_index].pos;
    fq.p1 = &enlist->nodes[epnt_index].pos;
    fq.startroom = (sroom_index > (static_cast<int>(Rooms.size()) - 1) &&
                    sroom_index < static_cast<int>(Rooms.size()) + 8)
                       ? GetTerrainRoomFromPos(snlist->nodes[spnt_index].pos).value_or(-1)
                       : sroom_index;
    fq.rad = size;

    fate = fvi_FindIntersection(fq, hit_info);

    if (fate == fvi_hit_type::none) {
      fq.p0 = &enlist->nodes[epnt_index].pos;
      fq.p1 = &snlist->nodes[spnt_index].pos;
      fq.startroom = (eroom_index > (static_cast<int>(Rooms.size()) - 1) &&
                      eroom_index < static_cast<int>(Rooms.size()) + 8)
                         ? GetTerrainRoomFromPos(enlist->nodes[epnt_index].pos).value_or(-1)
                         : eroom_index;
      fq.rad = size;

      fate = fvi_FindIntersection(fq, hit_info);
    }

    if (fate == fvi_hit_type::none)
      size += 1.0f;

  } while (fate == fvi_hit_type::none && size < MAX_BNODE_SIZE + 1.0f);

  return (size - 1.0f);
}

void EBNode_AutoEdgeNode(index_t spnt, index_t sroom) {
  BNode_verified = false;

  bn_list *snlist = BNode_GetBNListPtr(sroom);

  const int spnt_index = index_to_int(spnt);
  const int sroom_index = index_to_int(sroom);

  for (size_t i = 0; i < (snlist->nodes).size(); i++) {
    if (static_cast<int>(i) != spnt_index) {
      fvi_info hit_info;
      fvi_query fq;

      fq.p0 = &snlist->nodes[spnt_index].pos;
      fq.p1 = &snlist->nodes[i].pos;
      fq.startroom = (sroom_index > (static_cast<int>(Rooms.size()) - 1) &&
                      sroom_index < static_cast<int>(Rooms.size()) + 8)
                         ? GetTerrainRoomFromPos(snlist->nodes[spnt_index].pos).value_or(-1)
                         : sroom_index;

      fq.rad = 3.0f;
      fq.flags = fvi_query_flags_t{};
      fq.flags.no_relink = true;
      fq.thisobjnum.reset();
      fq.ignore_obj_list = NULL;

      if (fvi_FindIntersection(fq, hit_info) == fvi_hit_type::none)
        EBNode_AddEdge(spnt, sroom, to_index(static_cast<int>(i)), sroom);
    }
  }
}

void EBNode_AddEdge(index_t spnt, index_t sroom, index_t epnt, index_t eroom, bool f_add_reverse, float computed_max_rad) {
  BNode_verified = false;

  if (sroom == eroom && spnt == epnt)
    return;

  bn_list *snlist = BNode_GetBNListPtr(sroom);
  bn_list *enlist = BNode_GetBNListPtr(eroom);

  Q_ASSERT(snlist && enlist);

  const int spnt_index = index_to_int(spnt);
  const int epnt_index = index_to_int(epnt);
  const int eroom_index = index_to_int(eroom);

  bool f_exists = false;

  for (size_t i = 0; i < (snlist->nodes[spnt_index].edges).size(); i++) {
    if (snlist->nodes[spnt_index].edges[i].end_index == epnt_index &&
        snlist->nodes[spnt_index].edges[i].end_room == eroom_index) {
      f_exists = true;
      break;
    }
  }

  if (!f_exists) {
    int new_edge = static_cast<int>(snlist->nodes[spnt_index].edges.size());
    snlist->nodes[spnt_index].edges.push_back(bn_edge{});

    float cost = vm_VectorDistance(snlist->nodes[spnt_index].pos, enlist->nodes[epnt_index].pos);
    if (cost < 1.0f)
      cost = 1.0f;

    snlist->nodes[spnt_index].edges[new_edge].cost = (cost < 32767.0f) ? (int16_t)cost : (int16_t)32767;
    snlist->nodes[spnt_index].edges[new_edge].end_index = static_cast<int8_t>(epnt_index);
    snlist->nodes[spnt_index].edges[new_edge].end_room = static_cast<int16_t>(BOA_INDEX(eroom));
    snlist->nodes[spnt_index].edges[new_edge].flags = {};

    if (f_add_reverse) {
      snlist->nodes[spnt_index].edges[new_edge].max_rad =
          EBNode_DetermineMaxSizeForEdge(spnt, sroom, epnt, eroom);
      EBNode_AddEdge(epnt, eroom, spnt, sroom, false, snlist->nodes[spnt_index].edges[new_edge].max_rad);
    } else {
      snlist->nodes[spnt_index].edges[new_edge].max_rad = computed_max_rad;
    }
  }
}

void EBNode_MakeDefaultIntraRoomNodes(index_t roomnum) {
  room_t &rp = Rooms[*roomnum];
  const int room_index = index_to_int(roomnum);

  for (int i = 0; i < rp.portals.size(); i++) {
    vector3 pos;
    pos = rp.portals[i].path_pnt + rp.faces[*rp.portals[i].portal_face].normal * 0.75f;
    rp.portals[i].bnode_index = to_index(i);
    EBNode_AddNode(roomnum, pos, false, false);
  }

  EBNode_AddNode(roomnum, rp.path_pnt, false, false);

  for (size_t i = 0; i < (rp.bn_info.nodes).size(); i++) {
    for (size_t j = static_cast<size_t>(i) + 1; j < rp.bn_info.nodes.size(); j++) {
      if (i < static_cast<int>(rp.bn_info.nodes.size()) - 1 && j < static_cast<int>(rp.bn_info.nodes.size()) - 1) {
        vector3 vec = rp.portals[j].path_pnt - rp.portals[i].path_pnt;
        vector3 cvec = rp.path_pnt - rp.portals[i].path_pnt;

        scalar len = vm_NormalizeVector(vec);
        scalar cproj = vector3::dot(cvec, vec);

        if (len >= cproj && cproj >= 0.0f) {
          vector3 cxline = cproj * vec;
          vector3 dvec = rp.path_pnt - (rp.portals[i].path_pnt + cxline);

          if (vm_GetMagnitude(dvec) < 3.0f)
            continue;
        }
      }

      fvi_info hit_info;
      fvi_query fq;

      fq.p0 = &Rooms[*roomnum].bn_info.nodes[i].pos;
      fq.p1 = &Rooms[*roomnum].bn_info.nodes[j].pos;
      fq.startroom = (room_index > (static_cast<int>(Rooms.size()) - 1) &&
                      room_index < static_cast<int>(Rooms.size()) + 8)
                         ? GetTerrainRoomFromPos(Rooms[*roomnum].bn_info.nodes[i].pos).value_or(-1)
                         : room_index;

      fq.rad = 0.1f;
      fq.flags = fvi_query_flags_t{};
      fq.flags.solid_portals = true;
      fq.flags.no_relink = true;
      fq.thisobjnum.reset();
      fq.ignore_obj_list = NULL;

      if (fvi_FindIntersection(fq, hit_info) != fvi_hit_type::none)
        continue;

      EBNode_AddEdge(to_index(static_cast<int>(i)), roomnum, to_index(static_cast<int>(j)), roomnum);
    }
  }
}

void EBNode_MakeDefaultInterRoomEdges(index_t roomnum) {
  for (int i = 0; i < Rooms[*roomnum].portals.size(); i++) {
    const index_t cportal = Rooms[*roomnum].portals[i].connected_portal;
    const index_t croom = Rooms[*roomnum].portals[i].connected_room;
    if (cportal && croom && *croom > static_cast<uint32_t>(*roomnum) && !(Rooms[*croom].flags.external)) {
      if ((Rooms[*roomnum].portals[i].flags.render_faces) &&
          !(Rooms[*roomnum].portals[i].flags.rendered_flythrough)) {
        if (!(GameTextures[Rooms[*roomnum].faces[*Rooms[*roomnum].portals[i].portal_face].tmap].flags.breakable ||
              GameTextures[Rooms[*roomnum].faces[*Rooms[*roomnum].portals[i].portal_face].tmap].flags.forcefield))
          continue;
      }

      EBNode_AddEdge(to_index(i), roomnum, cportal, croom);
    }
  }
}

static void EBNode_RemoveNodesAtUnopenablePortals(index_t roomnum) {
  Q_ASSERT(Rooms[*roomnum].portals.size() + 1 == Rooms[*roomnum].bn_info.nodes.size());

  for (int i = static_cast<int>(Rooms[*roomnum].portals.size()) - 1; i >= 0; i--) {
    if ((Rooms[*roomnum].portals[i].flags.block) && !(Rooms[*roomnum].portals[i].flags.block_removable)) {
      EBNode_RemoveNode(roomnum, to_index(i));
      continue;
    }

    if ((Rooms[*roomnum].portals[i].flags.render_faces) &&
        !(Rooms[*roomnum].portals[i].flags.rendered_flythrough)) {
      if (!(GameTextures[Rooms[*roomnum].faces[*Rooms[*roomnum].portals[i].portal_face].tmap].flags.breakable ||
            GameTextures[Rooms[*roomnum].faces[*Rooms[*roomnum].portals[i].portal_face].tmap].flags.forcefield)) {
        EBNode_RemoveNode(roomnum, to_index(i));
        continue;
      }
    }

    if (Rooms[*roomnum].portals[i].flags.too_small_for_robot) {
      EBNode_RemoveNode(roomnum, to_index(i));
      continue;
    }
  }
}

static void EBNode_MakeDefaultTerrainNodes(int region) {
  Q_ASSERT(region >= 0 || region < static_cast<int>(BOA_num_terrain_regions));

  LOG_INFO("TR %d has %d nodes\n", region, BOA_num_connect[region]);

  for (int i = 0; i < BOA_num_connect[region]; i++) {
    const index_t end_room = BOA_connect[region][i].roomnum;
    const index_t p = BOA_connect[region][i].portal;
    Q_ASSERT(end_room && p);
    const room_t &rp = Rooms[*end_room];

    vector3 pos;
    pos = rp.portals[*p].path_pnt - rp.faces[*rp.portals[*p].portal_face].normal * 0.75f;

    const index_t external_room = rp.portals[*p].connected_room;
    const index_t external_portal = rp.portals[*p].connected_portal;
    Q_ASSERT(external_room && external_portal && Rooms[*external_room].flags.external);
    Rooms[*external_room].portals[*external_portal].bnode_index = to_index(i);

    const index_t terrain_room = to_roomnum(static_cast<int>(Rooms.size()) + region);
    EBNode_AddNode(terrain_room, pos, false, false);
    EBNode_AddEdge(to_index(i), terrain_room, p, end_room);
  }

  for (int i = 0; i < BOA_num_connect[region]; i++) {
    for (int j = i + 1; j < BOA_num_connect[region]; j++) {
      const index_t terrain_room = to_roomnum(static_cast<int>(Rooms.size()) + region);
      EBNode_AddEdge(to_index(i), terrain_room, to_index(j), terrain_room);
    }
  }
}

void EBNode_MakeFirstPass() {
  if (BNode_allocated) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "The BNode system is already made.\nUse the other functions to modify the graph.");
    return;
  }

  for (size_t i = 0; i < Rooms.size(); i++) {
    Q_ASSERT(Rooms[i].bn_info.nodes.empty());
    if (Rooms[i].used && !Rooms[i].flags.external)
      EBNode_MakeDefaultIntraRoomNodes(to_roomnum(static_cast<int>(i)));
  }

  for (int i = 0; i < static_cast<int>(BOA_num_terrain_regions); i++) {
    Q_ASSERT(BNode_terrain_list[i].nodes.empty());
    EBNode_MakeDefaultTerrainNodes(i);
  }

  for (size_t i = 0; i < Rooms.size(); i++) {
    if (Rooms[i].used && !Rooms[i].flags.external)
      EBNode_MakeDefaultInterRoomEdges(to_roomnum(static_cast<int>(i)));
  }

  for (size_t i = 0; i < Rooms.size(); i++) {
    if (Rooms[i].used && !Rooms[i].flags.external)
      EBNode_RemoveNodesAtUnopenablePortals(to_roomnum(static_cast<int>(i)));
  }

  for (size_t i = 0; i < Rooms.size() + static_cast<size_t>(BOA_num_terrain_regions); i++) {
    bn_list *nlist;

    if (i < static_cast<int>(Rooms.size()) && !Rooms[i].used)
      continue;

    nlist = BNode_GetBNListPtr(i);

    for (size_t j = 0; j < (nlist->nodes).size(); j++) {
      for (size_t k = 0; k < (nlist->nodes[j].edges).size(); k++) {
        if (nlist->nodes[j].edges[k].max_rad < 5.0f) {
          EBNode_RemoveEdge(to_index(static_cast<int>(j)), to_roomnum(static_cast<int>(i)),
                            to_index(nlist->nodes[j].edges[k].end_index),
                            to_roomnum(nlist->nodes[j].edges[k].end_room));
          k--;
        }
      }
    }
  }

  BNode_allocated = true;
  EBNode_VerifyGraph();
}

index_t EBNode_InsertNodeOnEdge(index_t spnt, index_t sroom, index_t epnt, index_t eroom) {
  BNode_verified = false;

  bn_list *snlist = BNode_GetBNListPtr(sroom);
  bn_list *enlist = BNode_GetBNListPtr(eroom);

  const int spnt_index = index_to_int(spnt);
  const int epnt_index = index_to_int(epnt);
  const int sroom_index = index_to_int(sroom);
  const int eroom_index = index_to_int(eroom);

  bool f_exists = false;

  for (size_t i = 0; i < (snlist->nodes[spnt_index].edges).size(); i++) {
    if (snlist->nodes[spnt_index].edges[i].end_index == epnt_index &&
        snlist->nodes[spnt_index].edges[i].end_room == eroom_index) {
      f_exists = true;
      break;
    }
  }

  Q_ASSERT(f_exists);

  vector3 new_pos = (snlist->nodes[spnt_index].pos + enlist->nodes[epnt_index].pos) / 2.0f;
  fvi_info hit_info;
  hit_info.hit_room = sroom_index;

  if (sroom != eroom) {
    fvi_query fq;
    fvi_hit_type fate;

    fq.p0 = &snlist->nodes[spnt_index].pos;
    fq.p1 = &enlist->nodes[epnt_index].pos;
    fq.startroom = (sroom_index >= static_cast<int>(Rooms.size()) &&
                    sroom_index < static_cast<int>(Rooms.size()) + 8)
                       ? GetTerrainRoomFromPos(snlist->nodes[spnt_index].pos).value_or(-1)
                       : sroom_index;

    fq.rad = 0.0f;
    fq.flags = fvi_query_flags_t{};
    fq.flags.ignore_render_through_portals = true;
    fq.thisobjnum.reset();
    fq.ignore_obj_list = NULL;

    fate = fvi_FindIntersection(fq, hit_info);

    if (fate != fvi_hit_type::none) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You can only do this function if the 2 nodes can\nsee each other or are in the same room.\n");
      return std::nullopt;
    }
  }

  const index_t hit_room = to_roomnum(hit_info.hit_room);
  index_t n_index = EBNode_AddNode(hit_room, new_pos, false, false);
  EBNode_AddEdge(n_index, hit_room, spnt, sroom);
  EBNode_AddEdge(n_index, hit_room, epnt, eroom);
  EBNode_RemoveEdge(epnt, eroom, spnt, sroom);

  return n_index;
}

static void EBNode_ComputeEdgeCosts(index_t sroom, index_t spnt, index_t eroom, index_t epnt) {
  bool f_found = false;

  bn_list *snlist = BNode_GetBNListPtr(sroom);
  bn_list *enlist = BNode_GetBNListPtr(eroom);

  const int spnt_index = index_to_int(spnt);
  const int epnt_index = index_to_int(epnt);
  const int sroom_index = index_to_int(sroom);
  const int eroom_index = index_to_int(eroom);

  for (size_t i = 0; i < (snlist->nodes[spnt_index].edges).size(); i++) {
    for (size_t j = 0; j < (enlist->nodes[epnt_index].edges).size(); j++) {
      if ((snlist->nodes[spnt_index].edges[i].end_index == epnt_index &&
           snlist->nodes[spnt_index].edges[i].end_room == eroom_index) &&
          (enlist->nodes[epnt_index].edges[j].end_index == spnt_index &&
           enlist->nodes[epnt_index].edges[j].end_room == sroom_index)) {
        f_found = true;

        float cost = vm_VectorDistance(snlist->nodes[spnt_index].pos, enlist->nodes[epnt_index].pos);
        if (cost < 1.0f)
          cost = 1.0f;

        int16_t scost = (cost < 32767.0f) ? (int16_t)cost : (int16_t)32767;

        snlist->nodes[spnt_index].edges[i].cost = scost;
        enlist->nodes[epnt_index].edges[j].cost = scost;

        snlist->nodes[spnt_index].edges[i].max_rad = enlist->nodes[epnt_index].edges[j].max_rad =
            EBNode_DetermineMaxSizeForEdge(spnt, sroom, epnt, eroom);
      }
    }
  }

  Q_ASSERT(f_found == true);
}

void EBNode_Move(bool f_offset, index_t roomnum, index_t pnt, const vector3& pos) {
  bn_list *nlist = BNode_GetBNListPtr(roomnum);

  const int node_index = index_to_int(pnt);
  const int room_index = index_to_int(roomnum);

  vector3 npos = pos;

  if (f_offset) {
    npos += nlist->nodes[node_index].pos;

    fvi_info hit_info;
    fvi_query fq;

    fq.p0 = &nlist->nodes[node_index].pos;
    fq.p1 = &npos;
    fq.startroom = (room_index >= static_cast<int>(Rooms.size()) &&
                    room_index < static_cast<int>(Rooms.size()) + 8)
                       ? GetTerrainRoomFromPos(nlist->nodes[node_index].pos).value_or(-1)
                       : room_index;

    fq.rad = 0.25f;
    fq.flags = fvi_query_flags_t{};
    fq.flags.solid_portals = true;
    fq.flags.no_relink = true;
    fq.thisobjnum.reset();
    fq.ignore_obj_list = NULL;

    fvi_FindIntersection(fq, hit_info);

    npos = hit_info.hit_pnt;
  }

  nlist->nodes[node_index].pos = npos;

  for (size_t i = 0; i < (nlist->nodes[node_index].edges).size(); i++) {
    const index_t end_room = to_roomnum(nlist->nodes[node_index].edges[i].end_room);
    const index_t end_node = to_index(nlist->nodes[node_index].edges[i].end_index);
    EBNode_ComputeEdgeCosts(roomnum, pnt, end_room, end_node);
    EBNode_ComputeEdgeCosts(end_room, end_node, roomnum, pnt);
  }
}
