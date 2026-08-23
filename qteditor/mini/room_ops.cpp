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

// Room operations ported from editor/Erooms.cpp, editor/HRoom.cpp,
// editor/RoomUVs.cpp, and editor/rotateroom.cpp.
//
// The editor library is not linked to the Qt port, so these functions are
// re-implemented here with the same semantics.  Only the editor-specific
// helpers (LevelTexIncrementTexture, etc.) are stubbed/skipped.

#include "editor_room_state.h"

#include <QtGlobal>
#include <QMessageBox>

#include "logger/log.h"

#include "d3edit.h"
#include "door.h"
#include "doorway.h"
#include "mem/mem.h"

#include "object.h"
#include "polymodel.h"

#include "room.h"
#include "special_face.h"
#include "trigger.h"
#include "vecmat.h"

#include <cassert>
#include <cmath>
#include <cmath>
#include <cstdio>
#include <cstring>

// ============================================================================
// DeleteTrigger — editor-guarded in Descent3/trigger.cpp, not linked.
// Reimplemented here for editor use.
// ============================================================================
static void EditorDeleteTrigger(int trig_num) {
  trigger *tp = &Triggers[trig_num];
  room *rp = &Rooms[tp->roomnum];
  face *fp = &rp->faces[tp->facenum];
  fp->flags &= ~FF_HAS_TRIGGER;
  for (int i = trig_num; i < Num_triggers - 1; i++)
    Triggers[i] = Triggers[i + 1];
  Num_triggers--;
}

static void EditorDeleteTriggerByRoomFace(int roomnum, int facenum) {
  for (int i = 0; i < Num_triggers; i++) {
    if (Triggers[i].roomnum == roomnum && Triggers[i].facenum == facenum) {
      EditorDeleteTrigger(i);
      return;
    }
  }
}

// ============================================================================
// Epsilon constants from editor/Erooms.h
// ============================================================================
#define POINT_TO_POINT_EPSILON 0.1f
#define POINT_TO_PLANE_EPSILON 0.1f
#define CONCAVITY_TOLERANCE 0.05f

// ============================================================================
// Helper: NormalsAreSame
// ============================================================================
static inline bool NormalsAreSame(const vector *n0, const vector *n1) {
  scalar d = vm_Dot3Product(*n0, *n1);
  return (d > 0.999f);
}

// ============================================================================
// ReInitRoomFace — editor/Erooms.cpp:1336
// Changes the number of verts in a face.  Frees and reallocates face_verts &
// face_uvls arrays.
// ============================================================================
void ReInitRoomFace(face *fp, int nverts) {
  Q_ASSERT(nverts != 0);

  fp->num_verts = nverts;

  mem_free(fp->face_verts);
  mem_free(fp->face_uvls);

  fp->face_verts = (int16_t *)mem_malloc(nverts * sizeof(*fp->face_verts));
  Q_ASSERT(fp->face_verts != NULL);
  fp->face_uvls = mem_rmalloc<roomUVL>(nverts);
  Q_ASSERT(fp->face_uvls != NULL);
}

// ============================================================================
// RoomAddVertices — editor/Erooms.cpp:1394
// Create space for additional vertices in a room.
// ============================================================================
int RoomAddVertices(room *rp, int num_new_verts) {
  if (num_new_verts == 0)
    return 0;

  auto newverts = mem_rmalloc<vector>(rp->num_verts + num_new_verts);
  Q_ASSERT(newverts != NULL);

  for (int i = 0; i < rp->num_verts; i++)
    newverts[i] = rp->verts[i];

  mem_free(rp->verts);
  rp->verts = newverts;
  rp->num_verts += num_new_verts;

  return (rp->num_verts - num_new_verts);
}

// ============================================================================
// RoomAddFaces — editor/Erooms.cpp:1421
// Create space for additional faces in a room.
// ============================================================================
int RoomAddFaces(room *rp, int num_new_faces) {
  if (num_new_faces == 0)
    return 0;

  auto newfaces = mem_rmalloc<face>(rp->num_faces + num_new_faces);
  Q_ASSERT(newfaces != NULL);

  for (int i = 0; i < rp->num_faces; i++)
    newfaces[i] = rp->faces[i];

  mem_free(rp->faces);
  rp->faces = newfaces;
  rp->num_faces += num_new_faces;

  if (rp->num_bbf_regions) {
    for (int i = 0; i < rp->num_bbf_regions; i++)
      mem_free(rp->bbf_list[i]);
    mem_free(rp->bbf_list);
    mem_free(rp->num_bbf);
    mem_free(rp->bbf_list_min_xyz);
    mem_free(rp->bbf_list_max_xyz);
    mem_free(rp->bbf_list_sector);
    rp->num_bbf_regions = 0;
  }

  return (rp->num_faces - num_new_faces);
}

// ============================================================================
// ResetRoomFaceNormals — editor/Erooms.cpp:1154
// ============================================================================
bool ResetRoomFaceNormals(room *rp) {
  int bad_normals = 0;

  for (int i = 0; i < rp->num_faces; i++)
    if (!ComputeFaceNormal(rp, i))
      bad_normals++;

  if (bad_normals > 0) {
    LOG_WARNING("Warning: Room %d has %d bad or low-precision normals\n", ROOMNUM(rp), bad_normals);
    return false;
  }
  return true;
}

// ============================================================================
// CopyFace — editor/Erooms.cpp:1171
// Copies the contents of one face to another.  Sets portal_num to -1.
// ============================================================================
void CopyFace(face *dfp, face *sfp) {
  InitRoomFace(dfp, sfp->num_verts);

  dfp->flags = sfp->flags;
  dfp->portal_num = -1;
  dfp->normal = sfp->normal;
  dfp->tmap = sfp->tmap;
  dfp->light_multiple = sfp->light_multiple;

  dfp->flags &= ~FF_LIGHTMAP;
  dfp->flags &= ~FF_HAS_TRIGGER;

  for (int i = 0; i < sfp->num_verts; i++) {
    dfp->face_verts[i] = sfp->face_verts[i];
    dfp->face_uvls[i] = sfp->face_uvls[i];
  }
}

// ============================================================================
// CopyFaceFlags — editor/Erooms.cpp:2813
// Copy goal face flags from one face to another.
// ============================================================================
void CopyFaceFlags(face *dfp, face *sfp) {
  dfp->flags = 0;
  if (sfp->flags & FF_GOALFACE)
    dfp->flags |= FF_GOALFACE;
}

// ============================================================================
// CopyRoom — editor/Erooms.cpp:1941
// Deep-copies a room (verts, faces, doorway, flags).  Portals are not copied.
// ============================================================================
void CopyRoom(room *destp, room *srcp) {
  InitRoom(destp, srcp->num_verts, srcp->num_faces, 0);

  for (int i = 0; i < destp->num_faces; i++)
    CopyFace(&destp->faces[i], &srcp->faces[i]);

  for (int i = 0; i < destp->num_verts; i++)
    destp->verts[i] = srcp->verts[i];

  if (srcp->doorway_data) {
    destp->doorway_data = mem_rmalloc<doorway>();
    *destp->doorway_data = *srcp->doorway_data;
  }

  destp->flags = srcp->flags;

  // Editor-lighting arrays (Room_multiplier, Room_ambience_*) live in the
  // editor lib and are not linked; skip.
}

// ============================================================================
// FaceIsPlanar — editor/Erooms.cpp:1194
// ============================================================================
bool FaceIsPlanar(int nv, int16_t *face_verts, vector *normal, vector *verts) {
  if (nv == 3)
    return true;

  float average_d = 0;
  for (int v = 0; v < nv; v++)
    average_d += vm_Dot3Product(verts[face_verts[v]], *normal);
  average_d /= nv;

  for (int v = 0; v < nv; v++) {
    float d = vm_Dot3Product(verts[face_verts[v]], *normal);
    if (fabs(d - average_d) > POINT_TO_PLANE_EPSILON)
      return false;
  }
  return true;
}

