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
#include "ui_brief_addtext.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QStringList>

#include <array>

#include "brief_mission_flags_dialog.h"
#include "gamefont.h"
#include "grdefs.h"

namespace {
constexpr int kMaxTabStops = 10;

int effectTypeToRadio(TCTEXTDESC *desc) {
  switch (desc->type) {
  case TC_TEXT_SCROLL:
    switch (desc->mode) {
    case tc_text_mode::scroll_l2r:
      return 4;
    case tc_text_mode::scroll_r2l:
      return 5;
    case tc_text_mode::scroll_t2b:
      return 6;
    case tc_text_mode::scroll_b2t:
      return 7;
    default:
      return 4;
    }
  case TC_TEXT_FADE:
    switch (desc->mode) {
    case tc_text_mode::fade_in:
      return 2;
    case tc_text_mode::fade_out:
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
    desc->mode = tc_text_mode::fade_in;
    break;
  case 3:
    desc->type = TC_TEXT_FADE;
    desc->mode = tc_text_mode::fade_out;
    break;
  case 4:
    desc->type = TC_TEXT_SCROLL;
    desc->mode = tc_text_mode::scroll_l2r;
    break;
  case 5:
    desc->type = TC_TEXT_SCROLL;
    desc->mode = tc_text_mode::scroll_r2l;
    break;
  case 6:
    desc->type = TC_TEXT_SCROLL;
    desc->mode = tc_text_mode::scroll_t2b;
    break;
  case 7:
    desc->type = TC_TEXT_SCROLL;
    desc->mode = tc_text_mode::scroll_b2t;
    break;
  }
}
} // namespace

BriefTextEditDialog::BriefTextEditDialog(int currScreen, TCTEXTDESC *d, const std::string &text,
                                         int id, QWidget *parent)
    : QDialog(parent), ui(new Ui::BriefTextDialog), m_screen(currScreen), m_text(""), m_id(id),
      m_effectType(0), m_richEdit(nullptr)
{
  ui->setupUi(this);
  m_desc = TCTEXTDESC{};
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
    m_desc.mode = d->mode;
    m_desc.type = d->type;
    if (d->caps.font)
      m_desc.font = d->font;
    if (d->caps.color)
      m_desc.color = d->color;
    if (d->caps.speed)
      m_desc.speed = d->speed;
    if (d->caps.looping)
      m_desc.looping = d->looping;
    if (d->caps.textbox)
      m_desc.textbox = d->textbox;
    if (d->caps.waittime)
      m_desc.waittime = d->waittime;
    m_desc.mission_mask_set = d->mission_mask_set;
    m_desc.mission_mask_unset = d->mission_mask_unset;
  }
  m_text = QString::fromStdString(text);

  m_effectType = effectTypeToRadio(&m_desc);

  if (auto *edit = ui->IDC_BRIEF_T_ID)
    edit->setText(QString::number(m_id));
  if (auto *edit = ui->IDC_BRIEF_T_LR_Y)
    edit->setText(QString::number(m_desc.textbox.bottom));
  if (auto *edit = ui->IDC_BRIEF_T_LR_X)
    edit->setText(QString::number(m_desc.textbox.right));
  if (auto *edit = ui->IDC_BRIEF_T_SPEED)
    edit->setText(QString::number(m_desc.speed));
  if (auto *edit = ui->IDC_BRIEF_T_STARTTIME)
    edit->setText(QString::number(m_desc.waittime));
  if (auto *edit = ui->IDC_BRIEF_T_UL_Y)
    edit->setText(QString::number(m_desc.textbox.top));
  if (auto *edit = ui->IDC_BRIEF_T_UL_X)
    edit->setText(QString::number(m_desc.textbox.left));
  if (auto *edit = ui->IDC_BRIEF_T_DESC)
    edit->setText((m_desc.caps.font || m_desc.caps.color || m_desc.caps.speed || m_desc.caps.looping ||
                   m_desc.caps.waittime || m_desc.caps.textbox || m_desc.caps.scroll || m_desc.caps.tabstop)
                      ? ""
                      : "");

