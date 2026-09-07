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

#include "terrain_keypad.h"
#include "ui_terrainkeypad.h"

#include "logger/log.h"

#include <algorithm>
#include <cstring>

#include <QMessageBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>

#include "d3edit.h"
#include "config.h"
#include "terrain.h"
#include "vecmat_external.h"
#include "object.h"
#include "room.h"
#include "gamepath.h"
#include "mem.h"


#include "editline_dialog.h"
#include "selectrange_dialog.h"

static int ColorMode_Sky = 0;
static int ColorMode_Horizon = 1;
static int ColorMode_Fog = 2;
static int ColorMode_Sat = 3;

TerrainKeypad::TerrainKeypad(QWidget *parent)
    : QDialog(parent), ui(new Ui::TerrainKeypad)
{
  ui->setupUi(this);

  connect(ui->IDC_TERRPAD_MOVE_UP, &QPushButton::clicked, this, &TerrainKeypad::onMoveUp);
  connect(ui->IDC_TERRPAD_MOVE_DOWN, &QPushButton::clicked, this, &TerrainKeypad::onMoveDown);
  connect(ui->IDC_TERRPAD_RAISE10, &QPushButton::clicked, this, &TerrainKeypad::onRaise10);
  connect(ui->IDC_TERRPAD_LOWER10, &QPushButton::clicked, this, &TerrainKeypad::onLower10);
  connect(ui->IDC_TERRPAD_SELECT_NONE, &QPushButton::clicked, this, &TerrainKeypad::onSelectNone);
  connect(ui->IDC_TERR_SELECT_ALL, &QPushButton::clicked, this, &TerrainKeypad::onSelectAll);
  connect(ui->IDC_TERRPAD_MAKE_MAX, &QPushButton::clicked, this, &TerrainKeypad::onMakeMax);
  connect(ui->IDC_TERRPAD_MAKE_MIN, &QPushButton::clicked, this, &TerrainKeypad::onMakeMin);
  connect(ui->IDC_TERRPAD_MAKE_ZERO, &QPushButton::clicked, this, &TerrainKeypad::onMakeZero);
  connect(ui->IDC_TERRPAD_FILL_AREA, &QPushButton::clicked, this, &TerrainKeypad::onFillArea);
  connect(ui->IDC_TERRPAD_PYRAMID, &QPushButton::clicked, this, &TerrainKeypad::onPyramid);
  connect(ui->IDC_TERRPAD_PANCAKES, &QPushButton::clicked, this, &TerrainKeypad::onPancakes);
  connect(ui->IDC_TERRPAD_RENORMALIZE, &QPushButton::clicked, this, &TerrainKeypad::onRenormalize);
  connect(ui->IDC_TERRPAD_ROT_TEXTURE, &QPushButton::clicked, this, &TerrainKeypad::onRotTexture);
  connect(ui->IDC_TERRPAD_REDO_TOPMAP, &QPushButton::clicked, this, &TerrainKeypad::onRedoTopmap);
  connect(ui->IDC_TILE_MORE, &QPushButton::clicked, this, &TerrainKeypad::onTileMore);
  connect(ui->IDC_TILE_LESS, &QPushButton::clicked, this, &TerrainKeypad::onTileLess);
  connect(ui->IDC_TERRPAD_SELECTRANGE, &QPushButton::clicked, this, [this]() {
    SelectRangeDialog dlg(this);
    dlg.exec();
    World_changed = true;
  });
  connect(ui->IDC_TERR_MORE_MOONS, &QPushButton::clicked, this, &TerrainKeypad::onMoreMoons);
  connect(ui->IDC_TERR_LESS_MOONS, &QPushButton::clicked, this, &TerrainKeypad::onLessMoons);
  connect(ui->IDC_TERR_NEXT_MOON, &QPushButton::clicked, this, &TerrainKeypad::onNextMoon);
  connect(ui->IDC_TERR_PREV_MOON, &QPushButton::clicked, this, &TerrainKeypad::onPrevMoon);
  connect(ui->IDC_MOVE_SAT_UP, &QPushButton::clicked, this, &TerrainKeypad::onMoveSatUp);
  connect(ui->IDC_MOVE_SAT_DOWN, &QPushButton::clicked, this, &TerrainKeypad::onMoveSatDown);
  connect(ui->IDC_MOVE_SAT_LEFT, &QPushButton::clicked, this, &TerrainKeypad::onMoveSatLeft);
  connect(ui->IDC_MOVE_SAT_RIGHT, &QPushButton::clicked, this, &TerrainKeypad::onMoveSatRight);
  connect(ui->IDC_TERR_MOVE_MOON, &QPushButton::clicked, this, &TerrainKeypad::onMoveMoonCloser);
  connect(ui->IDC_TERR_MOVE_MOON_AWAY, &QPushButton::clicked, this, &TerrainKeypad::onMoveMoonFarther);
  connect(ui->IDC_SKY_NEARER, &QPushButton::clicked, this, &TerrainKeypad::onSkyNearer);
  connect(ui->IDC_SKY_FARTHER, &QPushButton::clicked, this, &TerrainKeypad::onSkyFarther);
  connect(ui->IDC_TERR_RANDOMIZE_SKY, &QPushButton::clicked, this, &TerrainKeypad::onRandomizeSky);
  connect(ui->IDC_SMOOTH_TERRAIN, &QPushButton::clicked, this, &TerrainKeypad::onSmoothTerrain);
  connect(ui->IDC_DROP_TERRAIN, &QPushButton::clicked, this, &TerrainKeypad::onDropTerrain);
  connect(ui->IDC_TERRAIN_OCCLUSION, &QPushButton::clicked, this, &TerrainKeypad::onTerrainOcclusion);
  connect(ui->IDC_TOGGLE_VISIBILITY, &QPushButton::clicked, this, &TerrainKeypad::onToggleVisibility);

  connect(ui->IDC_FOG_DISTANCE_EDIT, &QLineEdit::editingFinished, this, &TerrainKeypad::onFogDistanceEdited);
  connect(ui->IDC_PIXEL_ERROR_EDIT, &QLineEdit::editingFinished, this, &TerrainKeypad::onPixelErrorEdited);
  connect(ui->IDC_FOG_SCALAR_EDIT, &QLineEdit::editingFinished, this, &TerrainKeypad::onFogScalarEdited);
  connect(ui->IDC_DAMAGE_PER_SEC_EDIT, &QLineEdit::editingFinished, this, &TerrainKeypad::onDamagePerSecEdited);
  connect(ui->IDC_ROTATE_SPEED_EDIT, &QLineEdit::editingFinished, this, &TerrainKeypad::onRotateSpeedEdited);
  connect(ui->IDC_SKY_RED_EDIT, &QLineEdit::editingFinished, this, &TerrainKeypad::onSkyRedEdited);
  connect(ui->IDC_SKY_GREEN_EDIT, &QLineEdit::editingFinished, this, &TerrainKeypad::onSkyGreenEdited);
  connect(ui->IDC_SKY_BLUE_EDIT, &QLineEdit::editingFinished, this, &TerrainKeypad::onSkyBlueEdited);

  connect(ui->IDC_STARS_CHECK, &QCheckBox::toggled, this, &TerrainKeypad::onStarsToggled);
  connect(ui->IDC_SATELLITE_CHECK, &QCheckBox::toggled, this, &TerrainKeypad::onSatelliteToggled);
  connect(ui->IDC_TEXTURE_SKY, &QCheckBox::toggled, this, &TerrainKeypad::onTexturedSkyToggled);
  connect(ui->IDC_USE_FOG, &QCheckBox::toggled, this, &TerrainKeypad::onUseFogToggled);
  connect(ui->IDC_USE_HALO, &QCheckBox::toggled, this, &TerrainKeypad::onUseHaloToggled);
  connect(ui->IDC_USE_ATMOSPHERE, &QCheckBox::toggled, this, &TerrainKeypad::onUseAtmosphereToggled);
  connect(ui->IDC_ROTATE_STARS, &QCheckBox::toggled, this, &TerrainKeypad::onRotateStarsToggled);
  connect(ui->IDC_ROTATE_SKY, &QCheckBox::toggled, this, &TerrainKeypad::onRotateSkyToggled);
  connect(ui->IDC_SHOW_TERRAIN, &QCheckBox::toggled, this, &TerrainKeypad::onShowTerrainToggled);
  connect(ui->IDC_FLAT_SHADE_TERRAIN_CHECK, &QCheckBox::toggled, this, &TerrainKeypad::onFlatShadeToggled);
  connect(ui->IDC_NO_LOD_ENGINE, &QCheckBox::toggled, this, &TerrainKeypad::onNoLodToggled);
  connect(ui->IDC_TERRAIN_2D, &QCheckBox::toggled, this, &TerrainKeypad::onTerrain2dToggled);
  connect(ui->IDC_SHOW_INVISIBLE, &QCheckBox::toggled, this, &TerrainKeypad::onShowInvisibleToggled);
  connect(ui->IDC_NO_EXT_ROOMS_OBJS, &QCheckBox::toggled, this, &TerrainKeypad::onNoExtRoomsObjsToggled);

  connect(ui->IDC_SKY_RADIO, &QRadioButton::clicked, this, &TerrainKeypad::onSkyRadio);
  connect(ui->IDC_HORIZON_RADIO, &QRadioButton::clicked, this, &TerrainKeypad::onHorizonRadio);
  connect(ui->IDC_FOG_RADIO, &QRadioButton::clicked, this, &TerrainKeypad::onFogRadio);
  connect(ui->IDC_SATELLITE_RADIO, &QRadioButton::clicked, this, &TerrainKeypad::onSatelliteRadio);

  updateDialog();
}

