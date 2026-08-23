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

#include "TelComEfxStructs.h"
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class BriefTextDialog; }
QT_END_NAMESPACE

class QPlainTextEdit;


// Mirror of the editor's briefing layout globals (set by the briefing editor).
constexpr int kMaxTelcomScreens = 20;

struct BriefEffect {
  int type;
  int next, prev;
};

struct BriefScreen {
  bool used;
  int root_effect;
  char layout[260];
  BriefEffect effects[32];
  uint32_t mission_mask_set, mission_mask_unset;
  int next, prev;
};

struct BriefLayoutScreen {
  char filename[260];
  int num_texts, num_bmps;
  struct {
    int lx, rx, ty, by;
  } texts[10];
  struct {
    int x, y;
  } bmps[10];
};
extern BriefLayoutScreen *PBlayouts;
extern int *PBnum_layouts;
extern BriefScreen Briefing_screens[kMaxTelcomScreens];

// Port of CBriefTextEdit (IDD_BRIEF_ADDTEXT): edits a briefing text effect.
class BriefTextEditDialog : public QDialog {
  Q_OBJECT
public:
  explicit BriefTextEditDialog(int currScreen, TCTEXTDESC *desc = nullptr,
                               const char *text = nullptr, int id = 0,
                               QWidget *parent = nullptr);
  ~BriefTextEditDialog();

  TCTEXTDESC result() const { return m_desc; }

private slots:
  void onOk();
  void onColorPal();
  void onPredefChanged(int index);
  void onMissionFlags();

private:
  Ui::BriefTextDialog *ui;

  void populatePredefs();
  int m_screen;
  TCTEXTDESC m_desc;
  QString m_text;
  int m_id;
  int m_effectType;
  QPlainTextEdit *m_richEdit;
};

