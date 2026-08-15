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

#include "brief_text_edit_dialog.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QStringList>
#include <cstdio>
#include <cstring>

#include "brief_mission_flags_dialog.h"
#include "gamefont.h"
#include "grdefs.h"

namespace QtEditor {

BriefLayoutScreen *PBlayouts = nullptr;
int *PBnum_layouts = nullptr;
BriefScreen Briefing_screens[kMaxTelcomScreens]{};

namespace {
constexpr int kMaxTabStops = 10;

int effectTypeToRadio(TCTEXTDESC *desc) {
  switch (desc->type) {
  case TC_TEXT_SCROLL:
    switch (desc->flags) {
    case TC_TEXTF_L2R:
      return 4;
    case TC_TEXTF_R2L:
      return 5;
    case TC_TEXTF_T2B:
      return 6;
    case TC_TEXTF_B2T:
      return 7;
    default:
      return 4;
    }
  case TC_TEXT_FADE:
    switch (desc->flags) {
    case TC_TEXTF_IN:
      return 2;
    case TC_TEXTF_OUT:
      return 3;
    default:
      return 2;
    }
  case TC_TEXT_FLASH:
    return 1;
  default:
    return 0;
  }
}

void radioToEffectType(int effectType, TCTEXTDESC *desc) {
  switch (effectType) {
  case 0:
    desc->type = TC_TEXT_STATIC;
    break;
  case 1:
    desc->type = TC_TEXT_FLASH;
    break;
  case 2:
    desc->type = TC_TEXT_FADE;
    desc->flags = TC_TEXTF_IN;
    break;
  case 3:
    desc->type = TC_TEXT_FADE;
    desc->flags = TC_TEXTF_OUT;
    break;
  case 4:
    desc->type = TC_TEXT_SCROLL;
    desc->flags = TC_TEXTF_L2R;
    break;
  case 5:
    desc->type = TC_TEXT_SCROLL;
    desc->flags = TC_TEXTF_R2L;
    break;
  case 6:
    desc->type = TC_TEXT_SCROLL;
    desc->flags = TC_TEXTF_T2B;
    break;
  case 7:
    desc->type = TC_TEXT_SCROLL;
    desc->flags = TC_TEXTF_B2T;
    break;
  }
}
} // namespace

BriefTextEditDialog::BriefTextEditDialog(int currScreen, TCTEXTDESC *d, const char *text_buffer,
                                         int id, QWidget *parent)
    : Dialog(":/ui/brief_addtext.ui", parent), m_screen(currScreen), m_text(""), m_id(id),
      m_effectType(0), m_richEdit(nullptr) {
  std::memset(&m_desc, 0, sizeof(TCTEXTDESC));
  m_desc.type = TC_TEXT_STATIC;
  m_desc.font = BRIEF_FONT_INDEX;
  m_desc.color = GR_GREEN;
  m_desc.speed = 1.0f;
  m_desc.looping = false;
  m_desc.waittime = 0;
  m_desc.textbox.left = m_desc.textbox.top = 0;
  m_desc.textbox.right = 639;
  m_desc.textbox.bottom = 479;
  m_desc.mission_mask_set = 0;
  m_desc.mission_mask_unset = 0;

  if (d) {
    m_desc.caps = d->caps;
    m_desc.flags = d->flags;
    m_desc.type = d->type;
    if (d->caps & TCTD_FONT)
      m_desc.font = d->font;
    if (d->caps & TCTD_COLOR)
      m_desc.color = d->color;
    if (d->caps & TCTD_SPEED)
      m_desc.speed = d->speed;
    if (d->caps & TCTD_LOOPING)
      m_desc.looping = d->looping;
    if (d->caps & TCTD_TEXTBOX)
      std::memcpy(&m_desc.textbox, &d->textbox, sizeof(tc_text));
    if (d->caps & TCTD_WAITTIME)
      m_desc.waittime = d->waittime;
    m_desc.mission_mask_set = d->mission_mask_set;
    m_desc.mission_mask_unset = d->mission_mask_unset;
  }
  if (text_buffer)
    m_text = text_buffer;

  m_effectType = effectTypeToRadio(&m_desc);

  if (auto *edit = find<QLineEdit>("IDC_BRIEF_T_ID"))
    edit->setText(QString::number(m_id));
  if (auto *edit = find<QLineEdit>("IDC_BRIEF_T_LR_Y"))
    edit->setText(QString::number(m_desc.textbox.bottom));
  if (auto *edit = find<QLineEdit>("IDC_BRIEF_T_LR_X"))
    edit->setText(QString::number(m_desc.textbox.right));
  if (auto *edit = find<QLineEdit>("IDC_BRIEF_T_SPEED"))
    edit->setText(QString::number(m_desc.speed));
  if (auto *edit = find<QLineEdit>("IDC_BRIEF_T_STARTTIME"))
    edit->setText(QString::number(m_desc.waittime));
  if (auto *edit = find<QLineEdit>("IDC_BRIEF_T_UL_Y"))
    edit->setText(QString::number(m_desc.textbox.top));
  if (auto *edit = find<QLineEdit>("IDC_BRIEF_T_UL_X"))
    edit->setText(QString::number(m_desc.textbox.left));
  if (auto *edit = find<QLineEdit>("IDC_BRIEF_T_DESC"))
    edit->setText(m_desc.caps ? "" : "");

  const char *effectRadios[] = {"IDC_BRIEF_T_STATIC", "IDC_BRIEF_T_FLASH", "IDC_BRIEF_T_FADEIN",
                                "IDC_BRIEF_T_FADEOUT", "IDC_BRIEF_T_SL2R", "IDC_BRIEF_T_SR2L",
                                "IDC_BRIEF_T_ST2B", "IDC_BRIEF_T_SB2T"};
  if (m_effectType >= 0 && m_effectType < 8) {
    if (auto *rb = find<QRadioButton>(effectRadios[m_effectType]))
      rb->setChecked(true);
  }

  find<QCheckBox>("IDC_TABSTOP")->setChecked((m_desc.caps & TCTD_TABSTOP) != 0);
  find<QLineEdit>("IDC_BRIEF_COLOR_R")->setText(QString::number((m_desc.color >> 16) & 0xff));
  find<QLineEdit>("IDC_BRIEF_COLOR_G")->setText(QString::number((m_desc.color >> 8) & 0xff));
  find<QLineEdit>("IDC_BRIEF_COLOR_B")->setText(QString::number(m_desc.color & 0xff));

  if (auto *combo = find<QComboBox>("IDC_BRIEF_T_FONT")) {
    combo->addItem("sm_brief");
    combo->addItem("lg_brief");
    combo->setCurrentIndex(m_desc.font == BRIEF_FONT_INDEX ? 0 : 1);
  }

  // The .ui has a QLabel placeholder (IDC_RICHFRAME); overlay a text editor on
  // top of it, mirroring the MFC CRichEditCtrl placement.
  if (auto *frame = find<QLabel>("IDC_RICHFRAME")) {
    const QRect rect = QRect(frame->mapTo(m_dialog, QPoint(0, 0)), frame->size()).adjusted(2, 2, -2, -2);
    m_richEdit = new QPlainTextEdit(m_dialog);
    m_richEdit->setObjectName("IDC_SCRIPTVIEW");
    m_richEdit->setGeometry(rect);
    m_richEdit->setFont(QFont("Courier"));
    m_richEdit->setTabStopDistance(360 * QFontMetrics(m_richEdit->font()).horizontalAdvance(' '));
    m_richEdit->setPlainText(m_text);
    m_richEdit->show();
    frame->hide();
  }

  if (auto *combo = find<QComboBox>("IDC_BRIEF_T_PREDEF")) {
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &BriefTextEditDialog::onPredefChanged);
    populatePredefs();
  }
  if (auto *btn = find<QPushButton>("IDC_BRIEF_COLOR_PAL"))
    connect(btn, &QPushButton::clicked, this, &BriefTextEditDialog::onColorPal);
  if (auto *btn = find<QPushButton>("IDC_MISSIONFLAGS"))
    connect(btn, &QPushButton::clicked, this, &BriefTextEditDialog::onMissionFlags);
  if (QPushButton *ok = find<QPushButton>("IDOK")) {
    disconnect(ok, &QPushButton::clicked, m_dialog, &QDialog::accept);
    connect(ok, &QPushButton::clicked, this, &BriefTextEditDialog::onOk);
  }
}

