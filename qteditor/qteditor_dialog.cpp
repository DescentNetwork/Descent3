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

#include "qteditor_dialog.h"

#include <QPushButton>

#include "qt_ui_loader.h"

namespace QtEditor {

void adoptUi(QWidget *self, QWidget *loaded) {
  if (loaded == nullptr)
    return;

  self->setWindowTitle(loaded->windowTitle());
  self->setWindowFlags(loaded->windowFlags());
  self->setEnabled(loaded->isEnabled());
  self->setMinimumSize(loaded->minimumSize());
  self->setMaximumSize(loaded->maximumSize());
  self->setGeometry(loaded->geometry());

  const QList<QWidget *> children = loaded->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
  for (QWidget *child : children)
    child->setParent(self);

  delete loaded;
}

Widget::Widget(const QString &uiResource, QWidget *parent) : QWidget(parent) {
  loadUi(uiResource);
}

Widget::~Widget() = default;

void Widget::loadUi(const QString &uiResource) {
  adoptUi(this, UiLoader::load(uiResource));
}

Dialog::Dialog(const QString &uiResource, QWidget *parent) : QDialog(parent) {
  loadUi(uiResource);
  if (QPushButton *ok = findChild<QPushButton *>("IDOK"))
    connect(ok, &QPushButton::clicked, this, &QDialog::accept);
  if (QPushButton *cancel = findChild<QPushButton *>("IDCANCEL"))
    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
}

Dialog::~Dialog() = default;

void Dialog::loadUi(const QString &uiResource) {
  adoptUi(this, UiLoader::load(uiResource));
}

}