// ============================================================================
// CheckFaceConcavity — editor/Erooms.cpp:1108
// Returns the index of the vertex causing concavity, or -1 if convex.
// ============================================================================
int CheckFaceConcavity(int num_verts, int16_t *face_verts, vector *normal, vector *verts) {
  int ii, jj;
  float i0, j0, i1, j1;
  float *v0, *v1;

  GetIJ(normal, &ii, &jj);

  v0 = (float *)&verts[face_verts[num_verts - 1]];
  v1 = (float *)&verts[face_verts[0]];
  i1 = v1[ii] - v0[ii];
  j1 = v1[jj] - v0[jj];

  for (int vn = 0; vn < num_verts; vn++) {
    v0 = v1;
    i0 = i1;
    j0 = j1;

    v1 = (float *)&verts[face_verts[(vn + 1) % num_verts]];
    i1 = v1[ii] - v0[ii];
    j1 = v1[jj] - v0[jj];

    float dot = (-j0 * i1) + (i0 * j1);
    dot /= sqrt(i0 * i0 + j0 * j0) * sqrt(i1 * i1 + j1 * j1);

    if (dot > CONCAVITY_TOLERANCE)
      return vn;
  }

  return -1;
}

// ============================================================================
// FindSharedEdge — editor/Erooms.cpp:1543
// Finds a shared edge between two faces.  Returns true if found.
// ============================================================================
bool FindSharedEdge(face *fp0, face *fp1, int *vn0, int *vn1) {
  for (int i = 0; i < fp0->num_verts; i++) {
    int a0 = fp0->face_verts[i];
    int b0 = fp0->face_verts[(i + 1) % fp0->num_verts];

    for (int j = 0; j < fp1->num_verts; j++) {
      int a1 = fp1->face_verts[j];
      int b1 = fp1->face_verts[(j + 1) % fp1->num_verts];

      if ((a0 == b1) && (b0 == a1)) {
        *vn0 = i;
        *vn1 = j;
        return true;
      }
    }
  }
  return false;
}

// ============================================================================
// DeleteRoomFace — editor/Erooms.cpp:1693
// Simplified version: deletes a face from a room, adjusts portal/trigger
// face indices.  Skips texture accounting (LevelTexDecrementTexture).
// ============================================================================
void DeleteRoomFace(room *rp, int facenum) {
  int f, i, t;

  if (rp->faces[facenum].flags & FF_HAS_TRIGGER)
    EditorDeleteTriggerByRoomFace(ROOMNUM(rp), facenum);

  for (int p = 0; p < rp->num_portals; p++) {
    portal *pp = &rp->portals[p];
    Q_ASSERT(pp->portal_face != facenum);
    if (pp->portal_face > facenum)
      pp->portal_face--;
  }

  for (t = 0; t < Num_triggers; t++) {
    trigger *tp = &Triggers[t];
    if (tp->roomnum == ROOMNUM(rp)) {
      Q_ASSERT(tp->facenum != facenum);
      if (tp->facenum > facenum)
        tp->facenum--;
    }
  }

  face *newfaces = mem_rmalloc<face>(rp->num_faces - 1);
  Q_ASSERT(newfaces != NULL);

  for (f = 0; f < facenum; f++)
    newfaces[f] = rp->faces[f];
  for (f = facenum + 1; f < rp->num_faces; f++)
    newfaces[f - 1] = rp->faces[f];

  FreeRoomFace(&rp->faces[facenum]);
  mem_free(rp->faces);
  rp->faces = newfaces;
  rp->num_faces--;

  if (rp == Curroomp) {
    if (Curface == rp->num_faces)
      Curface = rp->num_faces - 1;
    if (Markedface == rp->num_faces)
      Markedface = rp->num_faces - 1;
  }

  if (rp->num_bbf_regions) {
    for (i = 0; i < rp->num_bbf_regions; i++)
      mem_free(rp->bbf_list[i]);
    mem_free(rp->bbf_list);
    mem_free(rp->num_bbf);
    mem_free(rp->bbf_list_min_xyz);
    mem_free(rp->bbf_list_max_xyz);
    mem_free(rp->bbf_list_sector);
    rp->num_bbf_regions = 0;
  }
}

// ============================================================================
// DeleteRoomPortal — editor/Erooms.cpp:1787
// Deletes a portal from a room (does not delete the connecting side).
// ============================================================================
void DeleteRoomPortal(room *rp, int portalnum) {
  portal *pp = &rp->portals[portalnum];
  face *fp = &rp->faces[pp->portal_face];

  Q_ASSERT(fp->portal_num == portalnum);
  fp->portal_num = -1;

  for (int p = portalnum + 1; p < rp->num_portals; p++) {
    portal *tp = &rp->portals[p];
    Q_ASSERT(rp->faces[tp->portal_face].portal_num == p);
    rp->faces[tp->portal_face].portal_num--;
    if (tp->croom != -1)
      Rooms[tp->croom].portals[tp->cportal].cportal--;
  }

  portal *newportals;
  if (rp->num_portals == 1)
    newportals = NULL;
  else {
    newportals = mem_rmalloc<portal>(rp->num_portals - 1);
    Q_ASSERT(newportals != NULL);
  }

  for (int p = 0; p < portalnum; p++)
    newportals[p] = rp->portals[p];
  for (int p = portalnum + 1; p < rp->num_portals; p++)
    newportals[p - 1] = rp->portals[p];

  mem_free(rp->portals);
  rp->portals = newportals;
  rp->num_portals--;
}

// ============================================================================
// AddPortal — editor/Erooms.cpp:1975
// Adds a new portal to a room.  Returns the portal number.
// ============================================================================
int AddPortal(room *rp) {
  auto newlist = mem_rmalloc<portal>(rp->num_portals + 1);

  if (rp->num_portals) {
    for (int i = 0; i < rp->num_portals; i++)
      newlist[i] = rp->portals[i];
    mem_free(rp->portals);
  }

  rp->portals = newlist;
  rp->portals[rp->num_portals].flags = 0;
  rp->portals[rp->num_portals].bnode_index = -1;

  return rp->num_portals++;
}

// ============================================================================
// LinkRooms / LinkRoomsSimple — editor/Erooms.cpp:2001
// Links two rooms by creating portals on the specified faces.
// ============================================================================
void LinkRooms(room *roomlist, int room0, int face0, int room1, int face1) {
  room *rp0 = &roomlist[room0];
  room *rp1 = &roomlist[room1];

  Q_ASSERT(rp0->faces[face0].portal_num == -1);
  Q_ASSERT(rp1->faces[face1].portal_num == -1);

  int pn0 = AddPortal(rp0);
  int pn1 = AddPortal(rp1);

  rp0->portals[pn0].croom = room1;
  rp0->portals[pn0].cportal = pn1;
  rp1->portals[pn1].croom = room0;
  rp1->portals[pn1].cportal = pn0;

  rp0->portals[pn0].portal_face = face0;
  rp1->portals[pn1].portal_face = face1;

  rp0->faces[face0].portal_num = pn0;
  rp1->faces[face1].portal_num = pn1;
}

// ============================================================================
// DeletePortalPair — editor/HRoom.cpp:2186
// Deletes a portal pair (both sides of a connection).
// ============================================================================
void DeletePortalPair(room *rp, int portalnum) {
  int roomnum = ROOMNUM(rp);
  portal *pp = &rp->portals[portalnum];
  int croom = pp->croom, cportal = pp->cportal;

  if ((Rooms[croom].portals[cportal].croom != roomnum) || (Rooms[croom].portals[cportal].cportal != portalnum)) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot delete this portal: the connecting portal does not point back at it.");
    return;
  }

  if (rp->faces[pp->portal_face].portal_num != portalnum) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot delete this portal: its face does not point back at it.");
    return;
  }

  DeleteRoomPortal(rp, portalnum);
  DeleteRoomPortal(&Rooms[croom], cportal);

  EditorStatus("Deleted room %d portal %d and room %d portal %d", roomnum, portalnum, croom, cportal);

  World_changed = true;
}

// ============================================================================
// FlipFace — editor/HRoom.cpp:2211
// Flips the winding order of a face and recomputes its normal.
// ============================================================================
void FlipFace(room *rp, int facenum) {
  face *fp = &rp->faces[facenum];

  Q_ASSERT(fp->portal_num == -1);

  for (int i = 0; i < fp->num_verts / 2; i++) {
    int v = fp->face_verts[i];
    fp->face_verts[i] = fp->face_verts[fp->num_verts - 1 - i];
    fp->face_verts[fp->num_verts - 1 - i] = v;

    roomUVL uvl = fp->face_uvls[i];
    fp->face_uvls[i] = fp->face_uvls[fp->num_verts - 1 - i];
    fp->face_uvls[fp->num_verts - 1 - i] = uvl;
  }

  if (!ComputeFaceNormal(rp, facenum))
    Q_ASSERT(false);

  World_changed = true;
  EditorStatus("Room %d face %d flipped.", ROOMNUM(rp), facenum);
}

