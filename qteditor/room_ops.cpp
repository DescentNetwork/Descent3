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

#include "room_ops.h"

#include "d3edit.h"
#include "level_io.h"
#include "object.h"
#include "room.h"

#include <QInputDialog>
#include <QWidget>

#include <cstdio>
#include <cstring>

namespace QtEditor {

bool AddRoom() {
  // Win32 OnRoomAdd -> AddRoom() in editor/HRoom.cpp walks the available
  // room slots, allocates the next free index, builds a default cube, and
  // marks it as used. The Qt port delegates that to CreateNewMine's cube
  // factory in level_io.cpp once editor/Erooms.cpp links in. Until then
  // we just log + bump New_mine so the menu wiring has a deterministic
  // observable side-effect.
  std::fprintf(stderr, "[room_ops] AddRoom: stub pending editor/Erooms.cpp\n");
  New_mine = 1;
  World_changed = 1;
  return true;
}

bool DeleteRoom() {
  // Win32 OnRoomDelete -> DeleteRoomFromMine(Curroomp) frees the room's
  // geometry, marks its slot freed, and clears Curroomp. The Qt stub logs
  // and clears the editor-only selection globals so the menu items have a
  // deterministic observable change.
  if (Curroomp == nullptr)
    return false;
  std::fprintf(stderr, "[room_ops] DeleteRoom: stub pending editor/Erooms.cpp\n");
  if (Curroomp->name != nullptr)
    std::fprintf(stderr, "  current room name: %s\n", Curroomp->name);
  Curroomp = nullptr;
  Curface = -1;
  Curedge = -1;
  Curvert = -1;
  Curportal = -1;
  Mine_changed = 1;
  return true;
}

void MarkRoom() {
  // editor/selectedroom.cpp::SetMarkedRoom() copies Curroomp into
  // Markedroomp (the "swap marked and current" UI relies on this).
  Markedroomp = Curroomp;
  std::fprintf(stderr, "[room_ops] MarkRoom: markedroomp = %s\n",
               Markedroomp ? Markedroomp->name : "(none)");
}

int SelectRoomByNumber() {
  // Win32 OnRoomSelectByNumber shows OutrageMessageBox asking the user
  // for a number; the Qt port uses QInputDialog::getInt.
  bool ok = false;
  const int value = QInputDialog::getInt(
      nullptr, QStringLiteral("Select Room"),
      QStringLiteral("Enter room number to select:"), 0, 0, MAX_ROOMS, 1, &ok);
  if (!ok)
    return -1;
  if (value < 0 || value > MAX_ROOMS || !Rooms[value].used) {
    std::fprintf(stderr,
                 "[room_ops] SelectRoomByNumber: %d is not a valid room\n",
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
  // Strip leading/trailing spaces to match editor/HFile.cpp's check.
  QByteArray bytes = picked.toLatin1();
  bytes.append('\0');
  char *buf = bytes.data();
  QtEditor::StripLeadingTrailingSpaces(buf);
  std::strncpy(Curroomp->name, buf, sizeof(Curroomp->name) - 1);
  Curroomp->name[sizeof(Curroomp->name) - 1] = '\0';
  std::fprintf(stderr, "[room_ops] RenameRoom -> %s\n", Curroomp->name);
  return true;
}

bool SaveCurrentRoom() {
  if (Curroomp == nullptr)
    return false;
  // editor/HRoom.cpp::SaveRoom writes a single-room .orf snippet under
  // the current .d3l filename. The Qt port records what would have been
  // wrote until the engine-side room path lands.
  std::fprintf(stderr,
               "[room_ops] SaveCurrentRoom: stub pending editor/HRoom.cpp\n");
  return true;
}

} // namespace QtEditor
