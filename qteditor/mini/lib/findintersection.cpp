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

// Mini implementation of find_vector_intersection (FVI).
//
// Replaces the win32 Descent3Core FVI.  Traces a segment p0 -> p1 through the
// mine, crossing portals and stopping at the first wall.  Unlike the full
// engine version this is a segment (radius 0) tracer: object/terrain
// collisions and sphere extents are not modelled, which is adequate for the
// editor (object dragging, point casts, LOS checks, slew room tracking).

#include "findintersection.h"

#include <cmath>
#include <cstring>

#include "room.h"
#include "vecmat.h"

namespace {

// True when pnt (which lies on the plane of face f) is inside the face polygon.
bool pointInsideFace(const room *rp, int faceIndex, const vector &pnt) {
  const face *fp = &rp->faces[faceIndex];
  const int n = fp->num_verts;
  if (n < 3)
    return false;

  // Project onto the 2D plane of the face (drop the dominant normal axis).
  const float ax = std::fabs(fp->normal.x());
  const float ay = std::fabs(fp->normal.y());
  const float az = std::fabs(fp->normal.z());
  const bool dropX = (ax >= ay && ax >= az);

  float sx[64] = {};
  float sy[64] = {};
  for (int v = 0; v < n; ++v) {
    const vector rel = rp->verts[fp->face_verts[v]] - pnt;
    if (dropX) {
      sx[v] = rel.y();
      sy[v] = rel.z();
    } else if (ay >= az) {
      sx[v] = rel.x();
      sy[v] = rel.z();
    } else {
      sx[v] = rel.x();
      sy[v] = rel.y();
    }
  }

  bool inside = false;
  for (int i = 0, j = n - 1; i < n; j = i++) {
    if ((sy[i] > 0.0f) != (sy[j] > 0.0f) &&
        (0.0f < (sx[j] - sx[i]) * (0.0f - sy[i]) / (sy[j] - sy[i]) + sx[i]))
      inside = !inside;
  }
  return inside;
}

// Parametric t in [0,1] where the segment crosses the face plane.
bool facePlaneIntersect(const vector &p0, const vector &seg, const vector &v0, const vector &normal, float &tOut) {
  const float denom = vm_DotProduct(&normal, &seg);
  if (std::fabs(denom) < 1e-6f)
    return false;
  const float t = (vm_DotProduct(&normal, &v0) - vm_DotProduct(&normal, &p0)) / denom;
  if (!(t > 0.0f) || !(t < 1.0f))
    return false;
  tOut = t;
  return true;
}

// Strictly-inside test: a point is inside a room when it is on the interior
// side of every face plane.
bool pointInsideRoom(const room *rp, const vector &pnt) {
  for (int f = 0; f < rp->num_faces; ++f) {
    const face *fp = &rp->faces[f];
    if (fp->num_verts < 3)
      continue;
    const vector rel = pnt - rp->verts[fp->face_verts[0]];
    if (vm_DotProduct(&fp->normal, &rel) > 0.0f)
      return false;
  }
  return true;
}

} // namespace