// ============================================================================
// AssignUVsToFace — editor/RoomUVs.cpp:75
// Given u,v coordinates at two vertices, assign u,v to all other vertices.
// ============================================================================
void AssignUVsToFace(room *rp, int facenum, roomUVL *uva, roomUVL *uvb, int va, int vb) {
  face *fp = &rp->faces[facenum];
  int nv = fp->num_verts;
  int vlo, vhi;
  vector fvec, rvec, tvec;
  roomUVL ruvmag, fuvmag, uvlo, uvhi;
  float fmag, mag01;

  Q_ASSERT((va < nv) && (vb < nv));
  Q_ASSERT((abs(va - vb) == 1) || (abs(va - vb) == nv - 1));

  if (va == ((vb + 1) % nv)) {
    vlo = vb;
    vhi = va;
    uvlo = *uvb;
    uvhi = *uva;
  } else {
    vlo = va;
    vhi = vb;
    uvlo = *uva;
    uvhi = *uvb;
  }

  Q_ASSERT(((vlo + 1) % nv) == vhi);
  fp->face_uvls[vlo] = uvlo;
  fp->face_uvls[vhi] = uvhi;

  fmag = sqrt((uvhi.v - uvlo.v) * (uvhi.v - uvlo.v) + (uvhi.u - uvlo.u) * (uvhi.u - uvlo.u));
  if (fmag < 0.001f) {
    ruvmag.u = 256.0f;
    ruvmag.v = 256.0f;
    fuvmag.u = 256.0f;
    fuvmag.v = 256.0f;
  } else {
    ruvmag.u = uvhi.v - uvlo.v;
    ruvmag.v = uvlo.u - uvhi.u;
    fuvmag.u = uvhi.u - uvlo.u;
    fuvmag.v = uvhi.v - uvlo.v;
  }

  vector *vv0 = &rp->verts[fp->face_verts[vlo]];
  vector *vv1 = &rp->verts[fp->face_verts[vhi]];

  fvec = *vv1 - *vv0;
  mag01 = vm_NormalizeVector(&fvec);

  if (mag01 < 0.001f) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "U, V bogosity in room #%i, probably on face #%i.", ROOMNUM(rp), facenum);
    return;
  }

  rvec = vm_Cross3Product(fvec, fp->normal);

  ruvmag.u /= mag01;
  ruvmag.v /= mag01;
  fuvmag.u /= mag01;
  fuvmag.v /= mag01;

  for (int i = 1; i < nv - 1; i++) {
    int fv = (vhi + i) % nv;
    int rv = fp->face_verts[fv];

    tvec = rp->verts[rv] - *vv0;

    scalar rproj = vm_Dot3Product(tvec, rvec);
    scalar fproj = vm_Dot3Product(tvec, fvec);

    fp->face_uvls[fv].u = uvlo.u + (ruvmag.u * rproj) + (fuvmag.u * fproj);
    fp->face_uvls[fv].v = uvlo.v + (ruvmag.v * rproj) + (fuvmag.v * fproj);
  }
}

// ============================================================================
// AssignDefaultUVsToRoom — editor/Erooms.cpp:748
// ============================================================================
void AssignDefaultUVsToRoom(room *rp) {
  for (int i = 0; i < rp->num_faces; i++)
    AssignDefaultUVsToRoomFace(rp, i);
}

// ============================================================================
// FixConcaveFaces — editor/Erooms.cpp:1218
// Triangulates concave/nonplanar faces in a room.
// ============================================================================
void FixConcaveFaces(room *rp, int *facelist, int facecount) {
  for (int i = 0; i < facecount; i++) {
    face *fp = &rp->faces[facelist[i]];
    if (!FaceIsPlanar(fp->num_verts, fp->face_verts, &fp->normal, rp->verts)) {
      int concave_verts[MAX_VERTS_PER_FACE];
      int concave_count = rp->faces[facelist[i]].num_verts;
      int old_tmap = rp->faces[facelist[i]].tmap;
      int num_new_faces = concave_count - 3;
      Q_ASSERT(num_new_faces > 0);

      for (int t = 0; t < concave_count; t++)
        concave_verts[t] = rp->faces[facelist[i]].face_verts[t];

      int old_num_faces = rp->num_faces;
      int nfaces = rp->num_faces + num_new_faces;

      face *newfaces = mem_rmalloc<face>(nfaces);
      Q_ASSERT(newfaces != NULL);

      for (int t = 0; t < rp->num_faces; t++) {
        if (t != facelist[i]) {
          int nverts = rp->faces[t].num_verts;

          newfaces[t].face_verts = mem_rmalloc<int16_t>(nverts);
          newfaces[t].face_uvls = mem_rmalloc<roomUVL>(nverts);
          newfaces[t].normal = rp->faces[t].normal;
          newfaces[t].tmap = rp->faces[t].tmap;
          newfaces[t].flags = rp->faces[t].flags;
          newfaces[t].portal_num = rp->faces[t].portal_num;
          newfaces[t].num_verts = rp->faces[t].num_verts;
          newfaces[t].special_handle = BAD_SPECIAL_FACE_INDEX;

          for (int k = 0; k < nverts; k++) {
            newfaces[t].face_verts[k] = rp->faces[t].face_verts[k];
            newfaces[t].face_uvls[k] = rp->faces[t].face_uvls[k];
          }
        } else {
          newfaces[t].face_verts = mem_rmalloc<int16_t>(3);
          newfaces[t].face_uvls = mem_rmalloc<roomUVL>(3);
          newfaces[t].tmap = rp->faces[t].tmap;
          newfaces[t].flags = rp->faces[t].flags;
          newfaces[t].portal_num = rp->faces[t].portal_num;
          newfaces[t].num_verts = 3;
          newfaces[t].special_handle = BAD_SPECIAL_FACE_INDEX;

          for (int k = 0; k < 3; k++) {
            newfaces[t].face_verts[k] = rp->faces[t].face_verts[k];
            newfaces[t].face_uvls[k] = rp->faces[t].face_uvls[k];
          }

          if (!ComputeFaceNormal(rp, t))
            Q_ASSERT(false);
          newfaces[t].normal = rp->faces[t].normal;
        }
      }

      mem_free(rp->faces);
      rp->faces = newfaces;
      rp->num_faces = nfaces;

      if (rp->num_bbf_regions) {
        for (int j = 0; j < rp->num_bbf_regions; j++)
          mem_free(rp->bbf_list[j]);
        mem_free(rp->bbf_list);
        mem_free(rp->num_bbf);
        mem_free(rp->bbf_list_min_xyz);
        mem_free(rp->bbf_list_max_xyz);
        mem_free(rp->bbf_list_sector);
        rp->num_bbf_regions = 0;
      }

      for (int t = 0; t < num_new_faces; t++)
        InitRoomFace(&rp->faces[old_num_faces + t], 3);

      for (int t = 0; t < concave_count - 3; t++) {
        rp->faces[old_num_faces + t].face_verts[0] = concave_verts[0];
        rp->faces[old_num_faces + t].face_verts[1] = concave_verts[2 + t];
        rp->faces[old_num_faces + t].face_verts[2] = concave_verts[3 + t];
        rp->faces[old_num_faces + t].tmap = old_tmap;

        if (!ComputeFaceNormal(rp, old_num_faces + t))
          Q_ASSERT(false);

        AssignDefaultUVsToRoomFace(rp, old_num_faces + t);
      }
    }
  }

  if (!ResetRoomFaceNormals(rp))
    Q_ASSERT(false);
}

