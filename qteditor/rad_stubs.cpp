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

// Stubs for Win32 editor symbols referenced by the radiosity system.
// The hemicube form-factor renderer and BOA visibility computation
// call into editor/gameeditor.cpp (StartEditorFrame/EndEditorFrame),
// editor/rad_hemicube.cpp (ShowRadView), and the ddgr surface layer.
// For the Qt port these are no-ops that let the code link.

#include "logger/log.h"
#include "3d.h"
#include "vecmat.h"
#include "gr.h"
#include "Ddgr.h"
#include "BOA.h"


void StartEditorFrame(grViewport *vp, vector3 *view_vec, matrix *id_mat, float zoom) {}

void EndEditorFrame() {}

void ShowRadView() {}

bool ddgr_surf_InitVideo(ddgr_surface *sf) { return false; }
bool ddgr_surf_Create(ddgr_surface *sf) { return false; }
void ddgr_surf_CloseVideo(ddgr_surface *sf) {}
void ddgr_surf_Destroy(ddgr_surface *sf) {}
void ddgr_surf_Clear(ddgr_surface *dsf, ddgr_color col, int l, int t, int w, int h) {}
bool ddgr_surf_Blt(ddgr_surface *dsf, int dx, int dy, ddgr_surface *ssf, int sx, int sy, int sw, int sh) { return false; }
bool ddgr_surf_Lock(ddgr_surface *sf, void **ptr, int *rowsize) {
  if (ptr)
    *ptr = nullptr;
  if (rowsize)
    *rowsize = 0;
  return false;
}
bool ddgr_surf_Unlock(ddgr_surface *sf, void *ptr) { return false; }
void ddgr_surf_AttachHandle(ddgr_surface *sf, unsigned handle) {}
float ddgr_GetAspectRatio() { return 1.0f; }