  ui->IDC_BRIEF_T_STATIC->setChecked(m_effectType == 0);
  ui->IDC_BRIEF_T_FADEIN->setChecked(m_effectType == 2);
  ui->IDC_BRIEF_T_FADEOUT->setChecked(m_effectType == 3);
  ui->IDC_BRIEF_T_SL2R->setChecked(m_effectType == 4);
  ui->IDC_BRIEF_T_SR2L->setChecked(m_effectType == 5);
  ui->IDC_BRIEF_T_ST2B->setChecked(m_effectType == 6);
  ui->IDC_BRIEF_T_SB2T->setChecked(m_effectType == 7);

  ui->IDC_TABSTOP->setChecked(m_desc.caps.tabstop);
  ui->IDC_BRIEF_COLOR_R->setText(QString::number((m_desc.color >> 16) & 0xff));
  ui->IDC_BRIEF_COLOR_G->setText(QString::number((m_desc.color >> 8) & 0xff));
  ui->IDC_BRIEF_COLOR_B->setText(QString::number(m_desc.color & 0xff));

  if (auto *combo = ui->IDC_BRIEF_T_FONT) {
    combo->addItem("sm_brief");
    combo->addItem("lg_brief");
    combo->setCurrentIndex(m_desc.font == BRIEF_FONT_INDEX ? 0 : 1);
  }

  // The .ui has a QLabel placeholder (IDC_RICHFRAME); overlay a text editor on
  // top of it, mirroring the MFC CRichEditCtrl placement.
  if (auto *frame = ui->IDC_RICHFRAME) {
    const QRect rect = QRect(frame->mapTo(this, QPoint(0, 0)), frame->size()).adjusted(2, 2, -2, -2);
    m_richEdit = new QPlainTextEdit(this);
    m_richEdit->setObjectName("IDC_SCRIPTVIEW");
    m_richEdit->setGeometry(rect);
    m_richEdit->setFont(QFont("Courier"));
    m_richEdit->setTabStopDistance(360 * QFontMetrics(m_richEdit->font()).horizontalAdvance(' '));
    m_richEdit->setPlainText(m_text);
    m_richEdit->show();
    frame->hide();
  }

  if (auto *combo = ui->IDC_BRIEF_T_PREDEF) {
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &BriefTextEditDialog::onPredefChanged);
    populatePredefs();
  }
  if (auto *btn = ui->IDC_BRIEF_COLOR_PAL)
    connect(btn, &QPushButton::clicked, this, &BriefTextEditDialog::onColorPal);
  if (auto *btn = ui->IDC_MISSIONFLAGS)
    connect(btn, &QPushButton::clicked, this, &BriefTextEditDialog::onMissionFlags);

  connect(this, &QDialog::accepted, this, &BriefTextEditDialog::onOk);
}

BriefTextEditDialog::~BriefTextEditDialog() { delete ui; }

void BriefTextEditDialog::populatePredefs() {
  if (auto *combo = ui->IDC_BRIEF_T_PREDEF) {
    combo->clear();
    combo->addItem("<Raw>");
    int layout = -1;
    if (!PBlayouts.empty()) {
      for (size_t i = 0; i < PBlayouts.size(); i++) {
        if (Briefing_screens[m_screen].layout == PBlayouts[i].filename)
          layout = (int)i;
      }
      if (layout != -1) {
        for (int j = 0; j < PBlayouts[layout].num_texts; j++) {
          const auto &t = PBlayouts[layout].texts[j];
          combo->addItem(
              QString("(%1,%2)->(%3,%4)").arg(t.lx).arg(t.ty).arg(t.rx).arg(t.by));
        }
      }
    }
    combo->setCurrentIndex(0);
  }
}