// ============================================================================
// CombineFaces — editor/HRoom.cpp:1552
// Merges two coplanar, convex faces that share an edge.
// ============================================================================
bool CombineFaces(room *rp, int face0, int face1) {
  face *fp0 = &rp->faces[face0], *fp1 = &rp->faces[face1];
  int nv0 = fp0->num_verts, nv1 = fp1->num_verts;
  int v0, v1;
  int16_t vertlist[MAX_VERTS_PER_FACE];
  roomUVL uvllist[MAX_VERTS_PER_FACE];
  int nv;

  Q_ASSERT(face0 != face1);

  if ((fp0->portal_num != -1) || (fp1->portal_num != -1)) {
    SetErrorMessage("You cannot combine portal faces.");
    return false;
  }

  if (!FindSharedEdge(fp0, fp1, &v0, &v1)) {
    SetErrorMessage("The faces do not share an edge.");
    return false;
  }

  int first0 = v0 + 1, n0 = nv0 - 1;
  int first1 = v1 + 1, n1 = nv1 - 1;

  while (1) {
    int check0 = (first0 + 1) % nv0, check1 = (first1 + n1 - 1) % nv1;
    if (fp0->face_verts[check0] != fp1->face_verts[check1])
      break;
    first0++;
    n0--;
    n1--;
  }
  while (1) {
    int check1 = (first1 + 1) % nv1, check0 = (first0 + n0 - 1) % nv0;
    if (fp0->face_verts[check0] != fp1->face_verts[check1])
      break;
    first1++;
    n1--;
    n0--;
  }

  nv = 0;
  for (int i = 0; i < n0; i++) {
    vertlist[nv] = fp0->face_verts[(first0 + i) % nv0];
    uvllist[nv] = fp0->face_uvls[(first0 + i) % nv0];
    nv++;
  }
  for (int i = 0; i < n1; i++) {
    vertlist[nv] = fp1->face_verts[(first1 + i) % nv1];
    uvllist[nv] = fp1->face_uvls[(first1 + i) % nv1];
    nv++;
  }

  vector new_normal;
  ComputeNormal(&new_normal, nv, vertlist, rp->verts);

  if (!FaceIsPlanar(nv, vertlist, &new_normal, rp->verts)) {
    SetErrorMessage("The new face would not be planar.");
    return false;
  }

  if (CheckFaceConcavity(nv, vertlist, &new_normal, rp->verts) != -1) {
    SetErrorMessage("The new face would be concave.");
    return false;
  }

  ReInitRoomFace(fp0, nv);
  for (int i = 0; i < nv; i++) {
    fp0->face_verts[i] = vertlist[i];
    fp0->face_uvls[i] = uvllist[i];
  }

  fp0->normal = new_normal;

  AssignUVsToFace(rp, face0, &fp0->face_uvls[0], &fp0->face_uvls[1], 0, 1);

  DeleteRoomFace(rp, face1);

  World_changed = true;
  return true;
}

// ============================================================================
// RotateRooms — editor/rotateroom.cpp:58
// Rotates the marked room (and all rooms connected to it) around the portal
// connecting it to the current room.
// ============================================================================
void RotateRooms(angle p, angle h, angle b) {
  int checkfaces[MAX_FACES_PER_ROOM];
  int checkcount = 0;
  matrix rotmat, roommat;
  vector rotpoint, portal_normal;
  int marked_portalnum = -1;
  int cur_portalnum = -1;

  if (Curroomp == Markedroomp || Markedroomp == NULL) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You do not have a valid room marked.");
    return;
  }

  for (int i = 0; i < Markedroomp->num_portals; i++) {
    if (Markedroomp->portals[i].croom == (Curroomp - Rooms)) {
      marked_portalnum = i;
      break;
    }
  }

  if (marked_portalnum == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "The marked room is not connected to the current room!");
    return;
  }

  for (int i = 0; i < Curroomp->num_portals; i++) {
    if (Curroomp->portals[i].croom == (Markedroomp - Rooms)) {
      cur_portalnum = i;
      break;
    }
  }

  if (cur_portalnum == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "The marked room is not connected to the current room!");
    return;
  }

  SaveRoomSelectedList();

  Curroomp->portals[cur_portalnum].croom = -1;
  SelectConnectedRooms(Curroomp - Rooms);
  Curroomp->portals[cur_portalnum].croom = Markedroomp - Rooms;

  if (IsRoomSelected(Markedroomp - Rooms)) {
    RestoreRoomSelectedList();
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot rotate: rooms connect back to base room.");
    return;
  }

  ComputePortalCenter(&rotpoint, Curroomp, cur_portalnum);
  vm_AnglesToMatrix(&rotmat, p, h, b);
  face *fp = &Curroomp->faces[Curroomp->portals[cur_portalnum].portal_face];
  ComputeNormal(&portal_normal, fp->num_verts, fp->face_verts, Curroomp->verts);
  portal_normal *= -1.0;

  vm_VectorToMatrix(&roommat, &portal_normal, NULL, NULL);
  rotmat = roommat * ~rotmat * ~roommat;

  for (int i = 0; i < N_selected_rooms; i++) {
    room *rp = &Rooms[Selected_rooms[i]];

    for (int v = 0; v < rp->num_verts; v++) {
      if (rp == Curroomp) {
        face *cfp = &Curroomp->faces[Curroomp->portals[cur_portalnum].portal_face];
        for (int t = 0; t < cfp->num_verts; t++)
          if (v == cfp->face_verts[t])
            goto skip_vert;
      }
      rp->verts[v] = ((rp->verts[v] - rotpoint) * rotmat) + rotpoint;
    skip_vert:;
    }

    for (int t = 0; t < rp->num_faces; t++)
      rp->faces[t].normal = rp->faces[t].normal * rotmat;
  }

  RestoreRoomSelectedList();

  fp = &Curroomp->faces[Curroomp->portals[cur_portalnum].portal_face];
  for (int i = 0; i < fp->num_verts; i++) {
    int checkvert = fp->face_verts[i];

    for (int t = 0; t < Curroomp->num_faces; t++) {
      for (int l = 0; l < Curroomp->num_portals; l++)
        if (Curroomp->portals[l].portal_face == t)
          goto skip_face;

      for (int v = 0; v < Curroomp->faces[t].num_verts; v++) {
        if (Curroomp->faces[t].face_verts[v] == checkvert) {
          int k;
          for (k = 0; k < checkcount; k++)
            if (checkfaces[k] == t)
              break;
          if (k == checkcount)
            checkfaces[checkcount++] = t;
        }
      }
    skip_face:;
    }
  }

  if (checkcount > 0)
    FixConcaveFaces(Curroomp, checkfaces, checkcount);

  World_changed = true;
}

// ============================================================================
// ConnectPortal — simplified wrapper to link two rooms via portal
// ============================================================================
void ConnectPortal(room *rp, int portal_num, int dest_room) {
  if (portal_num < 0 || portal_num >= rp->num_portals) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Invalid portal number.");
    return;
  }

  portal *pp = &rp->portals[portal_num];
  int dest_face = pp->portal_face;

  if (pp->croom != -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "This portal is already connected.");
    return;
  }

  room *destp = &Rooms[dest_room];
  if (destp->faces[dest_face].portal_num != -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Destination face is already a portal.");
    return;
  }

  LinkRooms(Rooms, ROOMNUM(rp), pp->portal_face, dest_room, dest_face);

  World_changed = true;
  EditorStatus("Connected room %d to room %d.", ROOMNUM(rp), dest_room);
}

// ============================================================================
// DetachPortal — disconnect a portal pair
// ============================================================================
void DetachPortal(room *rp, int portal_num) {
  if (portal_num < 0 || portal_num >= rp->num_portals) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Invalid portal number.");
    return;
  }

  portal *pp = &rp->portals[portal_num];
  if (pp->croom == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "This portal is not connected.");
    return;
  }

  DeletePortalPair(rp, portal_num);
}

// ============================================================================
// Polygon clipping pipeline — ported from editor/HRoom.cpp and
// editor/Erooms.cpp.  Used by AttachRoom to clip portal faces.
// ============================================================================

struct clip_vertex {
  vector vec;
  roomUVL uvl;
};

#define POINT_TO_EDGE_EPSILON 0.1f

static bool PointsAreSame(const vector *v0, const vector *v1) {
  return vm_VectorDistance(v0, v1) < POINT_TO_POINT_EPSILON;
}

static int CheckPointAgainstEdge(const vector *checkv, const vector *v0, const vector *v1, const vector *normal) {
  int ii, jj;
  GetIJ(normal, &ii, &jj);

  float edge_i = ((const float *)v1)[ii] - ((const float *)v0)[ii];
  float edge_j = ((const float *)v1)[jj] - ((const float *)v0)[jj];
  float edge_mag = sqrtf(edge_i * edge_i + edge_j * edge_j);

  float check_i = ((const float *)checkv)[ii] - ((const float *)v0)[ii];
  float check_j = ((const float *)checkv)[jj] - ((const float *)v0)[jj];

  float dot = ((-edge_j * check_i) + (edge_i * check_j)) / edge_mag;

  if (dot > POINT_TO_EDGE_EPSILON)
    return -1;
  else if (dot < -POINT_TO_EDGE_EPSILON)
    return 1;
  else
    return 0;
}

