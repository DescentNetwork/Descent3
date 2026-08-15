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

#include "viewer_prop_dialog.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>

#include "d3edit.h"
#include "object.h"
#include "slew.h"
#include "vecmat.h"

extern int Slew_limitations;

namespace QtEditor {

namespace {
constexpr int kAnglesPerDegree = 65536 / 360;
constexpr int kDegreesPerAngle = 360 / 65536;
} // namespace

ViewerPropDialog::ViewerPropDialog(QWidget *parent) : Dialog(":/ui/viewer_dialog.ui", parent) {
  if (QCheckBox *cb = find<QCheckBox>("IDC_XMOVE_CHECK")) {
    cb->setChecked(true);
    connect(cb, &QCheckBox::toggled, this, &ViewerPropDialog::onXMoveToggled);
  }
  if (QCheckBox *cb = find<QCheckBox>("IDC_YMOVE_CHECK")) {
    cb->setChecked(true);
    connect(cb, &QCheckBox::toggled, this, &ViewerPropDialog::onYMoveToggled);
  }
  if (QCheckBox *cb = find<QCheckBox>("IDC_ZMOVE_CHECK")) {
    cb->setChecked(true);
    connect(cb, &QCheckBox::toggled, this, &ViewerPropDialog::onZMoveToggled);
  }

  updatePosition();
  updateOrientation();

  // Without a viewer (no level loaded) the editing controls are inert, so
  // disable them to match the "needs a level" gating.
  if (Viewer_object == nullptr) {
    const QList<QWidget *> all = m_widget->findChildren<QWidget *>();
    for (QWidget *w : all)
      if (w->objectName().startsWith("IDC_"))
        w->setEnabled(false);
  }

  if (QPushButton *b = find<QPushButton>("IDC_POS_COMMIT_BUTTON"))
    b->setEnabled(false);

  if (QPushButton *b = find<QPushButton>("IDC_ALIGN_UPYPOS_BUTTON"))
    connect(b, &QPushButton::clicked, this, &ViewerPropDialog::onAlignUpYpos);
  if (QPushButton *b = find<QPushButton>("IDC_ALIGN_XNEG_BUTTON"))
    connect(b, &QPushButton::clicked, this, &ViewerPropDialog::onAlignXneg);
  if (QPushButton *b = find<QPushButton>("IDC_ALIGN_XPOS_BUTTON"))
    connect(b, &QPushButton::clicked, this, &ViewerPropDialog::onAlignXpos);
  if (QPushButton *b = find<QPushButton>("IDC_ALIGN_YNEG_BUTTON"))
    connect(b, &QPushButton::clicked, this, &ViewerPropDialog::onAlignYneg);
  if (QPushButton *b = find<QPushButton>("IDC_ALIGN_YPOS_BUTTON"))
    connect(b, &QPushButton::clicked, this, &ViewerPropDialog::onAlignYpos);
  if (QPushButton *b = find<QPushButton>("IDC_ALIGN_ZNEG_BUTTON"))
    connect(b, &QPushButton::clicked, this, &ViewerPropDialog::onAlignZneg);
  if (QPushButton *b = find<QPushButton>("IDC_ALIGN_ZPOS_BUTTON"))
    connect(b, &QPushButton::clicked, this, &ViewerPropDialog::onAlignZpos);
  if (QPushButton *b = find<QPushButton>("IDC_ORIENT_COMMIT_BUTTON"))
    connect(b, &QPushButton::clicked, this, &ViewerPropDialog::onOrientCommit);
  if (QPushButton *b = find<QPushButton>("IDC_POS_COMMIT_BUTTON"))
    connect(b, &QPushButton::clicked, this, &ViewerPropDialog::onPosCommit);
}

ViewerPropDialog::~ViewerPropDialog() = default;

void ViewerPropDialog::updateOrientation() {
  if (Viewer_object == nullptr) {
    if (QWidget *w = find<QWidget>("IDC_PITCH_EDIT"))
      w->setEnabled(false);
    return;
  }
  angvec angs;
  vm_ExtractAnglesFromMatrix(&angs, &Viewer_object->orient);

  if (QLineEdit *edit = find<QLineEdit>("IDC_PITCH_EDIT"))
    edit->setText(QString::number((int)angs.p() * kDegreesPerAngle));
  if (QLineEdit *edit = find<QLineEdit>("IDC_HEADING_EDIT"))
    edit->setText(QString::number((int)angs.h() * kDegreesPerAngle));
  if (QLineEdit *edit = find<QLineEdit>("IDC_BANK_EDIT"))
    edit->setText(QString::number((int)angs.b() * kDegreesPerAngle));
}

void ViewerPropDialog::updatePosition() {
  const bool hasViewer = (Viewer_object != nullptr);
  if (Viewer_object == nullptr) {
    // Win32 gates the whole dialog on the viewer existing.
    if (QWidget *w = find<QWidget>("IDC_XPOS_EDIT"))
      w->setEnabled(false);
    return;
  }
  if (QLineEdit *edit = find<QLineEdit>("IDC_XPOS_EDIT"))
    edit->setText(QString::number((double)Viewer_object->pos.x(), 'f', 2));
  if (QLineEdit *edit = find<QLineEdit>("IDC_YPOS_EDIT"))
    edit->setText(QString::number((double)Viewer_object->pos.y(), 'f', 2));
  if (QLineEdit *edit = find<QLineEdit>("IDC_ZPOS_EDIT"))
    edit->setText(QString::number((double)Viewer_object->pos.z(), 'f', 2));
  (void)hasViewer;
}

bool ViewerPropDialog::ensureViewer() { return Viewer_object != nullptr; }

void ViewerPropDialog::setOrientation(matrix &m) {
  if (Viewer_object == nullptr)
    return;
  vm_Orthogonalize(&m);
  Viewer_moved = 1;
}

void ViewerPropDialog::onAlignUpYpos() {
  if (Viewer_object == nullptr)
    return;
  matrix *m = &Viewer_object->orient;
  m->rvec.y() = m->fvec.y() = m->uvec.x() = m->uvec.z() = 0;
  m->uvec.y() = 1.0;
  setOrientation(*m);
}

void ViewerPropDialog::onAlignXneg() {
  if (Viewer_object == nullptr)
    return;
  matrix *m = &Viewer_object->orient;
  m->fvec.x() = -1.0;
  m->fvec.y() = m->fvec.z() = 0;
  m->uvec.y() = 1.0;
  m->uvec.x() = m->uvec.z() = 0;
  m->rvec.z() = 1.0;
  m->rvec.x() = m->rvec.y() = 0;
  setOrientation(*m);
}

void ViewerPropDialog::onAlignXpos() {
  if (Viewer_object == nullptr)
    return;
  matrix *m = &Viewer_object->orient;
  m->fvec.x() = 1.0;
  m->fvec.y() = m->fvec.z() = 0;
  m->uvec.y() = 1.0;
  m->uvec.x() = m->uvec.z() = 0;
  m->rvec.z() = -1.0;
  m->rvec.x() = m->rvec.y() = 0;
  setOrientation(*m);
}

void ViewerPropDialog::onAlignYneg() {
  if (Viewer_object == nullptr)
    return;
  matrix *m = &Viewer_object->orient;
  m->fvec.y() = -1.0;
  m->fvec.x() = m->fvec.z() = 0;
  m->uvec.z() = 1.0;
  m->uvec.x() = m->uvec.y() = 0;
  m->rvec.x() = 1.0;
  m->rvec.y() = m->rvec.z() = 0;
  setOrientation(*m);
}

void ViewerPropDialog::onAlignYpos() {
  if (Viewer_object == nullptr)
    return;
  matrix *m = &Viewer_object->orient;
  m->fvec.y() = 1.0;
  m->fvec.x() = m->fvec.z() = 0;
  m->uvec.z() = -1.0;
  m->uvec.x() = m->uvec.y() = 0;
  m->rvec.x() = 1.0;
  m->rvec.y() = m->rvec.z() = 0;
  setOrientation(*m);
}

void ViewerPropDialog::onAlignZneg() {
  if (Viewer_object == nullptr)
    return;
  matrix *m = &Viewer_object->orient;
  m->fvec.z() = -1.0;
  m->fvec.x() = m->fvec.y() = 0;
  m->uvec.y() = 1.0;
  m->uvec.x() = m->uvec.z() = 0;
  m->rvec.x() = -1.0;
  m->rvec.y() = m->rvec.z() = 0;
  setOrientation(*m);
}

void ViewerPropDialog::onAlignZpos() {
  if (Viewer_object == nullptr)
    return;
  matrix *m = &Viewer_object->orient;
  m->fvec.z() = 1.0;
  m->fvec.x() = m->fvec.y() = 0;
  m->uvec.y() = 1.0;
  m->uvec.x() = m->uvec.z() = 0;
  m->rvec.x() = 1.0;
  m->rvec.y() = m->rvec.z() = 0;
  setOrientation(*m);
}

void ViewerPropDialog::onOrientCommit() {
  if (Viewer_object == nullptr)
    return;
  const int pitch = find<QLineEdit>("IDC_PITCH_EDIT")->text().toInt();
  const int heading = find<QLineEdit>("IDC_HEADING_EDIT")->text().toInt();
  const int bank = find<QLineEdit>("IDC_BANK_EDIT")->text().toInt();

  vm_AnglesToMatrix(&Viewer_object->orient, pitch * kAnglesPerDegree, heading * kAnglesPerDegree,
                    bank * kAnglesPerDegree);

  State_changed = Viewer_moved = 1;
}

void ViewerPropDialog::onPosCommit() {
  if (Viewer_object == nullptr)
    return;
  Viewer_object->pos.x() = find<QLineEdit>("IDC_XPOS_EDIT")->text().toFloat();
  Viewer_object->pos.y() = find<QLineEdit>("IDC_YPOS_EDIT")->text().toFloat();
  Viewer_object->pos.z() = find<QLineEdit>("IDC_ZPOS_EDIT")->text().toFloat();
  Viewer_moved = 1;
}

void ViewerPropDialog::onXMoveToggled(bool checked) {
  if (checked)
    Slew_limitations &= ~1;
  else
    Slew_limitations |= 1;
}

void ViewerPropDialog::onYMoveToggled(bool checked) {
  if (checked)
    Slew_limitations &= ~2;
  else
    Slew_limitations |= 2;
}

void ViewerPropDialog::onZMoveToggled(bool checked) {
  if (checked)
    Slew_limitations &= ~4;
  else
    Slew_limitations |= 4;
}

}
