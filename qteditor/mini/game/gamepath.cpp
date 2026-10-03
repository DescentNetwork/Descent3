// Game path subsystem ported from Descent3/gamepath.cpp.
//

#include "gamepath.h"

#include <QMessageBox>

//#include "mem/mem.h"
#include "string_helpers.h"
#include "findintersection.h"

// Returns the index of the game path whose name matches, or -1 if not found.
std::optional<uint32_t> FindGamePathName(const std::string &name) {
  for (uint32_t i = 0; i < GamePaths.size(); i++) {
    if (GamePaths.is_used(i) && match(GamePaths[i].name, name))
      return i;
  }
  return std::nullopt;
}

// Frees gamepath n for future use
void FreeGamePath(int n) {
  if (n < 0 || n >= static_cast<int>(GamePaths.size()))
    return;

  if (GamePaths.is_unused(n))
    return;

  GamePaths[n].pathnodes.clear();

  GamePaths[n].num_nodes = 0;
  GamePaths.release(n);
  Num_game_paths--;
}

// Clears every path slot.  Ported from the engine's InitGamePaths: a fresh
// level (or one whose PATH chunk references a subset of slots) must start from
// an empty table, otherwise stale paths leak across loads.
void InitGamePaths() {
  static bool f_game_paths_init = false;

  if (f_game_paths_init) {
    // Clear out the current path info
    for (size_t i = 0; i < GamePaths.size(); i++) {
      FreeGamePath(i);
    }
  }

  f_game_paths_init = true;

  for (size_t i = 0; i < GamePaths.size(); i++) {
    GamePaths[i].num_nodes = 0;
  }

  Num_game_paths = 0;
}


// Path editing helpers (editor/EPath.cpp in the MFC editor).
bool Show_paths = true;

int InsertNodeIntoPath(int pathnum, int nodenum, int flags, int roomnum, vector3 pos, matrix orient) {
  if (GamePaths[pathnum].pathnodes.size() >= MAX_NODES_PER_PATH) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Path already has its maximum amount of nodes.");
    return -1;
  }
  GamePaths[pathnum].pathnodes.insert(GamePaths[pathnum].pathnodes.begin() + nodenum + 1, node{});
  node &newnode_ref = GamePaths[pathnum].pathnodes[nodenum + 1];
  newnode_ref.pos = pos;
  newnode_ref.roomnum = roomnum;
  reinterpret_cast<uint32_t &>(newnode_ref.flags) = static_cast<uint32_t>(flags);
  newnode_ref.fvec = orient.fvec;
  newnode_ref.uvec = orient.uvec;
  GamePaths[pathnum].num_nodes++;
  return nodenum + 1;
}

void DeleteNodeFromPath(int pathnum, int nodenum) {
  GamePaths[pathnum].pathnodes.erase(GamePaths[pathnum].pathnodes.begin() + nodenum);
  GamePaths[pathnum].num_nodes--;
}

int AllocGamePath() {
  for (size_t i = 0; i < GamePaths.size(); i++) {
    if (GamePaths.is_unused(i)) {
      GamePaths.acquire(i);
      GamePaths[i].name.clear();
      GamePaths[i].num_nodes = 0;
      GamePaths[i].flags = {};
      GamePaths[i].pathnodes.clear();
      Num_game_paths++;
      return (int)i;
    }
  }
  // No free slot anywhere: grow the table by one at the frontier.
  const size_t i = GamePaths.add_slot(game_path{});
  GamePaths.acquire(i);
  Num_game_paths++;
  return (int)i;
}

int MovePathNodeToPos(int pathnum, int nodenum, vector3 *attempted_pos) {
  fvi_query fq;
  fvi_info hit_info;

  fq.p0 = &GamePaths[pathnum].pathnodes[nodenum].pos;
  fq.startroom = GamePaths[pathnum].pathnodes[nodenum].roomnum;
  fq.p1 = attempted_pos;
  fq.rad = 0.0f;
  fq.thisobjnum = -1;
  fq.ignore_obj_list = NULL;
  fq.flags = fvi_query_flags_t{};
  fq.flags.transpoint = true;
  fq.flags.ignore_render_through_portals = true;
  fvi_FindIntersection(&fq, &hit_info);

  if (nodenum >= 1) {
    fvi_query fq1;
    fvi_info hit_info1;
    fq1.p0 = &GamePaths[pathnum].pathnodes[nodenum - 1].pos;
    fq1.startroom = GamePaths[pathnum].pathnodes[nodenum - 1].roomnum;
    fq1.p1 = &hit_info.hit_pnt;
    fq1.rad = 0.0f;
    fq1.thisobjnum = -1;
    fq1.ignore_obj_list = NULL;
    fq1.flags = fvi_query_flags_t{};
    fq1.flags.transpoint = true;
    fq1.flags.ignore_render_through_portals = true;
    fvi_FindIntersection(&fq1, &hit_info1);
    if (vm_VectorDistance(&hit_info.hit_pnt, &hit_info1.hit_pnt) > .005) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot move point.  No line of sight from the previous node to the new position.");
      return -1;
    }
  }

  if (nodenum < GamePaths[pathnum].num_nodes - 1) {
    fvi_query fq1;
    fvi_info hit_info1;
    fq1.p0 = &GamePaths[pathnum].pathnodes[nodenum + 1].pos;
    fq1.startroom = GamePaths[pathnum].pathnodes[nodenum + 1].roomnum;
    fq1.p1 = &hit_info.hit_pnt;
    fq1.rad = 0.0f;
    fq1.thisobjnum = -1;
    fq1.ignore_obj_list = NULL;
    fq1.flags = fvi_query_flags_t{};
    fq1.flags.transpoint = true;
    fq1.flags.ignore_render_through_portals = true;
    fvi_FindIntersection(&fq1, &hit_info1);
    if (vm_VectorDistance(&hit_info.hit_pnt, &hit_info1.hit_pnt) > .005) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot move point.  No line of sight from the next node to the new position.");
      return -1;
    }
  }

  GamePaths[pathnum].pathnodes[nodenum].pos = hit_info.hit_pnt;
  GamePaths[pathnum].pathnodes[nodenum].roomnum = hit_info.hit_room;
  return 0;
}

int MovePathNode(int pathnum, int nodenum, vector3 *delta_pos) {
  vector3 attempted_pos = GamePaths[pathnum].pathnodes[nodenum].pos + *delta_pos;
  return MovePathNodeToPos(pathnum, nodenum, &attempted_pos);
}

int GetNextPath(int n) {
  if (GamePaths.empty())
    return -1;
  Q_ASSERT(n >= 0 && n < static_cast<int>(GamePaths.size()));
  return static_cast<int>(GamePaths.next(static_cast<size_t>(n)).value_or(-1));
}

int GetPrevPath(int n) {
  if (GamePaths.empty())
    return -1;
  Q_ASSERT(n >= 0 && n < static_cast<int>(GamePaths.size()));
  return static_cast<int>(GamePaths.prev(static_cast<size_t>(n)).value_or(-1));
}

int GetFirstPath() {
  for (size_t i = 0; i < GamePaths.size(); i++)
    if (GamePaths.is_used(i))
      return i;
  return -1;
}