TerrainKeypad::~TerrainKeypad() { delete ui; }

int TerrainKeypad::currentSat() const { return m_currentSatellite; }

void TerrainKeypad::updateDialog() {
  const bool hasTerrain = (Num_terrain_selected > 0);
  ui->IDC_TERRPAD_MOVE_UP->setEnabled(hasTerrain);
  ui->IDC_TERRPAD_MOVE_DOWN->setEnabled(hasTerrain);
  ui->IDC_TERRPAD_RAISE10->setEnabled(hasTerrain);
  ui->IDC_TERRPAD_LOWER10->setEnabled(hasTerrain);
  ui->IDC_TERRPAD_MAKE_MAX->setEnabled(hasTerrain);
  ui->IDC_TERRPAD_MAKE_MIN->setEnabled(hasTerrain);
  ui->IDC_TERRPAD_MAKE_ZERO->setEnabled(hasTerrain);
  ui->IDC_TERRPAD_FILL_AREA->setEnabled(hasTerrain);

  ui->IDC_NUM_MOONS_STATIC->setText(QString("Num of sats:%1").arg(Terrain_sky.num_satellites));
  ui->IDC_CUR_MOON_STATIC->setText(QString("Current sat:%1").arg(m_currentSatellite));

  {
    QLineEdit *e = ui->IDC_SKY_RED_EDIT;
    int r = 0, g = 0, b = 0;
    if (m_ccMode != ColorMode_Sat) {
      uint32_t color = (m_ccMode == ColorMode_Sky) ? Terrain_sky.sky_color
                      : (m_ccMode == ColorMode_Horizon) ? Terrain_sky.horizon_color
                      : Terrain_sky.fog_color;
      r = GR_COLOR_RED(color);
      g = GR_COLOR_GREEN(color);
      b = GR_COLOR_BLUE(color);
      e->setText(QString::number(r));
    } else {
      e->setText(QString::number(Terrain_sky.satellite_r[m_currentSatellite], 'f', 2));
    }
    ui->IDC_SKY_GREEN_EDIT->setText(m_ccMode != ColorMode_Sat ? QString::number(g) : QString::number(Terrain_sky.satellite_g[m_currentSatellite], 'f', 2));
    ui->IDC_SKY_BLUE_EDIT->setText(m_ccMode != ColorMode_Sat ? QString::number(b) : QString::number(Terrain_sky.satellite_b[m_currentSatellite], 'f', 2));
  }

  ui->IDC_FOG_DISTANCE_EDIT->setText(QString::number(Detail_settings.Terrain_render_distance / TERRAIN_SIZE, 'f', 1));
  ui->IDC_PIXEL_ERROR_EDIT->setText(QString::number(Detail_settings.Pixel_error, 'f', 1));
  ui->IDC_FOG_SCALAR_EDIT->setText(QString::number(Terrain_sky.fog_scalar, 'f', 3));
  ui->IDC_DAMAGE_PER_SEC_EDIT->setText(QString::number(Terrain_sky.damage_per_second, 'f', 3));
  ui->IDC_ROTATE_SPEED_EDIT->setText(QString::number(Terrain_sky.rotate_rate, 'f', 2));

  ui->IDC_STARS_CHECK->setChecked(Terrain_sky.flags.stars);
  ui->IDC_SATELLITE_CHECK->setChecked(Terrain_sky.flags.satellites);
  ui->IDC_TEXTURE_SKY->setChecked(Terrain_sky.textured);
  ui->IDC_USE_FOG->setChecked(Terrain_sky.flags.fog);
  ui->IDC_ROTATE_STARS->setChecked(Terrain_sky.flags.rotate_stars);
  ui->IDC_ROTATE_SKY->setChecked(Terrain_sky.flags.rotate_sky);
  if (m_currentSatellite >= 0 && m_currentSatellite < 5) {
    ui->IDC_USE_HALO->setChecked(Terrain_sky.satellite_flags[m_currentSatellite].halo);
    ui->IDC_USE_ATMOSPHERE->setChecked(Terrain_sky.satellite_flags[m_currentSatellite].atmosphere);
  }
  ui->IDC_SHOW_TERRAIN->setChecked(D3EditState.terrain_dots);
  ui->IDC_FLAT_SHADE_TERRAIN_CHECK->setChecked(D3EditState.terrain_flat_shade);
  ui->IDC_NO_LOD_ENGINE->setChecked(Editor_LOD_engine_off);
  ui->IDC_TERRAIN_2D->setChecked(Flat_terrain);
  ui->IDC_SHOW_INVISIBLE->setChecked(Show_invisible_terrain);
  ui->IDC_NO_EXT_ROOMS_OBJS->setChecked(!Terrain_render_ext_room_objs);

  ui->IDC_SKY_RADIO->setChecked(m_ccMode == ColorMode_Sky);
  ui->IDC_HORIZON_RADIO->setChecked(m_ccMode == ColorMode_Horizon);
  ui->IDC_FOG_RADIO->setChecked(m_ccMode == ColorMode_Fog);
  ui->IDC_SATELLITE_RADIO->setChecked(m_ccMode == ColorMode_Sat);
}