BriefTextEditDialog::~BriefTextEditDialog() = default;

void BriefTextEditDialog::populatePredefs() {
  if (auto *combo = find<QComboBox>("IDC_BRIEF_T_PREDEF")) {
    combo->clear();
    combo->addItem("<Raw>");
    int layout = -1;
    if (PBlayouts && PBnum_layouts) {
      for (int i = 0; i < *PBnum_layouts; i++) {
        if (std::strcmp(Briefing_screens[m_screen].layout, PBlayouts[i].filename) == 0)
          layout = i;
      }
      if (layout != -1) {
        for (int j = 0; j < PBlayouts[layout].num_texts; j++) {
          char buffer[100];
          std::sprintf(buffer, "(%d,%d)->(%d,%d)", PBlayouts[layout].texts[j].lx,
                       PBlayouts[layout].texts[j].ty, PBlayouts[layout].texts[j].rx,
                       PBlayouts[layout].texts[j].by);
          combo->addItem(buffer);
        }
      }
    }
    combo->setCurrentIndex(0);
  }
}

void BriefTextEditDialog::onPredefChanged(int index) {
  auto *combo = find<QComboBox>("IDC_BRIEF_T_PREDEF");
  if (!combo || index <= 0)
    return;
  int layout = -1;
  if (!PBlayouts || !PBnum_layouts)
    return;
  for (int i = 0; i < *PBnum_layouts; i++) {
    if (std::strcmp(Briefing_screens[m_screen].layout, PBlayouts[i].filename) == 0)
      layout = i;
  }
  if (layout != -1) {
    find<QLineEdit>("IDC_BRIEF_T_UL_X")->setText(QString::number(PBlayouts[layout].texts[index - 1].lx));
    find<QLineEdit>("IDC_BRIEF_T_UL_Y")->setText(QString::number(PBlayouts[layout].texts[index - 1].ty));
    find<QLineEdit>("IDC_BRIEF_T_LR_X")->setText(QString::number(PBlayouts[layout].texts[index - 1].rx));
    find<QLineEdit>("IDC_BRIEF_T_LR_Y")->setText(QString::number(PBlayouts[layout].texts[index - 1].by));
  }
}

