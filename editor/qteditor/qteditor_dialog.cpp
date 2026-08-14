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

#include <QDebug>
#include <QPushButton>

#include "ui_loader.h"

namespace QtEditor {

Widget::Widget(const QString &uiResource, QWidget *parent) : m_widget(nullptr) {
  m_widget = UiLoader::load(uiResource, parent);
  if (m_widget == nullptr) {
    qWarning() << "Failed to load UI from" << uiResource;
  }
}

Widget::~Widget() { delete m_widget; }

QWidget *Widget::operator[](const QString &objectName) const { return find(objectName); }

void Widget::show() const {
  if (m_widget != nullptr) {
    m_widget->show();
  }
}

Dialog::Dialog(const QString &uiResource, QWidget *parent) : Widget(uiResource, parent) {
  m_dialog = qobject_cast<QDialog *>(m_widget);
  if (m_dialog == nullptr) {
    qWarning() << "UI is not a dialog:" << uiResource;
    return;
  }

  if (QPushButton *ok = m_dialog->findChild<QPushButton *>("IDOK")) {
    connect(ok, &QPushButton::clicked, m_dialog, &QDialog::accept);
  }
  if (QPushButton *cancel = m_dialog->findChild<QPushButton *>("IDCANCEL")) {
    connect(cancel, &QPushButton::clicked, m_dialog, &QDialog::reject);
  }
}

Dialog::~Dialog() = default;

int Dialog::exec() const {
  if (m_dialog != nullptr) {
    return m_dialog->exec();
  }
  return QDialog::Rejected;
}

}