void TerrainKeypad::changeSelectedHeights(int delta, bool toAbsolute, int absoluteValue) {
  const int count = TERRAIN_WIDTH * TERRAIN_DEPTH;
  for (int i = 0; i < count; i++) {
    if (!TerrainSelected[i])
      continue;
    int newY = Terrain_seg[i].ypos;
    if (toAbsolute)
      newY = absoluteValue;
    else
      newY += delta;
    if (newY < 0) newY = 0;
    if (newY > 255) newY = 255;
    Terrain_seg[i].ypos = newY;
    Terrain_seg[i].y = (float)(newY * TERRAIN_HEIGHT_INCREMENT);
  }
  World_changed = true;
}

void TerrainKeypad::onMoveUp() { changeSelectedHeights(1); }
void TerrainKeypad::onMoveDown() { changeSelectedHeights(-1); }
void TerrainKeypad::onRaise10() { changeSelectedHeights(10); }
void TerrainKeypad::onLower10() { changeSelectedHeights(-10); }

void TerrainKeypad::onSelectNone() {
  const int count = TERRAIN_WIDTH * TERRAIN_DEPTH;
  for (int i = 0; i < count; i++)
    TerrainSelected[i] = 0;
  Num_terrain_selected = 0;
  World_changed = true;
  updateDialog();
}