static void ClipEdge(const vector *normal, const clip_vertex *v0, const clip_vertex *v1,
                     const vector *v2, const vector *v3, clip_vertex *newv) {
  int ii, jj;
  GetIJ(normal, &ii, &jj);

  const float *vv0 = (const float *)&v0->vec;
  const float *vv1 = (const float *)&v1->vec;
  const float *vv2 = (const float *)v2;
  const float *vv3 = (const float *)v3;

  float k = ((vv2[jj] - vv0[jj]) * (vv3[ii] - vv2[ii]) - (vv2[ii] - vv0[ii]) * (vv3[jj] - vv2[jj])) /
            ((vv1[jj] - vv0[jj]) * (vv3[ii] - vv2[ii]) - (vv1[ii] - vv0[ii]) * (vv3[jj] - vv2[jj]));

  if (k < 0.0f) {
    Q_ASSERT((vm_VectorDistance(&v1->vec, &v0->vec) * -k) < POINT_TO_EDGE_EPSILON);
    k = 0.0f;
  }
  if (k > 1.0f) {
    Q_ASSERT((vm_VectorDistance(&v1->vec, &v0->vec) * (k - 1.0f)) < POINT_TO_EDGE_EPSILON);
    k = 1.0f;
  }
  Q_ASSERT((k >= 0) && (k <= 1.0f));

  newv->vec = v0->vec + (v1->vec - v0->vec) * k;
  newv->uvl.u = v0->uvl.u + (v1->uvl.u - v0->uvl.u) * k;
  newv->uvl.v = v0->uvl.v + (v1->uvl.v - v0->uvl.v) * k;
  newv->uvl.alpha = v0->uvl.alpha + (v1->uvl.alpha - v0->uvl.alpha) * k;
}

// ============================================================================
// AddVertToFace — insert a vertex into a face after a given position
// ============================================================================
static void AddVertToFace(room *rp, int facenum, int new_v, int after_v) {
  face *fp = &rp->faces[facenum];
  int old_verts[MAX_VERTS_PER_FACE];
  roomUVL old_uvls[MAX_VERTS_PER_FACE];
  int t;

  for (t = 0; t < fp->num_verts; t++) {
    old_verts[t] = fp->face_verts[t];
    old_uvls[t] = fp->face_uvls[t];
  }

  ReInitRoomFace(fp, fp->num_verts + 1);

  for (t = 0; t <= after_v; t++) {
    fp->face_verts[t] = old_verts[t];
    fp->face_uvls[t] = old_uvls[t];
  }

  fp->face_verts[t++] = new_v;

  for (; t < fp->num_verts; t++) {
    fp->face_verts[t] = old_verts[t - 1];
    fp->face_uvls[t] = old_uvls[t - 1];
  }

  // Compute UV for the new vert by interpolating along the edge
  vector *va = &rp->verts[fp->face_verts[after_v]];
  vector *vb = &rp->verts[fp->face_verts[(after_v + 2) % fp->num_verts]];
  vector *vn = &rp->verts[fp->face_verts[after_v + 1]];
  roomUVL *uva = &fp->face_uvls[after_v];
  roomUVL *uvb = &fp->face_uvls[(after_v + 2) % fp->num_verts];

  float k = vm_VectorDistance(vn, va) / vm_VectorDistance(vb, va);

  fp->face_uvls[after_v + 1].u = uva->u + k * (uvb->u - uva->u);
  fp->face_uvls[after_v + 1].v = uva->v + k * (uvb->v - uva->v);
  fp->face_uvls[after_v + 1].u2 = uva->u2 + k * (uvb->u2 - uva->u2);
  fp->face_uvls[after_v + 1].v2 = uva->v2 + k * (uvb->v2 - uva->v2);
  fp->face_uvls[after_v + 1].alpha = uva->alpha + k * (uvb->alpha - uva->alpha);
}

// ============================================================================
// AddVertToAllEdges / AddPointToAllEdges
// ============================================================================
static void AddVertToAllEdges(room *rp, int v0, int v1, int new_v) {
  for (int f = 0; f < rp->num_faces; f++) {
    face *fp = &rp->faces[f];
    for (int v = 0; v < fp->num_verts; v++) {
      int cur = fp->face_verts[v];
      int nxt = fp->face_verts[(v + 1) % fp->num_verts];
      if ((cur == v0 && nxt == v1) || (cur == v1 && nxt == v0))
        AddVertToFace(rp, f, new_v, v);
    }
  }
}

static void AddPointToAllEdges(room *rp, int v0, int v1, const vector *new_v) {
  int newvertnum = RoomAddVertices(rp, 1);
  rp->verts[newvertnum] = *new_v;
  AddVertToAllEdges(rp, v0, v1, newvertnum);
}

// ============================================================================
// ClipAgainstEdge — clips a polygon against one edge, producing an
// inside polygon and possibly an outside polygon.
// ============================================================================
struct edge_insert {
  int v0, v1;
  int new_v;
};

static edge_insert Edge_inserts[MAX_VERTS_PER_FACE];
static int Num_edge_inserts;

static void AddEdgeInsert(int v0, int v1, int new_v) {
  Edge_inserts[Num_edge_inserts].v0 = v0;
  Edge_inserts[Num_edge_inserts].v1 = v1;
  Edge_inserts[Num_edge_inserts].new_v = new_v;
  Num_edge_inserts++;
}

static void ClipAgainstEdge(int nv, int16_t *vertnums, clip_vertex *vertices, int *num_vertices,
                            const vector *v0, const vector *v1, const vector *normal,
                            int16_t *inbuf, int *inv, int16_t *outbuf, int *onv) {
  int16_t *ip = inbuf, *op = outbuf;
  int inside_points = 0, outside_points = 0;

  for (int i = 0, prev = nv - 1, next = 1; i < nv; i++) {
    clip_vertex *curv = &vertices[vertnums[i]];

    int check = CheckPointAgainstEdge(&curv->vec, v0, v1, normal);
    if (check == 0) {
      *op++ = vertnums[i];
      *ip++ = vertnums[i];
    } else if (check == -1) {
      clip_vertex *prevv = &vertices[vertnums[prev]];
      clip_vertex *nextv = &vertices[vertnums[next]];

      int check2 = CheckPointAgainstEdge(&prevv->vec, v0, v1, normal);
      if (check2 == 1) {
        ClipEdge(normal, prevv, curv, v0, v1, &vertices[*num_vertices]);
        AddEdgeInsert(vertnums[prev], vertnums[i], *num_vertices);
        *op++ = *ip++ = (*num_vertices)++;
      }

      *op++ = vertnums[i];
      outside_points++;

      check2 = CheckPointAgainstEdge(&nextv->vec, v0, v1, normal);
      if (check2 == 1) {
        ClipEdge(normal, curv, nextv, v0, v1, &vertices[*num_vertices]);
        AddEdgeInsert(vertnums[i], vertnums[next], *num_vertices);
        *op++ = *ip++ = (*num_vertices)++;
      }
    } else {
      Q_ASSERT(check == 1);
      *ip++ = vertnums[i];
      inside_points++;
    }

    prev = i;
    if (++next == nv)
      next = 0;
  }

  *inv = inside_points ? (ip - inbuf) : 0;
  *onv = outside_points ? (op - outbuf) : 0;
}

