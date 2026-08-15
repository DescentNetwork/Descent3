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

#include "qteditor_dialog.h"

struct texture;

namespace QtEditor {

// Port of CWorldTexturesDialog (IDD_WORLDTEXTURES): edits the texture table
// (lighting, damage, slide, alpha, material/type flags, ambient sound, etc.).
class WorldTexturesDialog : public Dialog {
  Q_OBJECT
public:
  explicit WorldTexturesDialog(QWidget *parent = nullptr);
  ~WorldTexturesDialog() override;

private slots:
  void onAddNew();
  void onDelete();
  void onLock();
  void onCheckin();
  void onCheckedOut();
  void onOverride();
  void onChangeName();
  void onLoadBitmap();
  void onCurrent();
  void onNext();
  void onPrev();
  void onTexListChanged();
  void onAmbientSoundChanged();

private:
  void updateDialog();
  void saveTexturesOnClose();
  void setFlag(uint32_t flag, const char *checkName, bool checked);
  void bindFlag(const char *checkName, uint32_t flag);
  void bindEdit(const char *name, float texture::*field);
};

}