void TerrainKeypad::onSelectAll() {
  const int count = TERRAIN_WIDTH * TERRAIN_DEPTH;
  for (int i = 0; i < count; i++)
    TerrainSelected[i] = 1;
  Num_terrain_selected = count;
  World_changed = true;
  updateDialog();
}

void TerrainKeypad::onMakeMax() {
  int max_so_far = 0;
  const int count = TERRAIN_WIDTH * TERRAIN_DEPTH;
  for (int i = 0; i < count; i++)
    if (TerrainSelected[i] && Terrain_seg[i].ypos > max_so_far)
      max_so_far = Terrain_seg[i].ypos;
  changeSelectedHeights(0, true, max_so_far);
}

void TerrainKeypad::onMakeMin() {
  int min_so_far = 255;
  const int count = TERRAIN_WIDTH * TERRAIN_DEPTH;
  for (int i = 0; i < count; i++)
    if (TerrainSelected[i] && Terrain_seg[i].ypos < min_so_far)
      min_so_far = Terrain_seg[i].ypos;
  changeSelectedHeights(0, true, min_so_far);
}

void TerrainKeypad::onMakeZero() { changeSelectedHeights(0, true, 0); }

void TerrainKeypad::onFillArea() {
  const int count = TERRAIN_WIDTH * TERRAIN_DEPTH;
  for (int i = 0; i < count; i++)
    if (TerrainSelected[i])
      Terrain_tex_seg[Terrain_seg[i].texseg_index].tex_index = D3EditState.texdlg_texture;
  World_changed = true;
}

void TerrainKeypad::onMoreMoons() {
  if (Terrain_sky.num_satellites < 5)
    Terrain_sky.num_satellites++;
  TV_changed = true;
  updateDialog();
}

void TerrainKeypad::onLessMoons() {
  if (Terrain_sky.num_satellites > 0)
    Terrain_sky.num_satellites--;
  TV_changed = true;
  updateDialog();
}

void TerrainKeypad::onNextMoon() {
  m_currentSatellite = (m_currentSatellite + 1) % 5;
  updateDialog();
}

void TerrainKeypad::onPrevMoon() {
  m_currentSatellite--;
  if (m_currentSatellite < 0)
    m_currentSatellite = 4;
  updateDialog();
}

