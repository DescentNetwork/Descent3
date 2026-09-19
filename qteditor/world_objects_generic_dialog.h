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
namespace Ui { class WorldObjectsGenericDialog; }
QT_END_NAMESPACE


// Port of CWorldObjectsGenericDialog (IDD_WORLDOBJECTSGENERIC): the
// "intelligent object" editor shared by buildings, clutter, robots and
// powerups. Edits the object type table (model + LODs, physics/AI, death
// spew, sounds, inventory, script module, destroyable/score/ammo).
class WorldObjectsGenericDialog : public QDialog {
  Q_OBJECT
public:
  explicit WorldObjectsGenericDialog(int objType, int current, QWidget *parent = nullptr);
  ~WorldObjectsGenericDialog();

  int current() const { return m_current; }

private slots:
  void onEditAI();
  void onEditPhysics();
  void onAddNew();
  void onCheckedOut();
  void onCheckIn();
  void onDefineAnimStates();
  void onDelete();
  void onLock();
  void onUndoLock();
  void onNext();
  void onPrev();
  void onNamePulldownChanged();
  void onKillfocusSize();
  void onCopy();
  void onPaste();
  void onKillfocusHitpoints();
  void onWeaponInfo();
  void onLight();
  void onDefaultRadius();
  void onSelScript();
  void onKillfocusImpactDamage();
  void onKillfocusImpactSize();
  void onNolod();
  void onHiresRadio();
  void onMedresRadio();
  void onLoresRadio();
  void onKillfocusImpactTime();
  void onExplosionSoundChanged();
  void onAmbientSoundChanged();
  void onKillfocusInvenDescription();
  void onKillfocusInvenIconname();
  void onOverride();
  void onKillfocusLodDistance();
  void onInvtypeGame();
  void onInvtypeMission();
  void onDeathPowerup1Changed();
  void onDeathPowerup1NumEdited();
  void onDeathPowerup1PercentEdited();
  void onDeathPowerup2Changed();
  void onDeathPowerup2NumEdited();
  void onDeathPowerup2PercentEdited();
  void onDeathPowerupUse2(bool checked);
  void onDeathSpew2IfZero1(bool checked);
  void onKillfocusScriptname();
  void onCompilemodule();
  void onKillfocusScriptOverride();
  void onKillfocusRespawnScalar();
  void onGenericDeaths();
  void onKillfocusScore();
  void onKillfocusAmmo();

private:
  void updateDialog();
  void enableDisableAll(bool flag);
  bool isLocked(int n);
  int countLockedItems();
  void setCurrent(int id);
  void saveGenericsOnClose();

  Ui::WorldObjectsGenericDialog *ui;
  int m_type;
  int m_current;
  int m_lod = 0;
  int m_locked_count = 0;
};

// Opens the generic object dialog for the given object type (matching the
// original MainFrm handlers) and returns the resulting current id.
int editGenericObject(int objType, int initialCurrent, QWidget *parent);

