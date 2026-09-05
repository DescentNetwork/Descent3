// Game path subsystem ported from Descent3/gamepath.cpp.
//
// AllocGamePath lives in qteditor/d3_editor_state.cpp; the mini supplies the
// matching free function (declared in mini/editor/EPath.h).  Ported verbatim
// from the original except for the memory-system LOG (the mini has no
// memory-pool debug logging).

#include "EPath.h"

#include "gamepath.h"
#include "mem/mem.h"
#include "string_helpers.h"

// Returns the index of the game path whose name matches, or -1 if not found.
int FindGamePathName(const std::string &name) {
  for (int i = 0; i < MAX_GAME_PATHS; i++) {
    if (GamePaths[i].used && match(GamePaths[i].name, name))
      return i;
  }
  return -1;
}

// Frees gamepath n for future use
void FreeGamePath(int n) {
  if (n < 0 || n >= MAX_GAME_PATHS)
    return;

  if (!GamePaths[n].used)
    return;

  mem_free(GamePaths[n].pathnodes);

  GamePaths[n].num_nodes = 0;
  GamePaths[n].used = 0;
  Num_game_paths--;
}