void TerrainKeypad::moveSat(int pitch, int heading) {
  int n = m_currentSatellite;
  matrix rot_matrix;
  vm_AnglesToMatrix(&rot_matrix, pitch, heading, 0);
  vector3 sat_vec = Terrain_sky.satellite_vectors[n] - Viewer_object->pos;
  float mag = vm_GetMagnitude(&sat_vec);
  vm_NormalizeVector(&sat_vec);
  vector3 rot_vec;
  vm_MatrixMulVector(&rot_vec, &sat_vec, &rot_matrix);
  Terrain_sky.satellite_vectors[n] = Viewer_object->pos + (rot_vec * mag);
  TV_changed = true;
}

void TerrainKeypad::onMoveSatUp() { moveSat(1500, 0); }
void TerrainKeypad::onMoveSatDown() { moveSat(64000, 0); }
void TerrainKeypad::onMoveSatLeft() { moveSat(0, 64000); }
void TerrainKeypad::onMoveSatRight() { moveSat(0, 1500); }

void TerrainKeypad::onMoveMoonCloser() {
  Terrain_sky.satellite_size[m_currentSatellite] *= 1.1f;
  TV_changed = true;
}

void TerrainKeypad::onMoveMoonFarther() {
  Terrain_sky.satellite_size[m_currentSatellite] *= 0.9f;
  TV_changed = true;
}

void TerrainKeypad::onRenormalize() {
  LOG_INFO("Building terrain normals...\n");
  Terrain_checksum = -1;
  BuildMinMaxTerrain();
  BuildTerrainNormals();
  State_changed = true;
}

void TerrainKeypad::onRotTexture() {
  uint8_t touched[TERRAIN_TEX_WIDTH * TERRAIN_TEX_DEPTH];
  memset(touched, 0, sizeof(touched));
  const int count = TERRAIN_WIDTH * TERRAIN_DEPTH;
  for (int i = 0; i < count; i++) {
    if (TerrainSelected[i] && !touched[Terrain_seg[i].texseg_index]) {
      int val = Terrain_tex_seg[Terrain_seg[i].texseg_index].rotation & 0x0F;
      val = (val + 1) % 4;
      Terrain_tex_seg[Terrain_seg[i].texseg_index].rotation &= ~0x0F;
      Terrain_tex_seg[Terrain_seg[i].texseg_index].rotation |= val;
      touched[Terrain_seg[i].texseg_index] = 1;
    }
  }
  World_changed = true;
  TV_changed = true;
}

void TerrainKeypad::onRedoTopmap() { World_changed = true; }

void TerrainKeypad::onTileMore() {
  uint32_t flags_raw = 0;
  std::memcpy(&flags_raw, &Terrain_sky.flags, sizeof(flags_raw));
  if (Terrain_sky.radius > 500) {
    SetupSky(Terrain_sky.radius - 500, flags_raw);
    TV_changed = true;
  }
}

void TerrainKeypad::onTileLess() {
  uint32_t flags_raw = 0;
  std::memcpy(&flags_raw, &Terrain_sky.flags, sizeof(flags_raw));
  SetupSky(Terrain_sky.radius + 500, flags_raw);
  TV_changed = true;
}

void TerrainKeypad::onSkyNearer() {
  uint32_t flags_raw = 0;
  std::memcpy(&flags_raw, &Terrain_sky.flags, sizeof(flags_raw));
  if (Terrain_sky.radius > 500) {
    SetupSky(Terrain_sky.radius - 500, flags_raw);
    TV_changed = true;
  }
}

void TerrainKeypad::onSkyFarther() {
  uint32_t flags_raw = 0;
  std::memcpy(&flags_raw, &Terrain_sky.flags, sizeof(flags_raw));
  SetupSky(Terrain_sky.radius + 500, flags_raw);
  TV_changed = true;
}

void TerrainKeypad::onRandomizeSky() {
  uint32_t flags_raw = 0;
  std::memcpy(&flags_raw, &Terrain_sky.flags, sizeof(flags_raw));
  SetupSky(Terrain_sky.radius, flags_raw, 1);
  TV_changed = true;
}