// ============================================================================
// ClipFace — clips one face against another, producing the intersection
// polygon plus zero or more outside polygons.
// ============================================================================
static bool ClipFace(room *arp, int afacenum, room *brp, int bfacenum) {
  face *afp = &arp->faces[afacenum];
  face *bfp = &brp->faces[bfacenum];
  int16_t vbuf0[MAX_VERTS_PER_FACE], vbuf1[MAX_VERTS_PER_FACE];
  int16_t newface_verts[MAX_VERTS_PER_FACE][MAX_VERTS_PER_FACE];
  int newface_nvs[MAX_VERTS_PER_FACE];
  clip_vertex newverts[MAX_VERTS_PER_FACE];
  int16_t newvertnums[MAX_VERTS_PER_FACE];
  int num_newverts;
  int num_newfaces = 0;
  int16_t *src, *dest;
  int nv;

  nv = afp->num_verts;
  src = vbuf0;
  dest = vbuf1;
  Num_edge_inserts = 0;

  for (int i = 0; i < nv; i++) {
    newverts[i].vec = arp->verts[afp->face_verts[i]];
    newverts[i].uvl = afp->face_uvls[i];
    newvertnums[i] = afp->face_verts[i];
    src[i] = i;
  }
  num_newverts = nv;

  for (int edgenum = 0; edgenum < bfp->num_verts; edgenum++) {
    vector *ev0 = &brp->verts[bfp->face_verts[(bfp->num_verts - edgenum) % bfp->num_verts]];
    vector *ev1 = &brp->verts[bfp->face_verts[bfp->num_verts - edgenum - 1]];
    int16_t *outbuf = newface_verts[num_newfaces];
    int *onv = &newface_nvs[num_newfaces];

    ClipAgainstEdge(nv, src, newverts, &num_newverts, ev0, ev1, &afp->normal, dest, &nv, outbuf, onv);

    if (nv <= 2)
      return false;

    src = dest;
    dest = (src == vbuf0) ? vbuf1 : vbuf0;

    if (newface_nvs[num_newfaces])
      num_newfaces++;
  }

  int first_new_vert = RoomAddVertices(arp, num_newverts - afp->num_verts);

  for (int i = 0; i < num_newverts - afp->num_verts; i++) {
    arp->verts[first_new_vert + i] = newverts[afp->num_verts + i].vec;
    newvertnums[afp->num_verts + i] = first_new_vert + i;
  }

  ReInitRoomFace(afp, nv);
  for (int i = 0; i < nv; i++) {
    afp->face_verts[i] = newvertnums[src[i]];
    afp->face_uvls[i] = newverts[src[i]].uvl;
  }
  if (!ComputeFaceNormal(arp, afacenum))
    Q_ASSERT(0);

  int first_new_face = RoomAddFaces(arp, num_newfaces);

  for (int i = 0; i < num_newfaces; i++) {
    face *fp = &arp->faces[first_new_face + i];
    InitRoomFace(fp, newface_nvs[i]);
    for (int j = 0; j < newface_nvs[i]; j++) {
      fp->face_verts[j] = newvertnums[newface_verts[i][j]];
      fp->face_uvls[j] = newverts[newface_verts[i][j]].uvl;
    }
    if (!ComputeFaceNormal(arp, first_new_face + i))
      Q_ASSERT(0);
    fp->tmap = arp->faces[afacenum].tmap;
    CopyFaceFlags(fp, &arp->faces[afacenum]);
  }

  for (int i = 0; i < Num_edge_inserts; i++) {
    int v0 = newvertnums[Edge_inserts[i].v0];
    int v1 = newvertnums[Edge_inserts[i].v1];
    int new_v = newvertnums[Edge_inserts[i].new_v];
    AddVertToAllEdges(arp, v0, v1, new_v);
  }

  return true;
}

// ============================================================================
// ClipFacePair — clips two faces against each other
// ============================================================================
static bool ClipFacePair(room *rp0, int face0, room *rp1, int face1) {
  return ClipFace(rp0, face0, rp1, face1) && ClipFace(rp1, face1, rp0, face0);
}

// ============================================================================
// MatchPortalFaces — makes two portal faces match exactly by adding
// points where needed.  Returns number of points added.
// ============================================================================
static int MatchPortalFaces(room *rp0, int facenum0, room *rp1, int facenum1, bool check_only = false) {
  face *fp0 = &rp0->faces[facenum0];
  face *fp1 = &rp1->faces[facenum1];
  int n0, n1, i, j, prev_vn0, prev_vn1, max_nv;
  int points_added = 0;

check_faces:;

  // Find one point in common
  for (i = 0; i < fp0->num_verts; i++) {
    for (j = 0; j < fp1->num_verts; j++)
      if (PointsAreSame(&rp0->verts[fp0->face_verts[i]], &rp1->verts[fp1->face_verts[j]]))
        break;
    if (j < fp1->num_verts)
      break;
  }
  if (i >= fp0->num_verts) {
    if (!check_only)
      Q_ASSERT(0);
    return 0;
  }

  prev_vn0 = fp0->face_verts[i];
  prev_vn1 = fp1->face_verts[j];

  if (!check_only) {
    rp0->verts[prev_vn0] = rp1->verts[prev_vn1];
  }

  max_nv = std::max(fp0->num_verts, fp1->num_verts);

  for (n0 = n1 = 1; n0 < max_nv && n1 < max_nv; n0++, n1++) {
  recheck:;
    int vn0 = fp0->face_verts[(i + n0) % fp0->num_verts];
    int vn1 = fp1->face_verts[(j - n1 + fp1->num_verts) % fp1->num_verts];

    vector *v0 = &rp0->verts[vn0];
    vector *v1 = &rp1->verts[vn1];

    if (PointsAreSame(v0, v1)) {
      if (!check_only)
        *v0 = *v1;
    } else {
      vector *prev_v0 = &rp0->verts[prev_vn0];
      vector *prev_v1 = &rp1->verts[prev_vn1];

      float d0 = vm_VectorDistance(v0, prev_v0);
      float d1 = vm_VectorDistance(v1, prev_v1);

      if (d0 > d1) {
        if (CheckPointAgainstEdge(v1, prev_v0, v0, &fp0->normal)) {
          if (check_only)
            return 0;
          Q_ASSERT(0);
        } else {
          if (!check_only) {
            AddPointToAllEdges(rp0, prev_vn0, vn0, v1);
            points_added++;
          } else {
            n1++;
            goto recheck;
          }
        }
      } else {
        if (CheckPointAgainstEdge(v0, prev_v1, v1, &fp1->normal)) {
          if (check_only)
            return 0;
          Q_ASSERT(0);
        } else {
          if (!check_only) {
            AddPointToAllEdges(rp1, prev_vn1, vn1, v0);
            points_added++;
          } else {
            n0++;
            goto recheck;
          }
        }
      }

      if (!check_only)
        goto check_faces;
    }

    prev_vn0 = vn0;
    prev_vn1 = vn1;
  }

  if (check_only)
    return 1;

  Q_ASSERT(fp0->num_verts == fp1->num_verts);
  return points_added;
}

// ============================================================================
// AttachRoom — port of editor/HRoom.cpp:703
// Places a room that was placed via PlaceRoom, creating portals as
// needed.  Requires Placed_room, Placed_baseroomp, Placed_baseface,
// Placed_room_face, Placed_room_origin, Placed_room_attachpoint,
// and Placed_room_rotmat to be set by the caller.
// ============================================================================
void AttachRoom() {
  Q_ASSERT(Placed_room != -1);

  room *baseroomp = Placed_baseroomp;
  int baseface = Placed_baseface;
  room *attroomp = &Rooms[Placed_room];
  int attface = Placed_room_face;
  vector attcenter = Placed_room_origin;
  vector basecenter = Placed_room_attachpoint;

  // Find a free slot in Rooms[]
  int slot = -1;
  for (int i = 0; i < MAX_ROOMS; i++) {
    if (!Rooms[i].used) {
      slot = i;
      break;
    }
  }
  if (slot == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot attach room: No free rooms.");
    return;
  }

  // Initialize the room directly in the slot (avoids heap alloc + copy)
  room *newroomp = &Rooms[slot];
  memset(newroomp, 0, sizeof(room));
  InitRoom(newroomp, attroomp->num_verts, attroomp->num_faces, 0);

  // Rotate verts, copying into new room
  for (int i = 0; i < attroomp->num_verts; i++)
    newroomp->verts[i] = ((attroomp->verts[i] - attcenter) * Placed_room_rotmat) + basecenter;

  // Copy faces to new room
  for (int i = 0; i < attroomp->num_faces; i++) {
    CopyFace(&newroomp->faces[i], &attroomp->faces[i]);
  }

  // Recompute normals
  if (!ResetRoomFaceNormals(newroomp))
    Q_ASSERT(0);

  // Copy other room values
  newroomp->flags = attroomp->flags;
  newroomp->num_portals = 0;

  if (baseroomp == NULL) {
    // Terrain room — flag as external and delete the attach face
    newroomp->flags |= RF_EXTERNAL;
    DeleteRoomFace(newroomp, attface);
  } else {
    // Mine room — clip the connecting faces against each other
    if (!ClipFacePair(newroomp, attface, baseroomp, baseface)) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Error making portal -- faces probably don't overlap.");
      FreeRoom(newroomp);
      return;
    }

    MatchPortalFaces(baseroomp, baseface, newroomp, attface);

    LinkRooms(Rooms, ROOMNUM(baseroomp), baseface, ROOMNUM(newroomp), attface);

    // If there is a door, place it
    if (Placed_door != -1) {
      matrix orient = ~Placed_room_rotmat;
      vector doorcenter = {0, 0, 0};
      vector room_center = ((doorcenter - attcenter) * Placed_room_rotmat) + basecenter;

      FreeRoom(&Rooms[Placed_room]);

      ObjCreate(OBJ_DOOR, Placed_door, ROOMNUM(newroomp), &room_center, &orient);

      doorway *dp = DoorwayAdd(newroomp, Placed_door);
      (void)dp;

      Placed_door = -1;
    }
  }

  // Un-place the room
  Placed_room = -1;

  World_changed = true;
}