int fvi_FindIntersection(fvi_query *fq, fvi_info *ans, bool no_subdivision) {
  (void)no_subdivision;
  memset(ans, 0, sizeof(fvi_info));
  ans->hit_face[0] = -1;
  ans->hit_object[0] = -1;

  const vector p0 = *fq->p0;
  const vector p1 = *fq->p1;
  const vector seg = p1 - p0;
  const float segLen = vm_GetMagnitude(&seg);

  if (fq->startroom < 0 || fq->startroom > Highest_room_index) {
    // Starting outside the mine: treat as a free move, but report the room the
    // endpoint landed in, if any.
    for (int r = 0; r <= Highest_room_index; ++r) {
      if (Rooms[r].used && pointInsideRoom(&Rooms[r], p1)) {
        ans->hit_pnt = p1;
        ans->hit_room = r;
        ans->hit_dist = segLen;
        return HIT_NONE;
      }
    }
    ans->hit_pnt = p1;
    ans->hit_room = fq->startroom;
    ans->hit_dist = segLen;
    return HIT_NONE;
  }

  if (segLen < 1e-6f) {
    ans->hit_pnt = p0;
    ans->hit_room = fq->startroom;
    ans->hit_dist = 0.0f;
    return HIT_NONE;
  }

  const bool ignoreWalls = (fq->flags & FQ_IGNORE_WALLS) != 0;
  const bool solidPortals = (fq->flags & FQ_SOLID_PORTALS) != 0;

  int roomIdx = fq->startroom;
  float rayT = 0.0f;
  ans->hit_room = roomIdx;
  int roomCount = 0;
  if (roomCount < MAX_FVI_SEGS)
    ans->roomlist[roomCount++] = roomIdx;

  for (int iter = 0; iter < MAX_FVI_SEGS; ++iter) {
    if (roomIdx < 0 || roomIdx > Highest_room_index) {
      ans->hit_pnt = p1;
      ans->n_rooms = roomCount;
      return HIT_NONE;
    }
    const room *rp = &Rooms[roomIdx];
    if (!rp->used) {
      ans->hit_pnt = p1;
      ans->n_rooms = roomCount;
      return HIT_NONE;
    }

    // Nearest wall and nearest crossable portal ahead of rayT in this room.
    float bestWallT = 2.0f;
    int bestWallFace = -1;
    float bestPortalT = 2.0f;
    int bestPortalFace = -1;
    for (int f = 0; f < rp->num_faces; ++f) {
      const face *fp = &rp->faces[f];
      const int nv = fp->num_verts;
      if (nv < 3)
        continue;
      const vector v0 = rp->verts[fp->face_verts[0]];
      float t = 0.0f;
      if (!facePlaneIntersect(p0, seg, v0, fp->normal, t))
        continue;
      if (t <= rayT + 1e-5f)
        continue;
      const vector pnt = p0 + seg * t;
      if (!pointInsideFace(rp, f, pnt))
        continue;
      const bool isPortal = (fp->portal_num >= 0);
      if (isPortal && !solidPortals) {
        if (t < bestPortalT) {
          bestPortalT = t;
          bestPortalFace = f;
        }
      } else {
        if (t < bestWallT) {
          bestWallT = t;
          bestWallFace = f;
        }
      }
    }

    if (!ignoreWalls && bestWallFace >= 0 && bestWallT <= bestPortalT) {
      // Blocked by a wall (or a solid portal).
      ans->hit_pnt = p0 + seg * bestWallT;
      ans->hit_room = roomIdx;
      ans->hit_face[0] = bestWallFace;
      ans->hit_dist = bestWallT * segLen;
      ans->hit_wallnorm[0] = rp->faces[bestWallFace].normal;
      ans->num_hits = 1;
      ans->n_rooms = roomCount;
      ans->hit_type[0] = HIT_WALL;
      ans->hit_face_pnt[0] = ans->hit_pnt;
      ans->hit_face_room[0] = roomIdx;
      ans->hit_face[0] = bestWallFace;
      return HIT_WALL;
    }

    if (bestPortalFace >= 0) {
      const portal *pp = &rp->portals[rp->faces[bestPortalFace].portal_num];
      roomIdx = pp->croom;
      rayT = bestPortalT;
      if (roomCount < MAX_FVI_SEGS)
        ans->roomlist[roomCount++] = roomIdx;
      ans->hit_room = roomIdx;
      continue;
    }

    // Reached the endpoint without obstruction.
    ans->hit_pnt = p1;
    ans->hit_room = roomIdx;
    ans->hit_dist = segLen;
    ans->n_rooms = roomCount;
    return HIT_NONE;
  }

  ans->hit_pnt = p1;
  ans->n_rooms = roomCount;
  return HIT_NONE;
}