void TerrainKeypad::onPyramid() {
  int left = TERRAIN_WIDTH, right = 0;
  int top = TERRAIN_DEPTH, bottom = 0;
  int max_so_far = 0, min_so_far = 999;
  const int count = TERRAIN_WIDTH * TERRAIN_DEPTH;

  for (int i = 0; i < count; i++) {
    if (!TerrainSelected[i]) continue;
    if (Terrain_seg[i].ypos > max_so_far) max_so_far = Terrain_seg[i].ypos;
    if (Terrain_seg[i].ypos < min_so_far) min_so_far = Terrain_seg[i].ypos;
    int x = i % TERRAIN_WIDTH;
    int y = i / TERRAIN_WIDTH;
    if (x > right) right = x;
    if (x < left) left = x;
    if (y > bottom) bottom = y;
    if (y < top) top = y;
  }

  int w = right - left;
  int h = bottom - top;
  int hdiff = max_so_far - min_so_far;
  if (hdiff == 0)
    return;

  fix xstep = IntToFix(hdiff) / (w / 2);
  fix ystep = IntToFix(hdiff) / (h / 2);

  for (int quadrant = 0; quadrant < 4; quadrant++) {
    for (int i = 0; i <= h / 2; i++) {
      for (int t = 0; t <= w / 2; t++) {
        int xa = FixToInt(xstep * t);
        int ya = FixToInt(ystep * i);
        int a = std::max(xa, ya);
        int seg;
        switch (quadrant) {
          case 0: seg = ((top + i) * TERRAIN_WIDTH) + (t + left); break;
          case 1: seg = ((top + i) * TERRAIN_WIDTH) + (right - t); break;
          case 2: seg = ((bottom - i) * TERRAIN_WIDTH) + (left + t); break;
          default: seg = ((bottom - i) * TERRAIN_WIDTH) + (right - t); break;
        }
        Terrain_seg[seg].ypos = min_so_far + a;
        Terrain_seg[seg].y = (float)((min_so_far + a) * TERRAIN_HEIGHT_INCREMENT);
      }
    }
  }
  World_changed = true;
}

void TerrainKeypad::onPancakes() {
  onPyramid();
}

void TerrainKeypad::onSmoothTerrain() {
  if (Num_terrain_selected == 0)
    return;

  const int w = TERRAIN_WIDTH;
  const int h = TERRAIN_DEPTH;
  std::vector<uint8_t> src(w * h);

  for (int i = 0; i < w * h; i++)
    src[i] = Terrain_seg[i].ypos;

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      if (!TerrainSelected[y * w + x])
        continue;
      int total = 0, num = 0;
      total += src[y * w + x]; num++;
      if (x != 0) { total += src[y * w + (x - 1)]; num++; }
      if (x != w - 1) { total += src[y * w + (x + 1)]; num++; }
      if (y != 0) { total += src[(y - 1) * w + x]; num++; }
      if (y != h - 1) { total += src[(y + 1) * w + x]; num++; }
      if (x != 0 && y != 0) { total += src[(y - 1) * w + (x - 1)]; num++; }
      if (x != 0 && y != h - 1) { total += src[(y + 1) * w + (x - 1)]; num++; }
      if (x != w - 1 && y != 0) { total += src[(y - 1) * w + (x + 1)]; num++; }
      if (x != w - 1 && y != h - 1) { total += src[(y + 1) * w + (x + 1)]; num++; }
      if (num > 0)
        Terrain_seg[y * w + x].ypos = total / num;
    }
  }

  BuildMinMaxTerrain();
  QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Terrain smoothed!");
  World_changed = true;
}

void TerrainKeypad::onDropTerrain() {
  int lowest = INT_MAX, highest = 0;
  for (int i = 0; i < TERRAIN_WIDTH * TERRAIN_DEPTH; i++) {
    if (Terrain_seg[i].ypos < lowest) lowest = Terrain_seg[i].ypos;
    if (Terrain_seg[i].ypos > highest) highest = Terrain_seg[i].ypos;
  }

  int desired;
  if (!InputNumber(&desired, "Raise/Lower Terrain", "Enter the desired height for the lowest terrain point:"))
    return;
  if (desired < 0 || desired > 255)
    return;
  if (((highest - lowest) + desired) > 255)
    return;

  int delta = desired - lowest;
  for (int i = 0; i < TERRAIN_WIDTH * TERRAIN_DEPTH; i++)
    Terrain_seg[i].ypos += delta;

  BuildMinMaxTerrain();
  float delta_y = (float)(delta * TERRAIN_HEIGHT_INCREMENT);

  for (int r = 0; r <= Highest_room_index; r++) {
    if (!Rooms[r].used) continue;
    for (int v = 0; v < Rooms[r].num_verts; v++)
      Rooms[r].verts[v].y() += delta_y;
  }

  for (int o = 0; o <= Highest_object_index; o++) {
    if (Objects[o].type != OBJ_NONE) {
      vector3 new_pos = Objects[o].pos;
      new_pos.y() += delta_y;
      ObjSetPos(Objects[o], new_pos, Objects[o].roomnum, nullptr, false);
    }
  }

  for (int p = 0; p < Num_game_paths; p++)
    for (int n = 0; n < GamePaths[p].num_nodes; n++)
      GamePaths[p].pathnodes[n].pos.y() += delta_y;

  World_changed = true;
}

