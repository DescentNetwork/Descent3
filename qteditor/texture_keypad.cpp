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

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <cmath>

#include "d3edit.h"
#include "room_external.h"

namespace QtEditor {

namespace {

// Slide all UVs by (right, up) in 1/128th texture units.
void uvSlide(face *fp, float right, float up) {
  for (int i = 0; i < fp->num_verts; i++) {
    fp->face_uvls[i].u -= right / 128.0f;
    fp->face_uvls[i].v += up / 128.0f;
  }
}

void uvRotate(face *fp, float angle) {
  if (fp->num_verts <= 0)
    return;
  float cu = 0, cv = 0;
  for (int i = 0; i < fp->num_verts; i++) {
    cu += fp->face_uvls[i].u;
    cv += fp->face_uvls[i].v;
  }
  cu /= fp->num_verts;
  cv /= fp->num_verts;
  const float c = std::cos(angle);
  const float s = std::sin(angle);
  for (int i = 0; i < fp->num_verts; i++) {
    const float du = fp->face_uvls[i].u - cu;
    const float dv = fp->face_uvls[i].v - cv;
    fp->face_uvls[i].u = cu + du * c - dv * s;
    fp->face_uvls[i].v = cv + du * s + dv * c;
  }
}

void uvFlipX(face *fp) {
  for (int i = 0; i < fp->num_verts; i++)
    fp->face_uvls[i].u = -fp->face_uvls[i].u;
}

void uvFlipY(face *fp) {
  for (int i = 0; i < fp->num_verts; i++)
    fp->face_uvls[i].v = -fp->face_uvls[i].v;
}

void uvScaleU(face *fp, float factor) {
  for (int i = 0; i < fp->num_verts; i++)
    fp->face_uvls[i].u *= factor;
}

void uvScaleV(face *fp, float factor) {
  for (int i = 0; i < fp->num_verts; i++)
    fp->face_uvls[i].v *= factor;
}

} // namespace

TextureKeypad::TextureKeypad(QWidget *parent) : Keypad(":/ui/texturekeypad.ui", parent) {
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_SLIDELEFT"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onSlideLeft);
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_SLIDERIGHT"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onSlideRight);
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_SLIDEUP"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onSlideUp);
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_SLIDEDOWN"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onSlideDown);
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_ROTLEFT"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onRotLeft);
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_ROTRIGHT"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onRotRight);
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_ROTATE90"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onRotate90);
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_FLIPX"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onFlipX);
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_FLIPY"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onFlipY);
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_EXPAND_U"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onExpandU);
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_CONTRACT_U"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onContractU);
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_EXPAND_V"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onExpandV);
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_CONTRACT_V"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onContractV);
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_STRETCHLESS"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onStretchLess);
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_STRETCHMORE"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onStretchMore);
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_SETDEFAULT"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onSetDefault);
  if (QPushButton *b = find<QPushButton>("IDC_TEXPAD_GRAB"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onGrab);
  if (QPushButton *b = find<QPushButton>("IDC_REPLACE_TEXTURE"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onReplace);
  if (QPushButton *b = find<QPushButton>("IDC_FACE_MAP"))
    connect(b, &QPushButton::clicked, this, &TextureKeypad::onFaceMap);
  if (QLineEdit *edit = find<QLineEdit>("IDC_TEXSCALE_EDIT"))
    connect(edit, &QLineEdit::editingFinished, this, &TextureKeypad::onTexScaleEdited);

  if (QLineEdit *edit = find<QLineEdit>("IDC_TEXSCALE_EDIT"))
    edit->setText(QString::number(D3EditState.texscale));

  updateDialog();
}

TextureKeypad::~TextureKeypad() = default;

void TextureKeypad::updateDialog() {
  // Editing a face's texture requires a current room + face.
  const bool active = (Curroomp != nullptr && Curroomp->used && Curface >= 0 &&
                       Curface < Curroomp->num_faces);
  const QList<QWidget *> all = m_widget->findChildren<QWidget *>();
  for (QWidget *w : all)
    if (w->objectName().startsWith("IDC_TEXPAD") || w->objectName().startsWith("IDC_FACE_MAP"))
      w->setEnabled(active);
  if (QLabel *label = find<QLabel>("IDC_CURRENT_TEXTURE_NAME")) {
    if (active) {
      const int tmap = Curroomp->faces[Curface].tmap;
      label->setText(QString("Texture %1").arg(tmap));
    } else {
      label->setText("No face selected");
    }
  }
}

void TextureKeypad::onSlideLeft() { uvSlide(&Curroomp->faces[Curface], -1.0f * D3EditState.texscale, 0); }
void TextureKeypad::onSlideRight() { uvSlide(&Curroomp->faces[Curface], 1.0f * D3EditState.texscale, 0); }
void TextureKeypad::onSlideUp() { uvSlide(&Curroomp->faces[Curface], 0, 1.0f * D3EditState.texscale); }
void TextureKeypad::onSlideDown() { uvSlide(&Curroomp->faces[Curface], 0, -1.0f * D3EditState.texscale); }
void TextureKeypad::onRotLeft() {
  uvRotate(&Curroomp->faces[Curface], -0.1f * D3EditState.texscale);
}
void TextureKeypad::onRotRight() {
  uvRotate(&Curroomp->faces[Curface], 0.1f * D3EditState.texscale);
}
void TextureKeypad::onRotate90() { uvRotate(&Curroomp->faces[Curface], 3.14159f / 2.0f); }
void TextureKeypad::onFlipX() { uvFlipX(&Curroomp->faces[Curface]); }
void TextureKeypad::onFlipY() { uvFlipY(&Curroomp->faces[Curface]); }
void TextureKeypad::onExpandU() { uvScaleU(&Curroomp->faces[Curface], 1.1f); }
void TextureKeypad::onContractU() { uvScaleU(&Curroomp->faces[Curface], 1.0f / 1.1f); }
void TextureKeypad::onExpandV() { uvScaleV(&Curroomp->faces[Curface], 1.1f); }
void TextureKeypad::onContractV() { uvScaleV(&Curroomp->faces[Curface], 1.0f / 1.1f); }
void TextureKeypad::onStretchLess() { uvScaleU(&Curroomp->faces[Curface], 1.0f / 1.1f); }
void TextureKeypad::onStretchMore() { uvScaleU(&Curroomp->faces[Curface], 1.1f); }

void TextureKeypad::onSetDefault() {
  // Face-map the texture to the face's world extents (simple planar map).
  face *fp = &Curroomp->faces[Curface];
  vector minv = fp->min_xyz, maxv = fp->max_xyz;
  const float du = (maxv.x() - minv.x()) > 0.001f ? (maxv.x() - minv.x()) : 1.0f;
  const float dv = (maxv.y() - minv.y()) > 0.001f ? (maxv.y() - minv.y()) : 1.0f;
  for (int i = 0; i < fp->num_verts; i++) {
    const vector &p = Curroomp->verts[fp->face_verts[i]];
    fp->face_uvls[i].u = (p.x() - minv.x()) / du;
    fp->face_uvls[i].v = (p.y() - minv.y()) / dv;
  }
}

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
  if (QLineEdit *edit = find<QLineEdit>("IDC_TEXSCALE_EDIT")) {
    D3EditState.texscale = edit->text().toFloat();
    if (D3EditState.texscale <= 0)
      D3EditState.texscale = 1.0f;
  }
}

}

