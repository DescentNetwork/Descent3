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


namespace {

} // namespace

TextureKeypad::TextureKeypad(QWidget *parent)
    : QDialog(parent), ui(new Ui::TextureKeypad)
{
  ui->setupUi(this);
  {
    QPushButton *b = ui->IDC_TEXPAD_SLIDELEFT;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onSlideLeft);
  }
  {
    QPushButton *b = ui->IDC_TEXPAD_SLIDERIGHT;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onSlideRight);
  }
  {
    QPushButton *b = ui->IDC_TEXPAD_SLIDEUP;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onSlideUp);
  }
  {
    QPushButton *b = ui->IDC_TEXPAD_SLIDEDOWN;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onSlideDown);
  }
  {
    QPushButton *b = ui->IDC_TEXPAD_ROTLEFT;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onRotLeft);
  }
  {
    QPushButton *b = ui->IDC_TEXPAD_ROTRIGHT;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onRotRight);
  }
  {
    QPushButton *b = ui->IDC_TEXPAD_ROTATE90;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onRotate90);
  }
  {
    QPushButton *b = ui->IDC_TEXPAD_FLIPX;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onFlipX);
  }
  {
    QPushButton *b = ui->IDC_TEXPAD_FLIPY;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onFlipY);
  }
  {
    QPushButton *b = ui->IDC_TEXPAD_EXPAND_U;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onExpandU);
  }
  {
    QPushButton *b = ui->IDC_TEXPAD_CONTRACT_U;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onContractU);
  }
  {
    QPushButton *b = ui->IDC_TEXPAD_EXPAND_V;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onExpandV);
  }
  {
    QPushButton *b = ui->IDC_TEXPAD_CONTRACT_V;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onContractV);
  }
  {
    QPushButton *b = ui->IDC_TEXPAD_STRETCHLESS;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onStretchLess);
  }
  {
    QPushButton *b = ui->IDC_TEXPAD_STRETCHMORE;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onStretchMore);
  }
  {
    QPushButton *b = ui->IDC_TEXPAD_SETDEFAULT;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onSetDefault);
  }
  {
    QPushButton *b = ui->IDC_TEXPAD_GRAB;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onGrab);
  }
  {
    QPushButton *b = ui->IDC_REPLACE_TEXTURE;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onReplace);
  }
  {
    QPushButton *b = ui->IDC_FACE_MAP;
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onFaceMap);
  }
  {
    QLineEdit *edit = ui->IDC_TEXSCALE_EDIT;
    connect(edit, &QLineEdit::editingFinished, this, &TextureKeypad::onTexScaleEdited);
  }

  {
    QLineEdit *edit = ui->IDC_TEXSCALE_EDIT;
    edit->setText(QString::number(D3EditState.texscale));
  }

  updateDialog();
}

TextureKeypad::~TextureKeypad() { delete ui; }

void TextureKeypad::updateDialog() {
  // Editing a face's texture requires a current room + face.
  const bool active = (Curroomp != nullptr && Curroomp->used && Curface >= 0 &&
                       Curface < Curroomp->num_faces);
  const QList<QWidget *> all = this->findChildren<QWidget *>();
  for (QWidget *w : all)
    if (w->objectName().startsWith("IDC_TEXPAD") || w->objectName().startsWith("IDC_FACE_MAP"))
      w->setEnabled(active);
  {
    QLabel *label = ui->IDC_CURRENT_TEXTURE_NAME;
    if (active) {
      const int tmap = Curroomp->faces[Curface].tmap;
      label->setText(QString("Texture %1").arg(tmap));
    } else {
      label->setText("No face selected");
    }
  }
}

void TextureKeypad::onSlideLeft() { HTextureSlide(Curroomp, Curface, -1.0f * D3EditState.texscale, 0); }
void TextureKeypad::onSlideRight() { HTextureSlide(Curroomp, Curface, 1.0f * D3EditState.texscale, 0); }
void TextureKeypad::onSlideUp() { HTextureSlide(Curroomp, Curface, 0, 1.0f * D3EditState.texscale); }
void TextureKeypad::onSlideDown() { HTextureSlide(Curroomp, Curface, 0, -1.0f * D3EditState.texscale); }
void TextureKeypad::onRotLeft() {
  HTextureRotate(Curroomp, Curface, -0.1f * D3EditState.texscale);
}
void TextureKeypad::onRotRight() {
  HTextureRotate(Curroomp, Curface, 0.1f * D3EditState.texscale);
}
void TextureKeypad::onRotate90() { HTextureRotate(Curroomp, Curface, 3.14159f / 2.0f); }
void TextureKeypad::onFlipX() { HTextureFlipX(Curroomp, Curface); }
void TextureKeypad::onFlipY() { HTextureFlipY(Curroomp, Curface); }
void TextureKeypad::onExpandU() { ScaleFaceUVs(Curroomp, Curface, 1.1f); }
void TextureKeypad::onContractU() { ScaleFaceUVs(Curroomp, Curface, 1.0f / 1.1f); }
void TextureKeypad::onExpandV() { ScaleFaceUVs(Curroomp, Curface, 1.1f); }
void TextureKeypad::onContractV() { ScaleFaceUVs(Curroomp, Curface, 1.0f / 1.1f); }
void TextureKeypad::onStretchLess() { HTextureStretchLess(Curroomp, Curface, Curedge, D3EditState.texscale); }
void TextureKeypad::onStretchMore() { HTextureStretchMore(Curroomp, Curface, Curedge, D3EditState.texscale); }

void TextureKeypad::onSetDefault() { HTextureSetDefault(Curroomp, Curface); }

void TextureKeypad::onGrab() {
  if (Curroomp != nullptr && Curface >= 0)
    D3EditState.texdlg_texture = Curroomp->faces[Curface].tmap;
}

void TextureKeypad::onReplace() {
  if (Curroomp != nullptr && Curface >= 0)
    Curroomp->faces[Curface].tmap = D3EditState.texdlg_texture;
}

void TextureKeypad::onFaceMap() { onSetDefault(); }

void TextureKeypad::onTexScaleEdited() {
  {
    QLineEdit *edit = ui->IDC_TEXSCALE_EDIT;
    D3EditState.texscale = edit->text().toFloat();
    if (D3EditState.texscale <= 0)
      D3EditState.texscale = 1.0f;
  }
}