void TerrainKeypad::onTerrainOcclusion() {
  BuildMinMaxTerrain();
  BuildTerrainNormals();
  TV_changed = true;
}

void TerrainKeypad::onToggleVisibility() {
  const int count = TERRAIN_WIDTH * TERRAIN_DEPTH;
  for (int i = 0; i < count; i++) {
    if (TerrainSelected[i]) {
      Terrain_seg[i].flags.invisible = ~Terrain_seg[i].flags.invisible;
      World_changed = true;
    }
  }
  if (World_changed)
    GenerateLODDeltas();
}

void TerrainKeypad::onSkyRadio() { m_ccMode = ColorMode_Sky; updateDialog(); }
void TerrainKeypad::onHorizonRadio() { m_ccMode = ColorMode_Horizon; updateDialog(); }
void TerrainKeypad::onFogRadio() { m_ccMode = ColorMode_Fog; updateDialog(); }
void TerrainKeypad::onSatelliteRadio() { m_ccMode = ColorMode_Sat; updateDialog(); }

static uint32_t makeRGB(int r, int g, int b) {
  return GR_RGB(r, g, b);
}

void TerrainKeypad::onSkyRedEdited() {
  QLineEdit *e = ui->IDC_SKY_RED_EDIT;
  if (!e) return;
  float fv = e->text().toFloat();
  if (m_ccMode != ColorMode_Sat) {
    int r = std::clamp((int)fv, 0, 255);
    int g, b;
    if (m_ccMode == ColorMode_Sky) { g = GR_COLOR_GREEN(Terrain_sky.sky_color); b = GR_COLOR_BLUE(Terrain_sky.sky_color); Terrain_sky.sky_color = makeRGB(r, g, b); }
    else if (m_ccMode == ColorMode_Horizon) { g = GR_COLOR_GREEN(Terrain_sky.horizon_color); b = GR_COLOR_BLUE(Terrain_sky.horizon_color); Terrain_sky.horizon_color = makeRGB(r, g, b); }
    else { g = GR_COLOR_GREEN(Terrain_sky.fog_color); b = GR_COLOR_BLUE(Terrain_sky.fog_color); Terrain_sky.fog_color = makeRGB(r, g, b); }
  } else {
    if (fv < 0) fv = 0;
    Terrain_sky.satellite_r[m_currentSatellite] = fv;
  }
  World_changed = true;
  updateDialog();
}

void TerrainKeypad::onSkyGreenEdited() {
  QLineEdit *e = ui->IDC_SKY_GREEN_EDIT;
  if (!e) return;
  float fv = e->text().toFloat();
  if (m_ccMode != ColorMode_Sat) {
    int g = std::clamp((int)fv, 0, 255);
    int r, b;
    if (m_ccMode == ColorMode_Sky) { r = GR_COLOR_RED(Terrain_sky.sky_color); b = GR_COLOR_BLUE(Terrain_sky.sky_color); Terrain_sky.sky_color = makeRGB(r, g, b); }
    else if (m_ccMode == ColorMode_Horizon) { r = GR_COLOR_RED(Terrain_sky.horizon_color); b = GR_COLOR_BLUE(Terrain_sky.horizon_color); Terrain_sky.horizon_color = makeRGB(r, g, b); }
    else { r = GR_COLOR_RED(Terrain_sky.fog_color); b = GR_COLOR_BLUE(Terrain_sky.fog_color); Terrain_sky.fog_color = makeRGB(r, g, b); }
  } else {
    if (fv < 0) fv = 0;
    Terrain_sky.satellite_g[m_currentSatellite] = fv;
  }
  World_changed = true;
  updateDialog();
}

void TerrainKeypad::onSkyBlueEdited() {
  QLineEdit *e = ui->IDC_SKY_BLUE_EDIT;
  if (!e) return;
  float fv = e->text().toFloat();
  if (m_ccMode != ColorMode_Sat) {
    int b = std::clamp((int)fv, 0, 255);
    int r, g;
    if (m_ccMode == ColorMode_Sky) { r = GR_COLOR_RED(Terrain_sky.sky_color); g = GR_COLOR_GREEN(Terrain_sky.sky_color); Terrain_sky.sky_color = makeRGB(r, g, b); }
    else if (m_ccMode == ColorMode_Horizon) { r = GR_COLOR_RED(Terrain_sky.horizon_color); g = GR_COLOR_GREEN(Terrain_sky.horizon_color); Terrain_sky.horizon_color = makeRGB(r, g, b); }
    else { r = GR_COLOR_RED(Terrain_sky.fog_color); g = GR_COLOR_GREEN(Terrain_sky.fog_color); Terrain_sky.fog_color = makeRGB(r, g, b); }
  } else {
    if (fv < 0) fv = 0;
    Terrain_sky.satellite_b[m_currentSatellite] = fv;
  }
  World_changed = true;
  updateDialog();
}

