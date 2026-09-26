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

#include "texture_keypad.h"
#include "ui_texturekeypad.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <cmath>

#include "d3edit.h"
#include "editor_room_state.h"
#include "room_external.h"


TextureKeypad::TextureKeypad(QWidget *parent)
    : QDialog(parent), ui(new Ui::TextureKeypad)
{
  ui->setupUi(this);
  connect(ui->IDC_TEXPAD_SLIDELEFT, &QPushButton::clicked, this, &TextureKeypad::onSlideLeft);
  connect(ui->IDC_TEXPAD_SLIDERIGHT, &QPushButton::clicked, this, &TextureKeypad::onSlideRight);
  connect(ui->IDC_TEXPAD_SLIDEUP, &QPushButton::clicked, this, &TextureKeypad::onSlideUp);
  connect(ui->IDC_TEXPAD_SLIDEDOWN, &QPushButton::clicked, this, &TextureKeypad::onSlideDown);
  connect(ui->IDC_TEXPAD_ROTLEFT, &QPushButton::clicked, this, &TextureKeypad::onRotLeft);
  connect(ui->IDC_TEXPAD_ROTRIGHT, &QPushButton::clicked, this, &TextureKeypad::onRotRight);
  connect(ui->IDC_TEXPAD_ROTATE90, &QPushButton::clicked, this, &TextureKeypad::onRotate90);
  connect(ui->IDC_TEXPAD_FLIPX, &QPushButton::clicked, this, &TextureKeypad::onFlipX);
  connect(ui->IDC_TEXPAD_FLIPY, &QPushButton::clicked, this, &TextureKeypad::onFlipY);
  connect(ui->IDC_TEXPAD_EXPAND_U, &QPushButton::clicked, this, &TextureKeypad::onExpandU);
  connect(ui->IDC_TEXPAD_CONTRACT_U, &QPushButton::clicked, this, &TextureKeypad::onContractU);
  connect(ui->IDC_TEXPAD_EXPAND_V, &QPushButton::clicked, this, &TextureKeypad::onExpandV);
  connect(ui->IDC_TEXPAD_CONTRACT_V, &QPushButton::clicked, this, &TextureKeypad::onContractV);
  connect(ui->IDC_TEXPAD_STRETCHLESS, &QPushButton::clicked, this, &TextureKeypad::onStretchLess);
  connect(ui->IDC_TEXPAD_STRETCHMORE, &QPushButton::clicked, this, &TextureKeypad::onStretchMore);
  connect(ui->IDC_TEXPAD_SETDEFAULT, &QPushButton::clicked, this, &TextureKeypad::onSetDefault);
  connect(ui->IDC_TEXPAD_GRAB, &QPushButton::clicked, this, &TextureKeypad::onGrab);
  connect(ui->IDC_REPLACE_TEXTURE, &QPushButton::clicked, this, &TextureKeypad::onReplace);
  connect(ui->IDC_FACE_MAP, &QPushButton::clicked, this, &TextureKeypad::onFaceMap);
  connect(ui->IDC_TEXSCALE_EDIT, &QLineEdit::editingFinished, this, &TextureKeypad::onTexScaleEdited);

  ui->IDC_TEXSCALE_EDIT->setText(QString::number(app.texscale));

  updateDialog();
}

TextureKeypad::~TextureKeypad() { delete ui; }

void TextureKeypad::updateDialog() {
  // Editing a face's texture requires a current room + face.
  const bool active = (Curroomp != nullptr && Curroomp->used && Curface >= 0 &&
                       Curface < Curroomp->num_faces);
  for (QWidget *w : findChildren<QWidget *>())
    if (w->objectName().startsWith("IDC_TEXPAD") || w->objectName().startsWith("IDC_FACE_MAP"))
      w->setEnabled(active);

  if (active)
    ui->IDC_CURRENT_TEXTURE_NAME->setText(QString("Texture %1").arg(Curroomp->faces[Curface].tmap));
  else
    ui->IDC_CURRENT_TEXTURE_NAME->setText("No face selected");
}

void TextureKeypad::onSlideLeft() { HTextureSlide(Curroomp, Curface, -1.0f * app.texscale, 0); }
void TextureKeypad::onSlideRight() { HTextureSlide(Curroomp, Curface, 1.0f * app.texscale, 0); }
void TextureKeypad::onSlideUp() { HTextureSlide(Curroomp, Curface, 0, 1.0f * app.texscale); }
void TextureKeypad::onSlideDown() { HTextureSlide(Curroomp, Curface, 0, -1.0f * app.texscale); }
void TextureKeypad::onRotLeft() { HTextureRotate(Curroomp, Curface, -0.1f * app.texscale); }
void TextureKeypad::onRotRight() { HTextureRotate(Curroomp, Curface, 0.1f * app.texscale); }
void TextureKeypad::onRotate90() { HTextureRotate(Curroomp, Curface, 3.14159f / 2.0f); }
void TextureKeypad::onFlipX() { HTextureFlipX(Curroomp, Curface); }
void TextureKeypad::onFlipY() { HTextureFlipY(Curroomp, Curface); }
void TextureKeypad::onExpandU() { ScaleFaceUVs(Curroomp, Curface, 1.1f); }
void TextureKeypad::onContractU() { ScaleFaceUVs(Curroomp, Curface, 1.0f / 1.1f); }
void TextureKeypad::onExpandV() { ScaleFaceUVs(Curroomp, Curface, 1.1f); }
void TextureKeypad::onContractV() { ScaleFaceUVs(Curroomp, Curface, 1.0f / 1.1f); }
void TextureKeypad::onStretchLess() { HTextureStretchLess(Curroomp, Curface, Curedge, app.texscale); }
void TextureKeypad::onStretchMore() { HTextureStretchMore(Curroomp, Curface, Curedge, app.texscale); }

void TextureKeypad::onSetDefault() { HTextureSetDefault(Curroomp, Curface); }

void TextureKeypad::onGrab() {
  if (Curroomp != nullptr && Curface >= 0)
    app.texdlg_texture = Curroomp->faces[Curface].tmap;
}

void TextureKeypad::onReplace() {
  if (Curroomp != nullptr && Curface >= 0)
    Curroomp->faces[Curface].tmap = app.texdlg_texture;
}

void TextureKeypad::onFaceMap() { onSetDefault(); }

void TextureKeypad::onTexScaleEdited() {
  app.texscale = ui->IDC_TEXSCALE_EDIT->text().toFloat();
  if (app.texscale <= 0)
    app.texscale = 1.0f;
}


