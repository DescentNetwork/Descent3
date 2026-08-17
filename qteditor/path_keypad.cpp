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

#include "path_keypad.h"
#include "ui_pathkeypad.h"

#include <QCheckBox>
#include <QComboBox>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>

#include "qt_messagebox.h"
#include "d3edit.h"
#include "gamepath.h"
#include "vecmat.h"

// Editor-side path helpers provided in d3_editor_state.cpp.
int InsertNodeIntoPath(int pathnum, int nodenum, int flags, int roomnum, vector pos, matrix orient);
void DeleteNodeFromPath(int pathnum, int nodenum);
int GetNextPath(int n);
int GetPrevPath(int n);
int GetFirstPath();


PathKeypad::PathKeypad(QWidget *parent)
    : QDialog(parent), ui(new Ui::PathKeypad)
{
  ui->setupUi(this);
  if (QPushButton *b = ui->IDC_PATHPAD_ADD_PATH)
    connect(b, &QPushButton::clicked, this, &PathKeypad::onAddPath);
  if (QPushButton *b = ui->IDC_DELETE_PATH)
    connect(b, &QPushButton::clicked, this, &PathKeypad::onDeletePath);
  if (QPushButton *b = ui->IDC_PATHPAD_NEXT_NODE)
    connect(b, &QPushButton::clicked, this, &PathKeypad::onNextNode);
  if (QPushButton *b = ui->IDC_PATHPAD_PREV_NODE)
    connect(b, &QPushButton::clicked, this, &PathKeypad::onPrevNode);
  if (QPushButton *b = ui->IDC_PATHPAD_INSERT_NODE)
    connect(b, &QPushButton::clicked, this, &PathKeypad::onInsertNode);
  if (QPushButton *b = ui->IDC_PATHPAD_DELETE_NODE)
    connect(b, &QPushButton::clicked, this, &PathKeypad::onDeleteNode);
  if (QPushButton *b = ui->IDC_PATHPAD_MOVE_FORWARD)
    connect(b, &QPushButton::clicked, this, &PathKeypad::onMoveNode);
  if (QPushButton *b = ui->IDC_PATHPAD_MOVE_BACKWARD)
    connect(b, &QPushButton::clicked, this, &PathKeypad::onMoveNode);
  if (QPushButton *b = ui->IDC_PATHPAD_MOVE_LEFT)
    connect(b, &QPushButton::clicked, this, &PathKeypad::onMoveNode);
  if (QPushButton *b = ui->IDC_PATHPAD_MOVE_RIGHT)
    connect(b, &QPushButton::clicked, this, &PathKeypad::onMoveNode);
  if (QPushButton *b = ui->IDC_PATHPAD_MOVE_UP)
    connect(b, &QPushButton::clicked, this, &PathKeypad::onMoveNode);
  if (QPushButton *b = ui->IDC_PATHPAD_MOVE_DOWN)
    connect(b, &QPushButton::clicked, this, &PathKeypad::onMoveNode);

  if (QComboBox *combo = ui->IDC_PATHPAD_PULLDOWN)
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this, &PathKeypad::onPathPulldownChanged);
  if (QLineEdit *edit = ui->IDC_CURRENT_NODE_EDIT)
    connect(edit, &QLineEdit::editingFinished, this, &PathKeypad::onCurrentNodeEdited);
  if (QLineEdit *edit = ui->IDC_PATH_INC_TEXT)
    connect(edit, &QLineEdit::editingFinished, this, &PathKeypad::onIncEdited);
  if (QCheckBox *cb = ui->IDC_SHOW_NODES_CHECK)
    connect(cb, &QCheckBox::toggled, this, &PathKeypad::onShowNodesToggled);

  updateDialog();
}

PathKeypad::~PathKeypad() { delete ui; }

int PathKeypad::currentPath() { return D3EditState.current_path; }

int PathKeypad::currentNode() {
  const int p = currentPath();
  if (p < 0 || p >= MAX_GAME_PATHS || !GamePaths[p].used)
    return -1;
  if (D3EditState.current_node >= GamePaths[p].num_nodes)
    D3EditState.current_node = GamePaths[p].num_nodes - 1;
  return D3EditState.current_node;
}

void PathKeypad::updateDialog() {
  // Win32 disables path editing when there is no current path (requires a
  // loaded level with paths).
  const bool active = (D3EditState.current_path >= 0 && D3EditState.current_path < MAX_GAME_PATHS &&
                       GamePaths[D3EditState.current_path].used);
  const QList<QWidget *> all = this->findChildren<QWidget *>();
  for (QWidget *w : all) {
    if (w->objectName().startsWith("IDC_PATHPAD") || w->objectName().startsWith("IDC_DELETE_PATH") ||
        w->objectName().startsWith("IDC_CURRENT_NODE_EDIT") || w->objectName().startsWith("IDC_SHOW_NODES_CHECK"))
      w->setEnabled(active);
  }
  if (!active)
    return;
  const int p = currentPath();
  if (p < 0 || p >= MAX_GAME_PATHS || !GamePaths[p].used)
    return;

  if (QComboBox *combo = ui->IDC_PATHPAD_PULLDOWN) {
    QSignalBlocker blocker(combo);
    combo->clear();
    for (int i = 0; i < MAX_GAME_PATHS; i++)
      if (GamePaths[i].used)
        combo->addItem(GamePaths[i].name);
    combo->setCurrentText(GamePaths[p].name);
  }

  if (QLabel *label = ui->IDC_PATHPAD_NUM_NODES)
    label->setText(QString::number(GamePaths[p].num_nodes));

  const int n = currentNode();
  if (QLineEdit *edit = ui->IDC_CURRENT_NODE_EDIT)
    edit->setText(QString::number(n));
  if (QLabel *label = ui->IDC_PATHPAD_CUR_NODE_ROOM) {
    if (n >= 0 && n < GamePaths[p].num_nodes)
      label->setText(QString::number(GamePaths[p].pathnodes[n].roomnum));
  }
}