// ============================================================================
// UV manipulation functions — editor/RoomUVs.cpp and editor/HTexture.cpp
// ============================================================================

// GetUVLForRoomPoint — editor/Erooms.cpp:640
// Given a room, face, and vertex index, compute the default UV coordinates.
void GetUVLForRoomPoint(int roomnum, int facenum, int vertnum, roomUVL *uvl) {
  room *rp = &Rooms[roomnum];
  face *fp = &rp->faces[facenum];
  int nv = fp->num_verts;

  matrix face_matrix, trans_matrix;
  vector fvec, avg_vert, rot_vert;
  vector verts[MAX_VERTS_PER_FACE];

  // find the center point of this face
  vm_MakeZero(&avg_vert);
  int i;
  for (i = 0; i < nv; i++)
    avg_vert += rp->verts[fp->face_verts[i]];
  avg_vert /= static_cast<float>(i);

  // Make the orientation matrix (reverse the normal: looking "at" the face)
  fvec = -fp->normal;
  vm_VectorToMatrix(&face_matrix, &fvec, nullptr, nullptr);

  angvec avec;
  vm_ExtractAnglesFromMatrix(&avec, &face_matrix);
  vm_AnglesToMatrix(&trans_matrix, avec.p(), avec.h(), avec.b());

  // Rotate all the points
  for (i = 0; i < nv; i++) {
    vector vert = rp->verts[fp->face_verts[i]];
    vert -= avg_vert;
    vm_MatrixMulVector(&rot_vert, &vert, &trans_matrix);
    verts[i] = rot_vert;
  }

  // Find left most point
  int leftmost_point = 0;
  float leftmost_x = verts[0].x();
  for (i = 1; i < nv; i++) {
    if (verts[i].x() < leftmost_x) {
      leftmost_point = i;
      leftmost_x = verts[i].x();
    }
  }

  // Find top most point
  int topmost_point = 0;
  float topmost_y = verts[0].y();
  for (i = 1; i < nv; i++) {
    if (verts[i].y() > topmost_y) {
      topmost_point = i;
      topmost_y = verts[i].y();
    }
  }

  // now set the base vertex
  vector base_vector;
  base_vector.x() = verts[leftmost_point].x();
  base_vector.y() = verts[topmost_point].y();
  base_vector.z() = 0;

  // now actually find the uv of our specified point
  uvl->u = (verts[vertnum].x() - base_vector.x()) / 20.0f;
  uvl->v = (base_vector.y() - verts[vertnum].y()) / 20.0f;
}

// StretchRoomUVs — editor/RoomUVs.cpp:168
// Stretches the UVs of a face along the selected edge.
// Edge is the vertex index — the edge is (edge, edge+1).
void StretchRoomUVs(room *rp, int facenum, int edge) {
  face *fp = &rp->faces[facenum];
  int nv = fp->num_verts;
  int v0 = edge;
  int v1 = (v0 + 1) % nv;

  roomUVL uv0 = fp->face_uvls[v0];
  roomUVL uv1 = fp->face_uvls[v1];

  AssignUVsToFace(rp, facenum, &uv0, &uv1, v0, v1);
}

// ScaleFaceUVs — editor/RoomUVs.cpp:195
// Scale all UV values in a face from the center point (average of u,v).
void ScaleFaceUVs(room *rp, int facenum, float scale) {
  face *fp = &rp->faces[facenum];
  int nv = fp->num_verts;

  float midu = 0, midv = 0;
  for (int i = 0; i < nv; i++) {
    midu += fp->face_uvls[i].u;
    midv += fp->face_uvls[i].v;
  }
  midu /= nv;
  midv /= nv;

  for (int i = 0; i < nv; i++) {
    fp->face_uvls[i].u = midu + (fp->face_uvls[i].u - midu) * scale;
    fp->face_uvls[i].v = midv + (fp->face_uvls[i].v - midv) * scale;
  }
}

// HTextureSlide — editor/HTexture.cpp:334
// Slide all UVs on a face by (right, up) in 1/128th texture units.
void HTextureSlide(room *rp, int facenum, float right, float up) {
  if (!rp || !rp->used)
    return;
  for (int i = 0; i < rp->faces[facenum].num_verts; i++) {
    rp->faces[facenum].face_uvls[i].u -= right / 128.0f;
    rp->faces[facenum].face_uvls[i].v += up / 128.0f;
  }
}

// HTextureRotate — editor/HTexture.cpp:371
// Rotate all UVs on a face by the given angle (in radians).
void HTextureRotate(room *rp, int facenum, float angle_rad) {
  if (!rp || !rp->used)
    return;
  face *fp = &rp->faces[facenum];
  int nv = fp->num_verts;
  if (nv <= 0)
    return;

  float cu = 0, cv = 0;
  for (int i = 0; i < nv; i++) {
    cu += fp->face_uvls[i].u;
    cv += fp->face_uvls[i].v;
  }
  cu /= nv;
  cv /= nv;

  float c = std::cos(angle_rad);
  float s = std::sin(angle_rad);

  for (int i = 0; i < nv; i++) {
    float du = fp->face_uvls[i].u - cu;
    float dv = fp->face_uvls[i].v - cv;
    fp->face_uvls[i].u = cu + du * c - dv * s;
    fp->face_uvls[i].v = cv + du * s + dv * c;
  }
}

// HTextureFlipX — editor/HTexture.cpp:274
// Flip the U coordinate: u = 1 - u.
void HTextureFlipX(room *rp, int facenum) {
  if (!rp || !rp->used)
    return;
  for (int i = 0; i < rp->faces[facenum].num_verts; i++)
    rp->faces[facenum].face_uvls[i].u = 1 - rp->faces[facenum].face_uvls[i].u;
}

// HTextureFlipY — editor/HTexture.cpp:304
// Flip the V coordinate: v = 1 - v.
void HTextureFlipY(room *rp, int facenum) {
  if (!rp || !rp->used)
    return;
  for (int i = 0; i < rp->faces[facenum].num_verts; i++)
    rp->faces[facenum].face_uvls[i].v = 1 - rp->faces[facenum].face_uvls[i].v;
}

// HTextureRoomStretch — editor/HTexture.cpp:480
// Stretch UVs perpendicular to the selected edge. direction = +1 or -1.
void HTextureRoomStretch(room *rp, int facenum, int edge, int direction) {
  face *fp = &rp->faces[facenum];
  int nv = fp->num_verts;
  int next_edge = (edge + 1) % nv;

  float du = fp->face_uvls[next_edge].u - fp->face_uvls[edge].u;
  float dv = fp->face_uvls[next_edge].v - fp->face_uvls[edge].v;

  float nu = -dv;
  float nv_uv = du;
  float mag = sqrt(nv_uv * nv_uv + nu * nu);
  if (mag < 0.001f)
    return;
  nv_uv /= mag;
  nu /= mag;
  nv_uv /= 64.0f;
  nu /= 64.0f;

  for (int i = 0; i < nv; i++) {
    if (i == edge || i == next_edge)
      continue;
    fp->face_uvls[i].u -= nu * direction;
    fp->face_uvls[i].v -= nv_uv * direction;
  }
}

// HTextureStretchMore — editor/HTexture.cpp:400
void HTextureStretchMore(room *rp, int facenum, int edge, float texscale) {
  HTextureRoomStretch(rp, facenum, edge, (int)(1 * texscale));
}

