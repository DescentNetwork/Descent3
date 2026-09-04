/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 *
 * Qt port: Stub implementations for the 3D/rendering API. The Qt editor does
 * not include the hardware renderer, so these entry points are no-ops that
 * allow the rest of the editor to link.
 */

#include "3d.h"
#include "renderer.h"
#include "grdefs.h"

extern vector3 View_position;
extern matrix Unscaled_matrix;

void rend_SetLighting(light_state) {}
void rend_SetColorModel(color_model) {}
void rend_SetFlatColor(ddgr_color) {}
void rend_SetOverlayType(uint8_t) {}
void rend_DrawLine(int, int, int, int) {}
void rend_SetAlphaType(int8_t) {}
void rend_SetAlphaValue(uint8_t) {}
void rend_SetWrapType(wrap_type) {}
void rend_SetZBufferWriteMask(int) {}
void rend_SetCoplanarPolygonOffset(float) {}
void rend_SetBumpmapReadyState(int, int) {}
void rend_SetTextureType(texture_type) {}
void rend_SetZBias(float) {}
void rend_SetOverlayMap(int) {}
void rend_SetFogState(int8_t) {}
void rend_SetFogBorders(float, float) {}
void rend_SetFogColor(ddgr_color) {}
void rend_SetZBufferState(int8_t) {}
void rend_ClearScreen(ddgr_color) {}
void rend_ClearZBuffer() {}
void rend_SetPixel(ddgr_color, int, int) {}
ddgr_color rend_GetPixel(int, int) { return 0; }
void rend_SetAlphaFactor(float) {}
float rend_GetAlphaFactor() { return 1.0f; }

void g3_StartInstanceMatrix(vector3 *, matrix *) {}
void g3_StartInstanceAngles(vector3 *, angvec *) {}
void g3_DoneInstance() {}
void g3_DrawBitmap(vector3 *, float, float, int, int) {}
void g3_SetTriangulationTest(int) {}
int g3_DrawPoly(int, g3Point **, int, int, g3Codes *) { return 0; }
float g3_CalcPointDepth(vector3 *) { return 0.0f; }
void g3_GetViewPosition(vector3 *vp) { if (vp) *vp = View_position; }
void g3_GetUnscaledMatrix(matrix *mat) { if (mat) *mat = Unscaled_matrix; }
void g3_DrawSpecialLine(g3Point *, g3Point *) {}
void g3_DrawLine(ddgr_color, g3Point *, g3Point *) {}
void g3_DrawSphere(ddgr_color, g3Point *, float) {}
void g3_DrawBox(ddgr_color, g3Point *, float) {}
void g3_SetFarClipZ(float) {}
void g3_SetCustomClipPlane(uint8_t, vector3 *, vector3 *) {}
