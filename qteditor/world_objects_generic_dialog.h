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

#include "qteditor_dialog.h"

namespace QtEditor {

// Port of CWorldObjectsGenericDialog (IDD_WORLDOBJECTSGENERIC): the
// "intelligent object" editor shared by buildings, clutter, robots and
// powerups. Edits the object type table (model + LODs, physics/AI, death
// spew, sounds, inventory, script module, destroyable/score/ammo).
class WorldObjectsGenericDialog : public Dialog {
  Q_OBJECT
public:
  explicit WorldObjectsGenericDialog(int objType, int current, QWidget *parent = nullptr);
  ~WorldObjectsGenericDialog() override;

  int current() const { return m_current; }

private slots:
  void onEditAI();
  void onEditPhysics();
  void onUsesAI(bool checked);
  void onUsesPhysics(bool checked);
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
  void onDestroyableToggled(bool checked);
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
  void onInvenSelectable(bool checked);
  void onInvenNonuseable(bool checked);
  void onKillfocusLodDistance();
  void onInvtypeGame();
  void onInvtypeMission();
  void onInvenNoremove(bool checked);
  void onInvenViswhenused(bool checked);
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
  void onAiScriptedDeath(bool checked);
  void onKillfocusRespawnScalar();
  void onGenericDeaths();
  void onKillfocusScore();
  void onKillfocusAmmo();
  void onObjCeiling(bool checked);
  void onFlyThroughRenderedPortals(bool checked);
  void onNsc(bool checked);
  void onDsmpbd(bool checked);
  void onAmbient(bool checked);

private:
  void updateDialog();
  void enableDisableAll(bool flag);
  bool isLocked(int n);
  int countLockedItems();
  void setFlag(uint32_t flag, const char *checkName, bool checked);
  void setCurrent(int id);
  void saveGenericsOnClose();

  int m_type;
  int m_current;
  int m_lod = 0;
  int m_locked_count = 0;
};

// Opens the generic object dialog for the given object type (matching the
// original MainFrm handlers) and returns the resulting current id.
int editGenericObject(int objType, int initialCurrent, QWidget *parent);

}
