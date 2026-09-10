/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 *
 * Qt port: hardware-renderer internal declarations.  Ported from the engine's
 * renderer/HardwareInternal.h (the engine uses `vector`; this build uses
 * `vector3`).
 */

#ifndef __HARDWARE_INTERNAL_H__
#define __HARDWARE_INTERNAL_H__

#define MAX_POINTS_IN_POLY 100

// These structs are for drawing with vertex arrays
struct color_array {
  float r, g, b, a;
};

struct tex_array {
  float s, t, r, w;
};

struct PosColorUVVertex {
  vector3 pos;
  color_array color;
  tex_array uv;
};

struct PosColorUV2Vertex {
  vector3 pos;
  color_array color;
  tex_array uv0;
  tex_array uv1;
};

void FreeTempPoint(g3Point *p);
void InitFreePoints(void);
void ClipLine(g3Point **p0, g3Point **p1, uint8_t codes_or);

// Verify that all the temp points are free, and free them if they are not.
#ifdef _DEBUG
void CheckTempPoints();
#else
#define CheckTempPoints()
#endif

extern int Window_width, Window_height; // the actual integer width & height
extern float Window_w2, Window_h2;      // width,height/2
extern float View_zoom;
extern vector3 View_position, Matrix_scale;
extern matrix View_matrix, Unscaled_matrix;
extern float Far_clip_z;

// For custom clipping plane
extern uint8_t Clip_custom;
extern float Clip_plane_distance;
extern vector3 Clip_plane;

extern float gTransformViewPort[4][4];
extern float gTransformProjection[4][4];
extern float gTransformModelView[4][4];
extern float gTransformFull[4][4];
void g3_UpdateFullTransform();
void g3_ForceTransformRefresh(void);

void rend_TransformSetToPassthru(void);
void rend_TransformSetViewport(int lx, int ty, int width, int height);
void rend_TransformSetProjection(float trans[4][4]);
void rend_TransformSetModelView(float trans[4][4]);

int rend_ReInit();
float rend_GetAlphaMultiplier();

void gpu_SetMultitextureBlendMode(bool state);
void gpu_BindTexture(int handle, int map_type, int slot);
void gpu_RenderPolygon(PosColorUVVertex *vData, uint32_t nv);
void gpu_RenderPolygonUV2(PosColorUV2Vertex *vData, uint32_t nv);
void gpu_DrawFlatPolygon3D(g3Point **p, int nv);
void rend_DrawMultitexturePolygon3D(int handle, g3Point **p, int nv, int map_type);

/*
 * Returns the color to use for a given point, based on lighting and alpha modes.
 */
color_array DeterminePointColor(g3Point const *pnt, bool disableGouraud = false,
                                bool checkTextureQuality = false, bool flatColorForNoLight = false);

#endif