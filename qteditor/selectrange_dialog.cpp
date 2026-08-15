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

#include "selectrange_dialog.h"

#include <cstdlib>

#include <QCheckBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include "d3edit.h"
#include "terrain.h"

namespace QtEditor {

SelectRangeDialog::SelectRangeDialog(QWidget *parent) : Dialog(":/editor/selectrange_dialog.ui", parent) {
  if (QPushButton *ok = find<QPushButton>("IDOK")) {
    disconnect(ok, &QPushButton::clicked, m_dialog, &QDialog::accept);
    connect(ok, &QPushButton::clicked, this, &SelectRangeDialog::onOk);
  }
  if (auto *edit = find<QLineEdit>("IDC_LOWER_BOUND_EDIT"))
    connect(edit, &QLineEdit::editingFinished, this, &SelectRangeDialog::onLowerBoundChanged);
  if (auto *edit = find<QLineEdit>("IDC_UPPER_BOUND_EDIT"))
    connect(edit, &QLineEdit::editingFinished, this, &SelectRangeDialog::onUpperBoundChanged);
  if (auto *edit = find<QLineEdit>("IDC_SLOPE_EDIT"))
    connect(edit, &QLineEdit::editingFinished, this, &SelectRangeDialog::onSlopeChanged);
  if (auto *cb = find<QCheckBox>("IDC_SELECT_SLOPE_CHECK"))
    connect(cb, &QCheckBox::toggled, this, &SelectRangeDialog::onSelectSlopeToggled);
  if (auto *edit = find<QLineEdit>("IDC_SLOPE_EDIT"))
    edit->setEnabled(find<QCheckBox>("IDC_SELECT_SLOPE_CHECK")->isChecked());
}

SelectRangeDialog::~SelectRangeDialog() = default;

void SelectRangeDialog::clampAndStore(const char *editName, int *target) {
  QLineEdit *edit = find<QLineEdit>(editName);
  if (edit == nullptr)
    return;
  int num = edit->text().toInt();
  if (num < 0)
    num = 0;
  if (num > 255)
    num = 255;
  *target = num;
  edit->setText(QString::number(num));
}

void SelectRangeDialog::onLowerBoundChanged() { clampAndStore("IDC_LOWER_BOUND_EDIT", &m_lowerBound); }
void SelectRangeDialog::onUpperBoundChanged() { clampAndStore("IDC_UPPER_BOUND_EDIT", &m_upperBound); }
void SelectRangeDialog::onSlopeChanged() { clampAndStore("IDC_SLOPE_EDIT", &m_slopeAngle); }

void SelectRangeDialog::onSelectSlopeToggled(bool enabled) {
  if (QWidget *edit = find<QWidget>("IDC_SLOPE_EDIT"))
    edit->setEnabled(enabled);
}

void SelectRangeDialog::runSelection() {
  const bool use_slope = find<QCheckBox>("IDC_SELECT_SLOPE_CHECK")->isChecked();
  const bool inverted = find<QCheckBox>("IDC_INVERSE_CHECK")->isChecked();
  const bool texgrid = find<QCheckBox>("IDC_TEXTURE_GRID_CHECK")->isChecked();
  const bool random = find<QCheckBox>("IDC_RANDOM_CHECK")->isChecked();
  const bool cur_texture = find<QCheckBox>("IDC_SELECT_CUR_TEXTURE_CHECK")->isChecked();

  if (m_upperBound < m_lowerBound) {
    QMessageBox::warning(m_dialog, "Select Range", "Lower bound must be less than upper bound!");
    return;
  }

  const int count = TERRAIN_WIDTH * TERRAIN_DEPTH;
  for (int i = 0; i < count; i++)
    TerrainSelected[i] = 0;
  Num_terrain_selected = 0;

  for (int i = 0; i < count; i++) {
    int selected = 1;

    if (!(Terrain_seg[i].ypos >= m_lowerBound && Terrain_seg[i].ypos <= m_upperBound))
      selected = 0;

    if (use_slope) {
      int x = i % TERRAIN_WIDTH;
      int z = i / TERRAIN_WIDTH;

      if (x != (TERRAIN_WIDTH - 1)) {
        if (!(abs(Terrain_seg[i].ypos - Terrain_seg[z * TERRAIN_WIDTH + (x + 1)].ypos) >= m_slopeAngle))
          selected = 0;

        if (z != (TERRAIN_DEPTH - 1)) {
          if (!(abs(Terrain_seg[i].ypos - Terrain_seg[(z + 1) * TERRAIN_WIDTH + (x + 1)].ypos) >= m_slopeAngle))
            selected = 0;
        }
      }

      if (z != (TERRAIN_DEPTH - 1)) {
        if (!(abs(Terrain_seg[i].ypos - Terrain_seg[(z + 1) * TERRAIN_WIDTH + (x)].ypos) >= m_slopeAngle))
          selected = 0;
      }
    }

    if (random) {
      if (rand() > (RAND_MAX / 2))
        selected = 0;
    }

    if (texgrid) {
      int x = i % TERRAIN_WIDTH;
      int z = i / TERRAIN_WIDTH;

      if (x % (1 << (MAX_TERRAIN_LOD - 1)))
        selected = 0;

      if (z % (1 << (MAX_TERRAIN_LOD - 1)))
        selected = 0;
    }

    if (cur_texture) {
      if (Terrain_tex_seg[Terrain_seg[i].texseg_index].tex_index != D3EditState.texdlg_texture)
        selected = 0;
    }

    if (inverted)
      selected = !selected;

    if (selected) {
      TerrainSelected[i] = 1;
      Num_terrain_selected++;
    }
  }
}

void SelectRangeDialog::onOk() {
  clampAndStore("IDC_LOWER_BOUND_EDIT", &m_lowerBound);
  clampAndStore("IDC_UPPER_BOUND_EDIT", &m_upperBound);
  clampAndStore("IDC_SLOPE_EDIT", &m_slopeAngle);
  runSelection();
  m_dialog->accept();
}

}
