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
namespace Ui { class WorldObjectsPlayerDialog; }
QT_END_NAMESPACE


// Port of CWorldObjectsPlayerDialog (IDD_WORLDOBJECTSPLAYER): edits the player
// ship table (model + LODs, dying model, cockpit, armor, physics, weapons).
class WorldObjectsPlayerDialog : public QDialog {
  Q_OBJECT
public:
  explicit WorldObjectsPlayerDialog(QWidget *parent = nullptr);
  ~WorldObjectsPlayerDialog();

private slots:
  void onAddPship();
  void onPshipDelete();
  void onPshipLock();
  void onPshipCheckin();
  void onPshipsOut();
  void onPshipNext();
  void onPshipPrev();
  void onPshipPulldownChanged();
  void onPshipLoadModel();
  void onPshipDyingModel();
  void onNullDying();
  void onEditWeapons();
  void onPshipCockpit();
  void onPshipEditPhysics();
  void onKillfocusName();
  void onKillfocusCockpit();
  void onKillfocusArmor();
  void onKillfocusLodDistance();
  void onDefaultAllowToggled(bool checked);
  void onHiresRadio();
  void onMedresRadio();
  void onLoresRadio();
  void onNolod();

private:
  void updateDialog();

  Ui::WorldObjectsPlayerDialog *ui;
  int m_lod = 0;
};

