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

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "d3edit.h"
#include "config.h"
#include "terrain.h"


TerrainKeypad::TerrainKeypad(QWidget *parent)
    : QDialog(parent), ui(new Ui::TerrainKeypad)
{
  ui->setupUi(this);
  if (QPushButton *b = ui->IDC_TERRPAD_MOVE_UP)
    connect(b, &QPushButton::clicked, this, &TerrainKeypad::onMoveUp);
  if (QPushButton *b = ui->IDC_TERRPAD_MOVE_DOWN)
    connect(b, &QPushButton::clicked, this, &TerrainKeypad::onMoveDown);
  if (QPushButton *b = ui->IDC_TERRPAD_RAISE10)
    connect(b, &QPushButton::clicked, this, &TerrainKeypad::onRaise10);
  if (QPushButton *b = ui->IDC_TERRPAD_LOWER10)
    connect(b, &QPushButton::clicked, this, &TerrainKeypad::onLower10);
  if (QPushButton *b = ui->IDC_TERRPAD_SELECT_NONE)
    connect(b, &QPushButton::clicked, this, &TerrainKeypad::onSelectNone);
  if (QPushButton *b = ui->IDC_TERR_SELECT_ALL)
    connect(b, &QPushButton::clicked, this, &TerrainKeypad::onSelectAll);
  if (QPushButton *b = ui->IDC_TERRPAD_MAKE_MAX)
    connect(b, &QPushButton::clicked, this, &TerrainKeypad::onMakeMax);
  if (QPushButton *b = ui->IDC_TERRPAD_MAKE_MIN)
    connect(b, &QPushButton::clicked, this, &TerrainKeypad::onMakeMin);
  if (QPushButton *b = ui->IDC_TERRPAD_MAKE_ZERO)
    connect(b, &QPushButton::clicked, this, &TerrainKeypad::onMakeZero);
  if (QPushButton *b = ui->IDC_TERRPAD_FILL_AREA)
    connect(b, &QPushButton::clicked, this, &TerrainKeypad::onFillArea);
  if (QPushButton *b = ui->IDC_TERR_MORE_MOONS)
    connect(b, &QPushButton::clicked, this, &TerrainKeypad::onMoreMoons);
  if (QPushButton *b = ui->IDC_TERR_LESS_MOONS)
    connect(b, &QPushButton::clicked, this, &TerrainKeypad::onLessMoons);
  if (QLineEdit *edit = ui->IDC_FOG_DISTANCE_EDIT)
    connect(edit, &QLineEdit::editingFinished, this, &TerrainKeypad::onFogDistanceEdited);

  updateDialog();
}

TerrainKeypad::~TerrainKeypad() { delete ui; }

void TerrainKeypad::updateDialog() {
  const bool hasTerrain = (Num_terrain_selected > 0);
  const char *names[] = {"IDC_TERRPAD_MOVE_UP",    "IDC_TERRPAD_MOVE_DOWN", "IDC_TERRPAD_RAISE10",
                         "IDC_TERRPAD_LOWER10",    "IDC_TERRPAD_MAKE_MAX",  "IDC_TERRPAD_MAKE_MIN",
                         "IDC_TERRPAD_MAKE_ZERO",  "IDC_TERRPAD_FILL_AREA"};
  for (const char *name : names)
    if (QWidget *w = findChild<QWidget*>(name))
      w->setEnabled(hasTerrain);

  if (QLabel *label = ui->IDC_NUM_MOONS_STATIC)
    label->setText(QString("Moons: %1").arg(Terrain_sky.num_satellites));
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
    if (newY < 0)
      newY = 0;
    if (newY > 255)
      newY = 255;
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
  updateDialog();
}
void TerrainKeypad::onSelectAll() {
  const int count = TERRAIN_WIDTH * TERRAIN_DEPTH;
  for (int i = 0; i < count; i++)
    TerrainSelected[i] = 1;
  Num_terrain_selected = count;
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
  updateDialog();
}
void TerrainKeypad::onLessMoons() {
  if (Terrain_sky.num_satellites > 0)
    Terrain_sky.num_satellites--;
  updateDialog();
}
void TerrainKeypad::onFogDistanceEdited() {
  if (QLineEdit *edit = ui->IDC_FOG_DISTANCE_EDIT) {
    float predist = edit->text().toFloat();
    if (predist < 20)
      predist = 20;
    if (predist > 200)
      predist = 200;
    Detail_settings.Terrain_render_distance = predist * TERRAIN_SIZE;
    World_changed = true;
  }
}

