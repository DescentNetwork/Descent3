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
#include "utils.h"
#include "objinfo.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WorldObjectsGenericDialog; }
QT_END_NAMESPACE


// Port of CWorldObjectsGenericDialog (IDD_WORLDOBJECTSGENERIC): the
// "intelligent object" editor shared by buildings, clutter, robots and
// powerups. Edits the object type table (model + LODs, physics/AI, death
// spew, sounds, inventory, script module, destroyable/score/ammo).
class WorldObjectsGenericDialog : public QDialog {
  Q_OBJECT
public:
  explicit WorldObjectsGenericDialog(object_type_e objType, index_t object_id, QWidget *parent = nullptr);
  ~WorldObjectsGenericDialog();

  index_t objectId() const { return m_object_id; }

private slots:
  void onAddNew();
  void onCheckedOut();
  void onCheckIn();
  void onDelete();
  void onLock();
  void onUndoLock();
  void onNext();
  void onPrev();
  void onNamePulldownChanged();
  void onCopy();
  void onPaste();
  void onWeaponInfo();
  void onLight();
  void onDefaultRadius();
  void onSelScript();
  void onNolod();
  void onKillfocusInvenDescription();
  void onOverride();
  void onKillfocusLodDistance();
  void onKillfocusRespawnScalar();

private:
  void updateDialog();
  void enableDisableAll(bool flag);
  bool isLocked(index_t n);
  uint32_t countLockedItems();
  void setObjectId(index_t id);
  void saveGenericsOnClose();
  optref<object_info> data(void);

  Ui::WorldObjectsGenericDialog *ui;
  object_type_e m_type;
  index_t m_object_id;
  int m_lod = 0;
  int m_locked_count = 0;
};