// HTextureStretchLess — editor/HTexture.cpp:408
void HTextureStretchLess(room *rp, int facenum, int edge, float texscale) {
  HTextureRoomStretch(rp, facenum, edge, (int)(-1 * texscale));
}

// HTextureSetDefault — editor/HTexture.cpp:416
// Reset UVs to defaults using GetUVLForRoomPoint.
void HTextureSetDefault(room *rp, int facenum) {
  if (!rp || !rp->used)
    return;
  for (int i = 0; i < rp->faces[facenum].num_verts; i++) {
    float saveu2 = rp->faces[facenum].face_uvls[i].u2;
    float savev2 = rp->faces[facenum].face_uvls[i].v2;
    GetUVLForRoomPoint(ROOMNUM(rp), facenum, i, &rp->faces[facenum].face_uvls[i]);
    rp->faces[facenum].face_uvls[i].u2 = saveu2;
    rp->faces[facenum].face_uvls[i].v2 = savev2;
  }
}

// HTexturePropagateToFace — editor/HTexture.cpp:215
// Copy texture from current face to adjacent face, tiling UVs.
// tex = true also copies the texture assignment.
int HTexturePropagateToFace(room *destrp, int destface, room *srcrp, int srcface, bool tex) {
  face *dfp = &destrp->faces[destface];
  face *sfp = &srcrp->faces[srcface];
  int v0, v1;

  if (!FindSharedEdge(dfp, sfp, &v0, &v1))
    return 0;

  if (tex)
    dfp->tmap = sfp->tmap;

  AssignUVsToFace(destrp, destface, &sfp->face_uvls[(v1 + 1) % sfp->num_verts], &sfp->face_uvls[v1], v0,
                  (v0 + 1) % dfp->num_verts);

  return 1;
}

// HTextureCopyUVsToFace — editor/HTexture.cpp:251
// Copy texture UVs from one face to another with offset.
int HTextureCopyUVsToFace(room *destrp, int destface, room *srcrp, int srcface, int offset) {
  face *dfp = &destrp->faces[destface];
  face *sfp = &srcrp->faces[srcface];

  if (dfp->num_verts != sfp->num_verts)
    return 0;

  for (int i = 0; i < dfp->num_verts; i++) {
    dfp->face_uvls[(i + offset) % dfp->num_verts].u = sfp->face_uvls[i].u;
    dfp->face_uvls[(i + offset) % dfp->num_verts].v = sfp->face_uvls[i].v;
  }

  return 1;
}

// HTextureApplyToRoomFace — editor/HTexture.cpp:198
void HTextureApplyToRoomFace(room *rp, int facenum, int tnum) {
  if (!rp || !rp->used)
    return;
  rp->faces[facenum].tmap = tnum;
}

// ============================================================================
// PlaceRoom / ComputePlacedRoomMatrix / PlaceDoor
// Port of editor/HRoom.cpp:540-616 and editor/edoors.cpp:36-129.
//
// PlaceRoom sets up the Placed_* globals for interactive room placement.
// ComputePlacedRoomMatrix computes the rotation matrix for the placed room.
// PlaceDoor creates a room from a door polymodel and calls PlaceRoom.
// ============================================================================

void ComputePlacedRoomMatrix() {
  room *placedroomp;
  int placedface;
  matrix srcmat;
  vector t;

  if (Placed_room != -1) {
    placedroomp = &Rooms[Placed_room];
    placedface = Placed_room_face;
  } else {
    placedroomp = &Rooms[Placed_room];
    placedface = Placed_room_face;
  }

  t = -placedroomp->faces[placedface].normal;
  vm_VectorToMatrix(&srcmat, &t, NULL, NULL);
  vm_VectorAngleToMatrix(&Placed_room_orient, &Placed_room_orient.fvec, Placed_room_angle);

  vm_Orthogonalize(&srcmat);
  vm_Orthogonalize(&Placed_room_orient);

  vm_MatrixMulTMatrix(&Placed_room_rotmat, &srcmat, &Placed_room_orient);
  vm_Orthogonalize(&Placed_room_rotmat);
}

// PlaceRoom — editor/HRoom.cpp:585
// Sets up globals for interactive room placement.
void PlaceRoom(room *baseroomp, int baseface, int placed_room, int placed_room_face, int placed_room_door) {
  Q_ASSERT(baseroomp->faces[baseface].portal_num == -1);

  room *placedroomp = &Rooms[placed_room];

  Placed_room = placed_room;
  Placed_room_face = placed_room_face;
  Placed_room_orient.fvec = baseroomp->faces[baseface].normal;
  Placed_room_angle = 0;
  Placed_baseroomp = baseroomp;
  Placed_baseface = baseface;
  Placed_door = placed_room_door;

  ComputeCenterPointOnFace(&Placed_room_attachpoint, baseroomp, baseface);
  ComputeCenterPointOnFace(&Placed_room_origin, placedroomp, placed_room_face);

  ComputePlacedRoomMatrix();
}

// PlaceDoor — editor/edoors.cpp:36
// Creates a room from a door polymodel (shell + front face submodels) and
// places it for interactive positioning.
void PlaceDoor(room *baseroomp, int baseface, int placed_door) {
  poly_model *po = GetPolymodelPointer(GetDoorImage(placed_door));
  if (po == nullptr)
    return;

  int num_faces = 0, num_verts = 0;
  int got_shell = 0, got_front = 0;
  bsp_info *front_sm = nullptr, *shell_sm = nullptr;

  for (int i = 0; i < po->n_models; i++) {
    bsp_info *sm = &po->submodel[i];
    if (sm->flags & SOF_SHELL) {
      got_shell = 1;
      num_verts += sm->nverts;
      num_faces += sm->num_faces;
      shell_sm = sm;
    }
    if (sm->flags & SOF_FRONTFACE) {
      got_front = 1;
      num_verts += sm->nverts;
      num_faces++; // front face is always one face
      front_sm = sm;
    }
  }

  if (!got_shell || !got_front || shell_sm == nullptr || front_sm == nullptr) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "This door is not properly specified (missing shell or front face).");
    return;
  }

  int total_verts = num_verts;
  int total_faces = num_faces;

  room *rp = CreateNewRoom(total_verts, total_faces);
  Q_ASSERT(rp != nullptr);

  int index = 0;

  // Copy shell vertices
  for (int i = 0; i < shell_sm->nverts; i++, index++) {
    rp->verts[index] = shell_sm->verts[i];
  }
  // Copy front face vertices
  for (int i = 0; i < front_sm->nverts; i++, index++) {
    rp->verts[index] = front_sm->verts[i];
  }

  // Create faces from shell
  index = 0;
  for (int i = 0; i < shell_sm->num_faces; i++, index++) {
    InitRoomFace(&rp->faces[index], shell_sm->faces[i].nverts);
    rp->faces[index].tmap = D3EditState.texdlg_texture;
    for (int t = 0; t < rp->faces[index].num_verts; t++)
      rp->faces[index].face_verts[t] = shell_sm->faces[i].vertnums[t];
  }

  // Create the front face (always one face)
  int front_face_index = index;
  Q_ASSERT(front_sm->num_faces == 1);
  InitRoomFace(&rp->faces[index], front_sm->faces[0].nverts);
  rp->faces[index].tmap = D3EditState.texdlg_texture;

  // Remap front face vertices to match the shell
  int front_remap[30];
  for (int i = 0; i < front_sm->nverts; i++)
    front_remap[i] = -1;

  vector diff_vec = front_sm->offset - shell_sm->offset;

  for (int i = 0; i < shell_sm->nverts; i++) {
    for (int t = 0; t < front_sm->nverts; t++) {
      vector testvec = front_sm->verts[t] + diff_vec;
      if (PointsAreSame(&shell_sm->verts[i], &testvec))
        front_remap[t] = i;
    }
  }

  for (int i = 0; i < front_sm->nverts; i++)
    Q_ASSERT(front_remap[i] != -1);

  for (int t = 0; t < rp->faces[front_face_index].num_verts; t++)
    rp->faces[front_face_index].face_verts[t] = front_remap[front_sm->faces[0].vertnums[t]];

  // Compute normals and UVs
  if (!ResetRoomFaceNormals(rp))
    Q_ASSERT(0);

  AssignDefaultUVsToRoom(rp);

  // Now call PlaceRoom to set up interactive placement
  PlaceRoom(baseroomp, baseface, ROOMNUM(rp), front_face_index, placed_door);
}
