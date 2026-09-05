/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 *
 * Qt port: Definitions for globals and render helpers that are referenced by
 * the core object/render code but whose subsystems (fireballs, weapons,
 * terrain lighting, AI rendering bookkeeping) are not ported. These allow the
 * editor to link; the functions are no-ops and the globals default to neutral
 * values so the non-rendering editor functionality is unaffected.
 */

#include "AIMain.h"
#include "bitmap.h"
#include "fireball.h"
#include "player.h"
#include "renderer.h"
#include "terrain.h"
#include "viseffect.h"
#include "weapon.h"

#include <cstdint>

uint8_t Outline_release_mode = 0;
uint8_t Use_motion_blur = 0;

int AI_NumRendered = 0;
int AI_RenderedList[MAX_OBJECTS] = {0};

int Player_num = 0;

fireball Fireballs[NUM_FIREBALLS] = {};

vis_effect *VisEffects = new vis_effect[MAX_VIS_EFFECTS]();

float GetTerrainDynamicScalar(vector3 *, int) { return 1.0f; }

void DrawColoredDisk(vector3 *, float, float, float, float, float, float, uint8_t, uint8_t) {}

void DrawFireballObject(object *) {}
void DrawWeaponObject(object *) {}

void DrawVisEffect(vis_effect *) {}

void RenderTerrain(uint8_t, int, int, int, int) {}

int bm_format(int) { return BITMAP_FORMAT_1555; }