void TerrainKeypad::onFogDistanceEdited() {
  {
    QLineEdit *edit = ui->IDC_FOG_DISTANCE_EDIT;
    float predist = edit->text().toFloat();
    if (predist < 20) predist = 20;
    if (predist > 200) predist = 200;
    Detail_settings.Terrain_render_distance = predist * TERRAIN_SIZE;
    World_changed = true;
  }
}

void TerrainKeypad::onPixelErrorEdited() {
  {
    QLineEdit *edit = ui->IDC_PIXEL_ERROR_EDIT;
    float err = edit->text().toFloat();
    if (err < 0) err = 0;
    if (err > 64) err = 64;
    Detail_settings.Pixel_error = err;
    World_changed = true;
  }
}

void TerrainKeypad::onFogScalarEdited() {
  {
    QLineEdit *edit = ui->IDC_FOG_SCALAR_EDIT;
    Terrain_sky.fog_scalar = edit->text().toFloat();
    World_changed = true;
  }
}

void TerrainKeypad::onDamagePerSecEdited() {
  {
    QLineEdit *edit = ui->IDC_DAMAGE_PER_SEC_EDIT;
    Terrain_sky.damage_per_second = edit->text().toFloat();
    World_changed = true;
  }
}

void TerrainKeypad::onRotateSpeedEdited() {
  {
    QLineEdit *edit = ui->IDC_ROTATE_SPEED_EDIT;
    Terrain_sky.rotate_rate = edit->text().toFloat();
    World_changed = true;
  }
}

void TerrainKeypad::onStarsToggled(bool checked) {
  Terrain_sky.flags.stars = checked;
  TV_changed = true;
}
void TerrainKeypad::onSatelliteToggled(bool checked) {
  Terrain_sky.flags.satellites = checked;
  TV_changed = true;
}
void TerrainKeypad::onTexturedSkyToggled(bool checked) {
  Terrain_sky.textured = checked;
  World_changed = true;
}
void TerrainKeypad::onUseFogToggled(bool checked) {
  Terrain_sky.flags.fog = checked;
  World_changed = true;
}
void TerrainKeypad::onUseHaloToggled(bool checked) {
  if (m_currentSatellite >= 0 && m_currentSatellite < 5) {
    Terrain_sky.satellite_flags[m_currentSatellite].halo = checked;
    World_changed = true;
  }
}
void TerrainKeypad::onUseAtmosphereToggled(bool checked) {
  if (m_currentSatellite >= 0 && m_currentSatellite < 5) {
    Terrain_sky.satellite_flags[m_currentSatellite].atmosphere = checked;
    World_changed = true;
  }
}
void TerrainKeypad::onRotateStarsToggled(bool checked) {
  Terrain_sky.flags.rotate_stars = checked;
  TV_changed = true;
}
void TerrainKeypad::onRotateSkyToggled(bool checked) {
  Terrain_sky.flags.rotate_sky = checked;
  TV_changed = true;
}
void TerrainKeypad::onFastTerrainToggled(bool checked) {
  Fast_terrain = checked ? 1 : 0;
  TV_changed = true;
}
void TerrainKeypad::onShowTerrainToggled(bool checked) {
  D3EditState.terrain_dots = checked;
  State_changed = true;
}
void TerrainKeypad::onFlatShadeToggled(bool checked) {
  D3EditState.terrain_flat_shade = checked;
  if (checked) {
    Terrain_texture_distance = 0;
    Detail_settings.Terrain_render_distance = DEFAULT_VISIBLE_TERRAIN_DISTANCE * 2;
  } else {
    Terrain_texture_distance = 9999999;
    Detail_settings.Terrain_render_distance = DEFAULT_VISIBLE_TERRAIN_DISTANCE;
  }
  State_changed = true;
}
void TerrainKeypad::onNoLodToggled(bool checked) {
  Editor_LOD_engine_off = checked;
  State_changed = true;
}
void TerrainKeypad::onTerrain2dToggled(bool checked) {
  Flat_terrain = checked ? 1 : 0;
  State_changed = true;
}
void TerrainKeypad::onShowInvisibleToggled(bool checked) {
  Show_invisible_terrain = checked;
  State_changed = true;
}
void TerrainKeypad::onNoExtRoomsObjsToggled(bool checked) {
  Terrain_render_ext_room_objs = !checked;
}