void PathKeypad::onAddPath() {
  bool ok = false;
  const QString name = QInputDialog::getText(this, "Add path", "Enter a name for the path:", QLineEdit::Normal, "",
                                             &ok);
  if (!ok || name.isEmpty())
    return;
  int pathnum = -1;
  for (int i = 0; i < MAX_GAME_PATHS; i++) {
    if (!GamePaths[i].used) {
      pathnum = i;
      break;
    }
  }
  if (pathnum == -1) {
    OutrageMessageBox("Error: Too many paths in the level.");
    return;
  }
  snprintf(GamePaths[pathnum].name, sizeof(GamePaths[pathnum].name), "%s",
           name.toLocal8Bit().constData());
  GamePaths[pathnum].used = true;
  GamePaths[pathnum].num_nodes = 0;
  Num_game_paths++;
  D3EditState.current_path = pathnum;
  D3EditState.current_node = 0;
  updateDialog();
}

void PathKeypad::onDeletePath() {
  const int p = currentPath();
  if (p < 0)
    return;
  if (GamePaths[p].num_nodes > 0) {
    mem_free(GamePaths[p].pathnodes);
    GamePaths[p].pathnodes = nullptr;
  }
  GamePaths[p].used = false;
  GamePaths[p].num_nodes = 0;
  Num_game_paths--;
  D3EditState.current_path = GetNextPath(p);
  D3EditState.current_node = 0;
  updateDialog();
}

void PathKeypad::onPathPulldownChanged() {
  QComboBox *combo = ui->IDC_PATHPAD_PULLDOWN;
  const int i = FindGamePathName(combo->currentText().toLocal8Bit().constData());
  if (i == -1)
    return;
  D3EditState.current_path = i;
  D3EditState.current_node = 0;
  updateDialog();
}

void PathKeypad::onNextNode() {
  const int p = currentPath();
  if (p < 0)
    return;
  if (GamePaths[p].num_nodes == 0)
    return;
  D3EditState.current_node = (D3EditState.current_node + 1) % GamePaths[p].num_nodes;
  updateDialog();
}

void PathKeypad::onPrevNode() {
  const int p = currentPath();
  if (p < 0)
    return;
  if (GamePaths[p].num_nodes == 0)
    return;
  D3EditState.current_node =
      (D3EditState.current_node <= 0) ? (GamePaths[p].num_nodes - 1) : (D3EditState.current_node - 1);
  updateDialog();
}

void PathKeypad::onCurrentNodeEdited() {
  const int p = currentPath();
  if (p < 0)
    return;
  int n = ui->IDC_CURRENT_NODE_EDIT->text().toInt();
  if (n < 0)
    n = 0;
  if (n >= GamePaths[p].num_nodes)
    n = GamePaths[p].num_nodes - 1;
  D3EditState.current_node = n;
  updateDialog();
}

void PathKeypad::onInsertNode() {
  const int p = currentPath();
  const int n = currentNode();
  if (p < 0 || n < 0)
    return;
  matrix orient;
  vm_MakeIdentity(&orient);
  vector zero_pos;
  vm_MakeZero(&zero_pos);
  const int newnode = InsertNodeIntoPath(p, n, 0, 0, zero_pos, orient);
  if (newnode != -1)
    D3EditState.current_node = newnode;
  updateDialog();
}

void PathKeypad::onDeleteNode() {
  const int p = currentPath();
  const int n = currentNode();
  if (p < 0 || n < 0)
    return;
  if (GamePaths[p].num_nodes == 1) {
    OutrageMessageBox("Cannot delete the only node in a path.");
    return;
  }
  DeleteNodeFromPath(p, n);
  if (D3EditState.current_node >= GamePaths[p].num_nodes)
    D3EditState.current_node = GamePaths[p].num_nodes - 1;
  updateDialog();
}

void PathKeypad::onMoveNode() {
  const int p = currentPath();
  const int n = currentNode();
  if (p < 0 || n < 0)
    return;
  node *nd = &GamePaths[p].pathnodes[n];
  QPushButton *btn = qobject_cast<QPushButton *>(sender());
  if (btn == nullptr)
    return;
  const QString name = btn->objectName();
  if (name == "IDC_PATHPAD_MOVE_FORWARD")
    nd->pos += nd->fvec * m_inc;
  else if (name == "IDC_PATHPAD_MOVE_BACKWARD")
    nd->pos -= nd->fvec * m_inc;
  else if (name == "IDC_PATHPAD_MOVE_LEFT")
    nd->pos -= nd->uvec * m_inc;
  else if (name == "IDC_PATHPAD_MOVE_RIGHT")
    nd->pos += nd->uvec * m_inc;
  else if (name == "IDC_PATHPAD_MOVE_UP")
    nd->pos.y() += m_inc;
  else if (name == "IDC_PATHPAD_MOVE_DOWN")
    nd->pos.y() -= m_inc;
}

void PathKeypad::onShowNodesToggled(bool) {
  // Path nodes are drawn by the 3D view; the preference only affects rendering.
}

void PathKeypad::onIncEdited() {
  m_inc = ui->IDC_PATH_INC_TEXT->text().toFloat();
  if (m_inc <= 0)
    m_inc = 1.0f;
}

