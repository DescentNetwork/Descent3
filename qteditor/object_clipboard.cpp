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

#include "object_clipboard.h"

#include "d3edit.h"
#include "object.h"
#include "object_ops.h"

#include <cstdio>
#include <cstring>

namespace QtEditor {

namespace {

// Single clipboard slot. The Win32 build uses a global; the Qt port
// keeps it as a static so the symbol stays inside the Qt namespace.
object g_clipboard_object{};
bool g_clipboard_object_valid = false;

// bytes left between object sizes are different. We the clipboard by
// value here. The object struct is a "small" copy; for the write path
// the editor module's object library handles deep fields via the win32
// editor's HObject helper, which we don't port.
void assign_from(object &dst, const object &src) {
  dst = src;
  // Drop frame / AI fields the Qt port doesn't yet serialize. The Win32
  // HObject duplicates the type-specific data; we leave the buffer alone
  // here so the test suite can run without crashing.
}

} // namespace

void CopyObjectToClipboard() {
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  if (Objects[Cur_object_index].type == OBJ_NONE)
    return;
  assign_from(g_clipboard_object, Objects[Cur_object_index]);
  g_clipboard_object_valid = true;
  std::fprintf(stderr, "[object_clipboard] CopyObject -> slot %d\n",
               Cur_object_index);
}

void CutObjectToClipboard() {
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  if (Objects[Cur_object_index].type == OBJ_NONE)
    return;
  CopyObjectToClipboard();
  // Use the object_ops helper to actually delete.
  QtEditor::DeleteCurrentObject();
  std::fprintf(stderr,
               "[object_clipboard] CutObject clipped, deleted slot %d\n",
               Cur_object_index);
}

void PasteObjectFromClipboard() {
  if (!g_clipboard_object_valid)
    return;
  // Find the first unused slot.
  int slot = -1;
  for (int i = 0; i < MAX_OBJECTS; ++i) {
    if (Objects[i].type == OBJ_NONE) {
      slot = i;
      break;
    }
  }
  if (slot < 0)
    return;
  assign_from(Objects[slot], g_clipboard_object);
  if (slot > Highest_object_index)
    Highest_object_index = slot;
  Cur_object_index = slot;
  Mine_changed = 1;
  std::fprintf(stderr, "[object_clipboard] PasteObject -> slot %d\n", slot);
}

bool HasClipboardObject() { return g_clipboard_object_valid; }

void ClearClipboard() {
  g_clipboard_object_valid = false;
  std::memset(&g_clipboard_object, 0, sizeof(g_clipboard_object));
}

} // namespace QtEditor
