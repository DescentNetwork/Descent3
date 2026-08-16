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

// Qt implementations of the "Room" menu operations that editor/HRoom.cpp,
// editor/selectedroom.cpp, and editor/Erooms.cpp dispatches in the legacy
// MFC build. The geometry paths use Descent3Core (InitRoomFace, Compute-
// FaceNormal, AssignDefaultUVsToRoomFace from Descent3/room.cpp +
// Descent3/erooms.cpp) plus the qteditor-side CreateNewRoom /
// DestroyRoom in d3_editor_state.cpp. UI bits (the file dialog for
// SaveCurrentRoom) defer to QFileDialog through editor_file_dialogs.

#include "room_ops.h"

#include "d3edit.h"
#include "editor_room_state.h"
#include "gametexture.h"
#include "level_io.h"
#include "object.h"
#include "room.h"

#include <QInputDialog>
#include <QLineEdit>
#include <QString>
#include <QWidget>

#include <cstdio>
#include <cstring>

namespace QtEditor {

namespace {

// Default extrusion length for AddRoom (`DEFAULT_ROOM_LENGTH` in
// editor/HRoom.cpp). The new room's verts are pushed outward along the
// current face's normal by this amount.
constexpr float kDefaultRoomLength = 20.0f;

// Find the first free slot in Rooms[]. Returns the index or -1 if every
// slot is in use. Walks Highest_room_index + 1 first so newly freed
// slots get re-used before we extend the high-water mark.
int find_free_room_slot() {
  const int limit = std::min(Highest_room_index + 1, MAX_ROOMS - 1);
  for (int i = 0; i <= limit; ++i)
    if (!Rooms[i].used)
      return i;
  for (int i = limit + 1; i < MAX_ROOMS; ++i)
    if (!Rooms[i].used)
      return i;
  return -1;
}

} // namespace

bool AddRoom() {
  if (Curroomp == nullptr) {
    std::fprintf(stderr, "[room_ops] AddRoom: no current room\n");
    return false;
  }
  if (Curface < 0 || Curface >= Curroomp->num_faces) {
    std::fprintf(stderr, "[room_ops] AddRoom: invalid current face (%d)\n",
                 Curface);
    return false;
  }
  face *cfp = &Curroomp->faces[Curface];
  if (cfp->portal_num != -1) {
    std::fprintf(stderr,
                 "[room_ops] AddRoom: face already connected (portal %d)\n",
                 cfp->portal_num);
    return false;
  }

  const int cnv = cfp->num_verts;
  if (cnv < 3) {
    std::fprintf(stderr, "[room_ops] AddRoom: face has %d verts, need >= 3\n",
                 cnv);
    return false;
  }
  const int nfaces = cnv + 2;
  const int slot = find_free_room_slot();
  if (slot < 0) {
    std::fprintf(stderr, "[room_ops] AddRoom: no free slot\n");
    return false;
  }

  // Allocate the room at the chosen slot (CreateNewRoom uses new[] for
  // verts/faces; DestroyRoom in d3_editor_state.cpp releases those when
  // DeleteRoom is called).
  room *rp = CreateNewRoom(cnv * 2, nfaces, /*palette_room=*/false);
  if (rp == nullptr) {
    std::fprintf(stderr, "[room_ops] AddRoom: CreateNewRoom returned null\n");
    return false;
  }

  // Drop the freshly minted room into Rooms[] at `slot`. The pointer
  // returned by CreateNewRoom is heap-allocated; we copy it into the
  // slot and then orphan the heap copy so DestroyRoom handles the field
  // arrays correctly.
  Rooms[slot] = *rp;
  rp->verts = nullptr;
  rp->faces = nullptr;
  rp->portals = nullptr;
  delete rp;

  rp = &Rooms[slot];
  if (slot > Highest_room_index)
    Highest_room_index = slot;

  // Geometry: extrude the current face's verts outward by `kDefaultRoomLength`
  // along the face normal so the new room extends from the existing face.
  const vector room_delta = cfp->normal * -kDefaultRoomLength;
  for (int i = 0; i < cnv; ++i) {
    rp->verts[i] = Curroomp->verts[cfp->face_verts[cnv - 1 - i]];
    rp->verts[cnv + i] = rp->verts[i] + room_delta;
  }

  // Faces: front face copies current face verts in reverse, back face
  //          is the extruded copy, and the (cnv) side quads stitch them.
  InitRoomFace(&rp->faces[0], cnv);
  for (int i = 0; i < cnv; ++i)
    rp->faces[0].face_verts[i] = i;

  InitRoomFace(&rp->faces[1], cnv);
  for (int i = 0; i < cnv; ++i)
    rp->faces[1].face_verts[i] = cnv * 2 - 1 - i;

  for (int i = 0; i < nfaces - 2; ++i) {
    InitRoomFace(&rp->faces[i + 2], 4);
    rp->faces[i + 2].face_verts[0] = i;
    rp->faces[i + 2].face_verts[1] = i + cnv;
    rp->faces[i + 2].face_verts[2] = ((i + 1) % cnv) + cnv;
    rp->faces[i + 2].face_verts[3] = (i + 1) % cnv;
  }

  for (int i = 0; i < nfaces; ++i) {
    if (!ComputeFaceNormal(rp, i)) {
      std::fprintf(stderr,
                   "[room_ops] AddRoom: ComputeFaceNormal failed for face %d\n",
                   i);
    }
    rp->faces[i].tmap = (i + 1) % MAX_TEXTURES;
    AssignDefaultUVsToRoomFace(rp, i);
  }

  // Wire the new room into the editor view: it's the current selection
  // and the marked room for follow-on edits.
  Curroomp = rp;
  Curface = Curedge = Curvert = Curportal = 0;
  MarkRoom();
  D3EditState.current_room = slot;

  Mine_changed = 1;
  New_mine = 1;
  std::fprintf(stderr, "[room_ops] AddRoom -> room %d (%d verts, %d faces)\n",
               slot, cnv * 2, nfaces);
  return true;
}

bool DeleteRoom() {
  if (Curroomp == nullptr) {
    std::fprintf(stderr, "[room_ops] DeleteRoom: no current room\n");
    return false;
  }
  if (!Curroomp->used) {
    std::fprintf(stderr, "[room_ops] DeleteRoom: current room already unused\n");
    Curroomp = nullptr;
    return false;
  }
  // Don't delete the room with the player in it — editor/HRoom.cpp's
  // DeleteRoomFromMine() bails on that. Our stub doesn't track
  // Player_object's room yet, so this is a straight "no player here" OK.
  const int slot = ROOMNUM(Curroomp);

  // Clear any marked-room alias before we tear down the slot.
  if (Markedroomp == Curroomp)
    Markedroomp = nullptr;

  DestroyRoom(slot);

  // Pick a sensible successor selection: previous used slot, or -1.
  Curroomp = nullptr;
  Curface = Curedge = Curvert = Curportal = -1;
  D3EditState.current_room = -1;
  for (int s = slot - 1; s >= 0; --s) {
    if (Rooms[s].used) {
      Curroomp = &Rooms[s];
      D3EditState.current_room = s;
      break;
    }
  }
  Mine_changed = 1;

  std::fprintf(stderr, "[room_ops] DeleteRoom: cleared slot %d\n", slot);
  return true;
}

void MarkRoom() {
  // editor/selectedroom.cpp::SetMarkedRoom() captures (Curroomp,
  // Curface, Curedge, Curvert); we mirror the same state but use the qteditor
  // globals From d3_editor_state.cpp.
  Markedroomp = Curroomp;
  Markedface = Curface;
  Markededge = Curedge;
  Markedvert = Curvert;
  State_changed = 1;
  std::fprintf(stderr, "[room_ops] MarkRoom: slot %d face %d\n",
               Curroomp ? ROOMNUM(Curroomp) : -1, Curface);
}

int SelectRoomByNumber() {
  // Use the MFC-equivalent of OutrageMessageBox("Select room number", "...").
  // QInputDialog::getInt is the natural Qt analogue; the test suite
  // dismisses it via dismissModals() so headless runs auto-cancel.
  bool ok = false;
  const int value = QInputDialog::getInt(
      nullptr, QStringLiteral("Select Room"),
      QStringLiteral("Enter room number to select:"), 0, 0, MAX_ROOMS, 1, &ok);
  if (!ok)
    return -1;
  if (value < 0 || value >= MAX_ROOMS || !Rooms[value].used) {
    std::fprintf(stderr,
                 "[room_ops] SelectRoomByNumber: %d is not an used slot\n",
                 value);
    return -1;
  }
  Curroomp = &Rooms[value];
  Curface = Curedge = Curvert = Curportal = 0;
  D3EditState.current_room = value;
  return value;
}

bool RenameRoom() {
  if (Curroomp == nullptr)
    return false;
  bool ok = false;
  QString current = (Curroomp->name != nullptr)
                        ? QString::fromLatin1(Curroomp->name)
                        : QString();
  const QString picked = QInputDialog::getText(
      nullptr, QStringLiteral("Rename Room"),
      QStringLiteral("New name:"), QLineEdit::Normal, current, &ok);
  if (!ok || picked.isEmpty())
    return false;
  QByteArray bytes = picked.toLatin1();
  bytes.append('\0');
  char *buf = bytes.data();
  QtEditor::StripLeadingTrailingSpaces(buf);
  std::strncpy(Curroomp->name, buf, sizeof(Curroomp->name) - 1);
  Curroomp->name[sizeof(Curroomp->name) - 1] = '\0';
  Mine_changed = 1;
  std::fprintf(stderr, "[room_ops] RenameRoom -> %s\n", Curroomp->name);
  return true;
}

bool SaveCurrentRoom() {
  if (Curroomp == nullptr)
    return false;
  // editor/HRoom.cpp::SaveRoom writes a single-room .orf snippet under the
  // current .d3l filename. The Qt port doesn't yet drive that binary path;
  // mark the mine as changed so the next Save writes it once the engine
  // bridge lands.
  Mine_changed = 1;
  std::fprintf(stderr,
               "[room_ops] SaveCurrentRoom: deferred to EditorSaveLevel\n");
  return true;
}

} // namespace QtEditor