void BriefTextEditDialog::onPredefChanged(int index) {
  auto *combo = ui->IDC_BRIEF_T_PREDEF;
  if (!combo || index <= 0)
    return;
  int layout = -1;
  if (PBlayouts.empty())
    return;
  for (size_t i = 0; i < PBlayouts.size(); i++) {
    if (Briefing_screens[m_screen].layout == PBlayouts[i].filename)
      layout = (int)i;
  }
  if (layout != -1) {
    ui->IDC_BRIEF_T_UL_X->setText(QString::number(PBlayouts[layout].texts[index - 1].lx));
    ui->IDC_BRIEF_T_UL_Y->setText(QString::number(PBlayouts[layout].texts[index - 1].ty));
    ui->IDC_BRIEF_T_LR_X->setText(QString::number(PBlayouts[layout].texts[index - 1].rx));
    ui->IDC_BRIEF_T_LR_Y->setText(QString::number(PBlayouts[layout].texts[index - 1].by));
  }
}

void BriefTextEditDialog::onColorPal() {
  auto *red = ui->IDC_BRIEF_COLOR_R;
  auto *green = ui->IDC_BRIEF_COLOR_G;
  auto *blue = ui->IDC_BRIEF_COLOR_B;
  QColor color(red->text().toInt(), green->text().toInt(), blue->text().toInt());
  const QColor chosen = QColorDialog::getColor(color, this);
  if (chosen.isValid()) {
    red->setText(QString::number(chosen.red()));
    green->setText(QString::number(chosen.green()));
    blue->setText(QString::number(chosen.blue()));
  }
}

void BriefTextEditDialog::onMissionFlags() {
  BriefMissionFlagsDialog dlg(m_desc.mission_mask_set, m_desc.mission_mask_unset, this);
  if (dlg.exec() == QDialog::Accepted) {
    m_desc.mission_mask_set = dlg.setFlags();
    m_desc.mission_mask_unset = dlg.unsetFlags();
  }
}

void BriefTextEditDialog::onOk() {
  m_desc.caps.font = true;
  m_desc.caps.color = true;
  m_desc.caps.speed = true;
  m_desc.caps.looping = true;
  m_desc.caps.waittime = true;
  m_desc.caps.textbox = true;
  m_desc.caps.scroll = true;
  m_desc.textbox.bottom = ui->IDC_BRIEF_T_LR_Y->text().toInt();
  m_desc.textbox.right = ui->IDC_BRIEF_T_LR_X->text().toInt();
  m_desc.textbox.top = ui->IDC_BRIEF_T_UL_Y->text().toInt();
  m_desc.textbox.left = ui->IDC_BRIEF_T_UL_X->text().toInt();
  m_desc.speed = ui->IDC_BRIEF_T_SPEED->text().toFloat();
  m_desc.waittime = ui->IDC_BRIEF_T_STARTTIME->text().toFloat();
  m_desc.color = GR_RGB(ui->IDC_BRIEF_COLOR_R->text().toInt(),
                        ui->IDC_BRIEF_COLOR_G->text().toInt(),
                        ui->IDC_BRIEF_COLOR_B->text().toInt());
  if (ui->IDC_TABSTOP->isChecked())
    m_desc.caps.tabstop = true;

  auto *combo = ui->IDC_BRIEF_T_FONT;
  m_desc.font = (combo && combo->currentIndex() == 1) ? BBRIEF_FONT_INDEX : BRIEF_FONT_INDEX;

  int effectType = 0;
  if (ui->IDC_BRIEF_T_STATIC->isChecked())
    effectType = 0;
  else if (ui->IDC_BRIEF_T_FADEIN->isChecked())
    effectType = 2;
  else if (ui->IDC_BRIEF_T_FADEOUT->isChecked())
    effectType = 3;
  else if (ui->IDC_BRIEF_T_SL2R->isChecked())
    effectType = 4;
  else if (ui->IDC_BRIEF_T_SR2L->isChecked())
    effectType = 5;
  else if (ui->IDC_BRIEF_T_ST2B->isChecked())
    effectType = 6;
  else if (ui->IDC_BRIEF_T_SB2T->isChecked())
    effectType = 7;
  radioToEffectType(effectType, &m_desc);

  if (m_richEdit)
    m_text = m_richEdit->toPlainText();
  accept();
}

