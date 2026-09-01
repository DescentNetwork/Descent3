/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral room management (ported from the original room.cpp).
 */

#include "room.h"
#include "mem.h"
#include "d3edit.h"

#include <cstring>
#include <cmath>
#include <cstdint>

void InitRoom(room *rp, int nverts, int nfaces, int nportals) {
  memset(&rp->flags, 0, sizeof(uint32_t));
  rp->objects = -1;
  rp->vis_effects = -1;
  rp->volume_lights = nullptr;
  rp->mirror_face = -1;
  rp->num_mirror_faces = 0;
  rp->mirror_faces_list = nullptr;
  rp->room_change_flags = 0;
  rp->wind = vector3{};
  rp->num_faces = nfaces;
  rp->num_verts = nverts;
  rp->num_portals = nportals;
  rp->last_render_time = 0;
  rp->fog_depth = 100.0f;
  rp->fog_r = rp->fog_g = rp->fog_b = 1.0f;
  rp->faces = nfaces ? static_cast<face *>(mem_malloc(sizeof(face) * nfaces)) : nullptr;
  rp->verts = nverts ? static_cast<vector3 *>(mem_malloc(sizeof(vector3) * nverts)) : nullptr;
  rp->verts4 = nullptr;
  rp->portals = nportals ? static_cast<portal *>(mem_malloc(sizeof(portal) * nportals)) : nullptr;
  rp->pulse_time = 0;
  rp->pulse_offset = 0;
  rp->ambient_sound = -1;
  rp->name.clear();
  rp->doorway_data = nullptr;
  rp->env_reverb = 0;
  rp->damage = 0.0f;
  rp->damage_type = 0;
  rp->bn_info.num_nodes = 0;
  rp->bn_info.nodes = nullptr;
  rp->used = 1;
}

void FreeRoom(room *rp) {
  if (rp == nullptr)
    return;
  if (rp->faces) {
    for (int i = 0; i < rp->num_faces; i++)
      FreeRoomFace(&rp->faces[i]);
    mem_free(rp->faces);
    rp->faces = nullptr;
  }
  if (rp->verts) {
    mem_free(rp->verts);
    rp->verts = nullptr;
  }
  if (rp->verts4) {
    mem_free(rp->verts4);
    rp->verts4 = nullptr;
  }
  if (rp->portals) {
    mem_free(rp->portals);
    rp->portals = nullptr;
  }
  if (rp->mirror_faces_list) {
    mem_free(rp->mirror_faces_list);
    rp->mirror_faces_list = nullptr;
  }
  if (rp->bn_info.nodes) {
    mem_free(rp->bn_info.nodes);
    rp->bn_info.nodes = nullptr;
  }
  rp->used = 0;
}

void InitRoomFace(face *fp, int nverts) {
  fp->flags = 0;
  fp->num_verts = nverts;
  fp->portal_num = -1;
  fp->tmap = 0;
  fp->lmi_handle = 0;
  fp->special_handle = 0;
  fp->light_multiple = 4;
  fp->normal = vector3{};
  fp->min_xyz = vector3{};
  fp->max_xyz = vector3{};
  fp->renderframe = 0;
  fp->face_verts = nverts ? static_cast<int16_t *>(mem_malloc(sizeof(int16_t) * nverts)) : nullptr;
  fp->face_uvls = nverts ? static_cast<roomUVL *>(mem_malloc(sizeof(roomUVL) * nverts)) : nullptr;
  for (int i = 0; i < nverts; i++)
    fp->face_uvls[i].alpha = 255;
}

void FreeRoomFace(face *fp) {
  if (fp == nullptr)
    return;
  if (fp->face_verts) {
    mem_free(fp->face_verts);
    fp->face_verts = nullptr;
  }
  if (fp->face_uvls) {
    mem_free(fp->face_uvls);
    fp->face_uvls = nullptr;
  }
  fp->num_verts = 0;
}

// Approximate face normal from the first non-degenerate triangle winding.
bool ComputeFaceNormal(room *rp, int facenum) {
  if (facenum < 0 || facenum >= rp->num_faces)
    return false;
  face *fp = &rp->faces[facenum];
  if (fp->num_verts < 3)
    return false;
  const vector3 *v0 = &rp->verts[fp->face_verts[0]];
  const vector3 *v1 = &rp->verts[fp->face_verts[1]];
  const vector3 *v2 = &rp->verts[fp->face_verts[2]];
  vector3 a = *v1 - *v0;
  vector3 b = *v2 - *v0;
  fp->normal = vector3{a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
  const scalar mag = fp->normal.mag();
  if (mag < 0.0001f)
    return false;
  fp->normal = fp->normal * (1.0f / mag);
  return true;
}

bool ComputeNormal(vector3 *normal, int num_verts, short *vertnum_list, vector3 *verts) {
  if (num_verts < 3 || verts == nullptr || vertnum_list == nullptr)
    return false;
  const vector3 *v0 = &verts[vertnum_list[0]];
  const vector3 *v1 = &verts[vertnum_list[1]];
  const vector3 *v2 = &verts[vertnum_list[2]];
  vector3 a = *v1 - *v0;
  vector3 b = *v2 - *v0;
  *normal = vector3{a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
  return true;
}

void ComputePortalCenter(vector3 *center, room *rp, int portal) { *center = {}; }
void ComputeCenterPointOnFace(vector3 *center, room *rp, int facenum) { *center = {}; }
void GetIJ(const vector3 *normal, int *ii, int *jj) {
  if (normal == nullptr || ii == nullptr || jj == nullptr) {
    if (ii)
      *ii = 0;
    if (jj)
      *jj = 1;
    return;
  }
  // Project onto 2d: find the dominant axis of the surface normal.
  if (fabs(normal->x()) > fabs(normal->y()))
    if (fabs(normal->x()) > fabs(normal->z())) {
      if (normal->x() > 0) {
        *ii = 2;
        *jj = 1;
      } else {
        *ii = 1;
        *jj = 2;
      }
    } else {
      if (normal->z() > 0) {
        *ii = 1;
        *jj = 0;
      } else {
        *ii = 0;
        *jj = 1;
      }
    }
  else if (fabs(normal->y()) > fabs(normal->z())) {
    if (normal->y() > 0) {
      *ii = 0;
      *jj = 2;
    } else {
      *ii = 2;
      *jj = 0;
    }
  } else {
    if (normal->z() > 0) {
      *ii = 1;
      *jj = 0;
    } else {
      *ii = 0;
      *jj = 1;
    }
  }
}
void GetVolumeSizeOfRoom(room *rp, int *w, int *h, int *d) { if (w) *w = 0; if (h) *h = 0; if (d) *d = 0; }
void FreeAllRooms() {
  for (int i = 0; i < MAX_ROOMS; i++) {
    if (Rooms[i].used)
      FreeRoom(&Rooms[i]);
  }
}

// Zeroes the whole room table and selection globals used by the editor.
void InitRooms() {
  for (int i = 0; i < MAX_ROOMS; i++) {
    if (Rooms[i].used) {
      FreeRoom(&Rooms[i]);
    }
  }
  Highest_room_index = -1;
  Curroomp = nullptr;
  Markedroomp = nullptr;
  Markedface = -1;
  Curface = -1;
  Curvert = -1;
  Curedge = -1;
  Curportal = -1;
}