void BriefTextEditDialog::onColorPal() {
  auto *red = find<QLineEdit>("IDC_BRIEF_COLOR_R");
  auto *green = find<QLineEdit>("IDC_BRIEF_COLOR_G");
  auto *blue = find<QLineEdit>("IDC_BRIEF_COLOR_B");
  QColor color(red->text().toInt(), green->text().toInt(), blue->text().toInt());
  const QColor chosen = QColorDialog::getColor(color, m_dialog);
  if (chosen.isValid()) {
    red->setText(QString::number(chosen.red()));
    green->setText(QString::number(chosen.green()));
    blue->setText(QString::number(chosen.blue()));
  }
}

void BriefTextEditDialog::onMissionFlags() {
  BriefMissionFlagsDialog dlg(m_desc.mission_mask_set, m_desc.mission_mask_unset, m_dialog);
  if (dlg.exec() == QDialog::Accepted) {
    m_desc.mission_mask_set = dlg.setFlags();
    m_desc.mission_mask_unset = dlg.unsetFlags();
  }
}

void BriefTextEditDialog::onOk() {
  m_desc.caps = TCTD_FONT | TCTD_COLOR | TCTD_SPEED | TCTD_LOOPING | TCTD_WAITTIME | TCTD_TEXTBOX |
                TCTD_SCROLL;
  m_desc.textbox.bottom = find<QLineEdit>("IDC_BRIEF_T_LR_Y")->text().toInt();
  m_desc.textbox.right = find<QLineEdit>("IDC_BRIEF_T_LR_X")->text().toInt();
  m_desc.textbox.top = find<QLineEdit>("IDC_BRIEF_T_UL_Y")->text().toInt();
  m_desc.textbox.left = find<QLineEdit>("IDC_BRIEF_T_UL_X")->text().toInt();
  m_desc.speed = find<QLineEdit>("IDC_BRIEF_T_SPEED")->text().toFloat();
  m_desc.waittime = find<QLineEdit>("IDC_BRIEF_T_STARTTIME")->text().toFloat();
  m_desc.color = GR_RGB(find<QLineEdit>("IDC_BRIEF_COLOR_R")->text().toInt(),
                        find<QLineEdit>("IDC_BRIEF_COLOR_G")->text().toInt(),
                        find<QLineEdit>("IDC_BRIEF_COLOR_B")->text().toInt());
  if (find<QCheckBox>("IDC_TABSTOP")->isChecked())
    m_desc.caps |= TCTD_TABSTOP;

  auto *combo = find<QComboBox>("IDC_BRIEF_T_FONT");
  m_desc.font = (combo && combo->currentIndex() == 1) ? BBRIEF_FONT_INDEX : BRIEF_FONT_INDEX;

  int effectType = 0;
  const char *effectRadios[] = {"IDC_BRIEF_T_STATIC", "IDC_BRIEF_T_FLASH", "IDC_BRIEF_T_FADEIN",
                                "IDC_BRIEF_T_FADEOUT", "IDC_BRIEF_T_SL2R", "IDC_BRIEF_T_SR2L",
                                "IDC_BRIEF_T_ST2B", "IDC_BRIEF_T_SB2T"};
  for (int i = 0; i < 8; i++) {
    if (auto *rb = find<QRadioButton>(effectRadios[i]); rb && rb->isChecked()) {
      effectType = i;
      break;
    }
  }
  radioToEffectType(effectType, &m_desc);

  if (m_richEdit)
    m_text = m_richEdit->toPlainText();
  m_dialog->accept();
}

}
