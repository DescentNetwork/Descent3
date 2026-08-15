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

#pragma once

class QWidget;

namespace QtEditor {

// Qt port of the Win32 OpenFileDialog / SaveFileDialog helpers from
// editor/editor.h.
//
// Both helpers take an MFC-style filter string ("X (*.y)|*.y|Z (*.w)|*.w||"),
// present a QFileDialog of the right kind, and on success copy the selected
// path into the caller-supplied `pathname` buffer.
//
// `initialdir`, when provided, names a `char[dirlen]` buffer: it defaults to
// the directory QFileDialog opens in and is overwritten with the directory
// the user was last browsing when the dialog closes, so subsequent calls
// start where the previous one finished (matches the Win32 behaviour).
bool OpenFileDialog(QWidget *parent, const char *filter, char *pathname,
                    char *initialdir = nullptr, int dirlen = 0);
bool SaveFileDialog(QWidget *parent, const char *filter, char *pathname,
                    char *initialdir = nullptr, int dirlen = 0);

// Qt port of the Win32 PrintToDlgItem helper from editor/editor.h. MFC looks
// up the title via numeric ID; in the Qt editor the equivalent lookup is by
// objectName, so this version takes an objectName (the Win32 resource ID
// alias "IDC_*" string), formats with vsnprintf, and writes into any matching
// QLabel / QLineEdit child.
void PrintToDlgItem(QWidget *dlg, const char *id_name, const char *fmt, ...);

} // namespace QtEditor
