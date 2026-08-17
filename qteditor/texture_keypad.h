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

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class TextureKeypad; }
QT_END_NAMESPACE


// Port of CTextureDialog (IDD_TEXTUREKEYPAD): face texture UV manipulation
// (slide/rotate/flip/expand/contract) plus texture selection filters.
class TextureKeypad : public QDialog {
  Q_OBJECT
public:
  explicit TextureKeypad(QWidget *parent = nullptr);
  ~TextureKeypad();

private slots:
  void onSlideLeft();
  void onSlideRight();
  void onSlideUp();
  void onSlideDown();
  void onRotLeft();
  void onRotRight();
  void onRotate90();
  void onFlipX();
  void onFlipY();
  void onExpandU();
  void onContractU();
  void onExpandV();
  void onContractV();
  void onStretchLess();
  void onStretchMore();
  void onSetDefault();
  void onGrab();
  void onReplace();
  void onFaceMap();
  void onTexScaleEdited();

private:
  void updateDialog();

  template <typename T>
  T *find(const QString &name) const { return findChild<T *>(name); }

  Ui::TextureKeypad *ui;
};

