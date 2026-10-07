#include "editor_room_state.h"

#include "room.h"
#include "d3edit.h"

// Room selection list (defined in the MFC editor's EDVARS.cpp).
int N_selected_rooms = 0;
std::array<int, MAX_ROOMS> Selected_rooms;

// Editor-only room/face helpers (defined in editor/Erooms.cpp /
// editor/selectedroom.cpp). The Qt port's level_io.cpp + room_ops.cpp use
// these to construct / tear down rooms with full geometry; they preserve
// the public signatures so qteditor links cleanly.

int CreateNewRoom(int nverts, int nfaces, bool palette_room) {
  (void)palette_room;
  const int slot = FindFreeRoomSlot();
  if (slot < 0)
    return -1;

  // `room{}` value-initialises (zeroing PODs, default-constructing the
  // std::string name).  No memset — memset would corrupt the std::string.
  room_t &rp = Rooms[slot];
  rp = room_t{};
  rp.used = 1;
  rp.verts.resize(nverts);
  rp.faces.resize(nfaces);
  rp.num_verts = nverts;
  rp.num_faces = nfaces;
  return slot;
}

// Mirrors GetFreeRoom() in editor/Erooms.cpp:579: a linear scan for the first
// unused slot, else a fresh slot appended at the end of the room table.  The
// vector's size tracks the high-water mark (size() == watermark + 1), so no
// separate watermark update is needed.  The Win32 BNode terrain remap and
// Current_faces touch-up are editor UI side-effects and are skipped.  Returns
// the slot index or -1 when the room capacity limit is reached.
int FindFreeRoomSlot() {
  for (int roomnum = 0; roomnum < static_cast<int>(Rooms.size()); ++roomnum)
    if (!Rooms[roomnum].used)
      return roomnum;

  const int slot = static_cast<int>(Rooms.size());
  if (!RoomsEnsureIndex(slot))
    return -1;
  return slot;
}

// Counterpart of CreateNewRoom() that releases the per-room vectors/faces
// array and marks the slot free. The full Win32 path also walks the
// portal list, recycles to the free list, and detaches from the marked
// room; the Qt port stops at "free the slot" because app.current.room tracking
// lives at the qteditor level, not in Descent3Core.
void DestroyRoom(int roomnum) {
  if (roomnum < 0)
    return;
  if (static_cast<size_t>(roomnum) >= Rooms.size())
    return;
  room_t *rp = &Rooms[roomnum];
  if (!rp->used)
    return;
  rp->verts.clear();
  rp->verts4.clear();
  rp->faces.clear();
  rp->portals.clear();
  rp->bbf_list.clear();
  rp->num_bbf.clear();
  rp->bbf_list_min_xyz.clear();
  rp->bbf_list_max_xyz.clear();
  rp->bbf_list_sector.clear();
  rp->used = 0;
  rp->num_verts = 0;
  rp->num_faces = 0;
  rp->num_portals = 0;

  // Rooms.size() == high-water mark + 1, so pop the unused tail (matching
  // the Win32 room free path's trim loop).
  while (!Rooms.empty() && !Rooms.back().used)
    Rooms.pop_back();
}

// Editor-only room selection helper (defined in editor/selectedroom.cpp).
int IsRoomSelected(int roomnum) {
  for (int i = 0; i < N_selected_rooms; i++)
    if (Selected_rooms[i] == roomnum)
      return 1;
  return 0;
}


void AssignDefaultUVsToRoomFace(int roomnum, int facenum) {
  if (roomnum < 0 || static_cast<size_t>(roomnum) >= Rooms.size() || !Rooms[roomnum].used)
    return;
  if (facenum < 0 || facenum >= Rooms[roomnum].num_faces)
    return;
  face *fp = &Rooms[roomnum].faces[facenum];
  if (fp->num_verts < 3)
    return;

  for (int t = 0; t < fp->num_verts; t++) {
    GetUVLForRoomPoint(roomnum, facenum, t, &fp->face_uvls[t]);
    fp->face_uvls[t].alpha = 255;
  }
}

// Editor-only room selection list (editor/selectedroom.cpp in Win32).
void ClearRoomSelectedList() {
  N_selected_rooms = 0;
  app.State_changed = true;
}

void AddRoomToSelectedList(int roomnum) {
  if (!IsRoomSelected(roomnum)) {
    Selected_rooms[N_selected_rooms++] = roomnum;
    app.State_changed = true;
  }
}

void RemoveRoomFromSelectedList(int roomnum) {
  for (int i = 0; i < N_selected_rooms; i++) {
    if (Selected_rooms[i] == roomnum) {
      for (int j = i; j < N_selected_rooms - 1; j++)
        Selected_rooms[j] = Selected_rooms[j + 1];
      N_selected_rooms--;
      app.State_changed = true;
      return;
    }
  }
}

int ToggleRoomSelectedState(int roomnum) {
  app.State_changed = true;
  for (int i = 0; i < N_selected_rooms; i++) {
    if (Selected_rooms[i] == roomnum) {
      for (int j = i; j < N_selected_rooms - 1; j++)
        Selected_rooms[j] = Selected_rooms[j + 1];
      N_selected_rooms--;
      return 0;
    }
  }
  Selected_rooms[N_selected_rooms++] = roomnum;
  return 1;
}

int SelectConnectedRooms(int roomnum) {
  if (IsRoomSelected(roomnum))
    return 0;

  Selected_rooms[N_selected_rooms++] = roomnum;
  int count = 1;
  app.State_changed = true;

  for (int s = 0; s < Rooms[roomnum].num_portals; s++) {
    const index_t connected_room = Rooms[roomnum].portals[s].connected_room;
    if (connected_room)
      count += SelectConnectedRooms(*connected_room);
  }
  return count;
}

static int *Save_selected_rooms = nullptr;
static int N_save_selected_rooms = -1;

void SaveRoomSelectedList() {
  if (N_save_selected_rooms != -1)
    return;

  N_save_selected_rooms = N_selected_rooms;
  if (!N_save_selected_rooms)
    return;

  Save_selected_rooms = new int[N_save_selected_rooms];
  for (int i = 0; i < N_selected_rooms; i++)
    Save_selected_rooms[i] = Selected_rooms[i];
}

void RestoreRoomSelectedList() {
  if (N_save_selected_rooms == -1)
    return;

  N_selected_rooms = N_save_selected_rooms;
  N_save_selected_rooms = -1;

  if (!N_selected_rooms)
    return;

  for (int i = 0; i < N_selected_rooms; i++)
    Selected_rooms[i] = Save_selected_rooms[i];

  delete[] Save_selected_rooms;
  Save_selected_rooms = nullptr;
}

