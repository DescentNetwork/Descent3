// Game path subsystem ported from Descent3/gamepath.cpp.
//
// AllocGamePath lives in qteditor/d3_editor_state.cpp; the mini supplies the
// matching free function (declared in mini/editor/EPath.h).  Ported verbatim
// from the original except for the memory-system LOG (the mini has no
// memory-pool debug logging).

#include "EPath.h"

#include "gamepath.h"
#include "mem/mem.h"

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