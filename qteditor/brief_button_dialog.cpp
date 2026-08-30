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

#include "brief_button_dialog.h"
#include "ui_brief_addbutton.h"

#include <QCheckBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

#include <array>
#include <cstdint>
#include <cstring>
#include <string>

#include "brief_mission_flags_dialog.h"

namespace {

// Button type / click type numeric values (mirror the game's TelComEffects.h,
// which the mini build's TelComEfxStructs.h does not carry).
constexpr uint8_t kButtUpArrow = 0;
constexpr uint8_t kButtDownArrow = 1;
constexpr uint8_t kButtNextPage = 2;
constexpr uint8_t kButtPrevPage = 3;
constexpr uint8_t kButtQuit = 4;
constexpr uint8_t kButtJump = 6;
constexpr uint8_t kClickCursorClickUp = 1;
constexpr uint8_t kClickCursorClickDown = 0;
constexpr uint8_t kClickCursorHold = 2;

void copyToFilename(char *dst, size_t dstLen, const std::string &src) {
  std::strncpy(dst, src.c_str(), dstLen - 1);
  dst[dstLen - 1] = '\0';
}

void clearFilename(char *dst, size_t dstLen) {
  dst[0] = '\0';
  (void)dstLen;
}

// Button-type radios indexed by the win32 radio index (0..5).
static const std::array<const char *, 6> buttonRadios = {
    "IDC_BRIEF_BT_SCROLLUP",   "IDC_BRIEF_BT_SCROLLDOWN", "IDC_BRIEF_BT_NEXTSCREEN",
    "IDC_BRIEF_BT_PREVSCREEN", "IDC_BRIEF_BT_QUIT",       "IDC_BRIEF_BT_JUMP"};
static const std::array<uint8_t, 6> buttonRadioValues = {kButtUpArrow, kButtDownArrow,
                                                          kButtNextPage, kButtPrevPage,
                                                          kButtQuit, kButtJump};

int buttonTypeToRadio(uint8_t buttonType) {
  switch (buttonType) {
  case kButtUpArrow:
    return 0;
  case kButtDownArrow:
    return 1;
  case kButtNextPage:
    return 2;
  case kButtPrevPage:
    return 3;
  case kButtQuit:
    return 4;
  case kButtJump:
    return 5;
  default:
    return 0;
  }
}

// Click-type radios indexed by the win32 radio index (0..2).
static const std::array<const char *, 3> clickRadios = {
    "IDC_BRIEF_BT_MOUSEUP", "IDC_BRIEF_BT_MOUSEDOWN", "IDC_BRIEF_BT_MOUSEHOLD"};

int clickTypeToRadio(uint8_t clickType) {
  switch (clickType) {
  case kClickCursorClickUp:
    return 0;
  case kClickCursorClickDown:
    return 1;
  case kClickCursorHold:
    return 2;
  default:
    return 0;
  }
}

} // namespace

BriefButtonDialog::BriefButtonDialog(TCBUTTONDESC *desc, QWidget *parent)
    : QDialog(parent), ui(new Ui::BriefButtonDialog)
{
  ui->setupUi(this);
  std::memset(&m_desc, 0, sizeof(TCBUTTONDESC));

  if (desc) {
    copyToFilename(m_desc.filename, MAX_FILELEN, desc->filename);
    copyToFilename(m_desc.filename_focus, MAX_FILELEN, desc->filename_focus);
    if (desc->flasher) {
      m_desc.flasher = true;
      m_desc.flash_time = desc->flash_time;
      copyToFilename(m_desc.flash_filename, MAX_FILELEN, desc->flash_filename);
      copyToFilename(m_desc.flash_filename_focus, MAX_FILELEN, desc->flash_filename_focus);
    }
    m_desc.sibling_id = desc->sibling_id;
    m_desc.parent_id = desc->parent_id;
    m_desc.x = desc->x;
    m_desc.y = desc->y;
    m_desc.w = desc->w;
    m_desc.h = desc->h;
    m_desc.osflags = desc->osflags;
    m_desc.button_type = desc->button_type;
    m_desc.click_type = desc->click_type;
    m_desc.mission_mask_set = desc->mission_mask_set;
    m_desc.mission_mask_unset = desc->mission_mask_unset;
    m_desc.jump_page = desc->jump_page;
  } else {
    m_desc.button_type = kButtNextPage;
    m_desc.click_type = kClickCursorClickDown;
  }

  ui->IDC_BRIEF_BT_FILENAME->setText(QString::fromStdString(m_desc.filename));
  ui->IDC_BRIEF_BT_FILENAME_FOCUS->setText(QString::fromStdString(m_desc.filename_focus));
  ui->IDC_BRIEF_BT_FLASH_FILENAME->setText(QString::fromStdString(m_desc.flash_filename));
  ui->IDC_BRIEF_BT_FLASH_FILENAME_FOCUS->setText(
      QString::fromStdString(m_desc.flash_filename_focus));
  ui->IDC_BRIEF_BT_ID->setText("0");
  ui->IDC_BRIEF_BT_PID->setText(QString::number(m_desc.parent_id));
  ui->IDC_BRIEF_BT_SID->setText(QString::number(m_desc.sibling_id));
  ui->IDC_BRIEF_BT_X->setText(QString::number(m_desc.x));
  ui->IDC_BRIEF_BT_Y->setText(QString::number(m_desc.y));
  ui->IDC_BRIEF_BT_JUMP_SCREEN->setText(QString::number(m_desc.jump_page));
  ui->IDC_BRIEF_BT_GLOW->setChecked((m_desc.osflags & OBF_GLOW) != 0);
  ui->IDC_BRIEF_BT_FLASHER->setChecked((m_desc.osflags & OBF_FLASH) != 0);
  ui->IDC_BRIEF_BT_FLASHSTART->setText(
      QString::number((m_desc.osflags & OBF_FLASH) ? m_desc.flash_time : 0));

  int bi = buttonTypeToRadio(m_desc.button_type);
  if (bi >= 0 && bi < (int)buttonRadios.size())
    if (auto *rb = findChild<QRadioButton*>(buttonRadios[bi]))
      rb->setChecked(true);
  int ci = clickTypeToRadio(m_desc.click_type);
  if (ci >= 0 && ci < (int)clickRadios.size())
    if (auto *rb = findChild<QRadioButton*>(clickRadios[ci]))
      rb->setChecked(true);

  if (auto *btn = ui->IDC_BRIEF_BT_CHOOSE)
    connect(btn, &QPushButton::clicked, this, &BriefButtonDialog::onChoose);
  if (auto *btn = ui->IDC_BRIEF_BT_CHOOSE_FOCUS)
    connect(btn, &QPushButton::clicked, this, &BriefButtonDialog::onChooseFocus);
  if (auto *btn = ui->IDC_BRIEF_BT_FLASH_CHOOSE)
    connect(btn, &QPushButton::clicked, this, &BriefButtonDialog::onFlashChoose);
  if (auto *btn = ui->IDC_BRIEF_BT_FLASH_CHOOSE_FOCUS)
    connect(btn, &QPushButton::clicked, this, &BriefButtonDialog::onFlashChooseFocus);
  if (auto *btn = ui->IDC_MISSIONFLAGS)
    connect(btn, &QPushButton::clicked, this, &BriefButtonDialog::onMissionFlags);

  connect(ui->IDC_BRIEF_BT_FLASHER, &QCheckBox::toggled, this, &BriefButtonDialog::onFlasherToggled);
  connect(ui->IDC_BRIEF_BT_GLOW, &QCheckBox::toggled, this, &BriefButtonDialog::onGlowToggled);
  for (const auto &name : buttonRadios) {
    if (auto *rb = findChild<QRadioButton*>(name))
      connect(rb, &QRadioButton::toggled, this, &BriefButtonDialog::updateStates);
  }

  updateStates();

  connect(this, &QDialog::accept, this, &BriefButtonDialog::onOk);
}

BriefButtonDialog::~BriefButtonDialog() { delete ui; }

void BriefButtonDialog::updateStates() {
  const bool flasher = ui->IDC_BRIEF_BT_FLASHER->isChecked();
  const bool glow = ui->IDC_BRIEF_BT_GLOW->isChecked();
  const bool active = flasher || glow;
  ui->IDC_BRIEF_BT_FLASH_FILENAME->setEnabled(active);
  ui->IDC_BRIEF_BT_FLASH_CHOOSE->setEnabled(active);
  ui->IDC_BRIEF_BT_FLASH_FILENAME_FOCUS->setEnabled(active);
  ui->IDC_BRIEF_BT_FLASHSTART->setEnabled(flasher);

  if (auto *jump = findChild<QRadioButton*>("IDC_BRIEF_BT_JUMP"))
    ui->IDC_BRIEF_BT_JUMP_SCREEN->setEnabled(jump->isChecked());
}

void BriefButtonDialog::onFlasherToggled(bool checked) {
  if (checked)
    m_desc.osflags |= OBF_FLASH;
  else
    m_desc.osflags &= ~OBF_FLASH;
  updateStates();
}

void BriefButtonDialog::onGlowToggled(bool checked) {
  if (checked)
    m_desc.osflags |= OBF_GLOW;
  else
    m_desc.osflags &= ~OBF_GLOW;
  updateStates();
}

void BriefButtonDialog::onChoose() {
  const QString file = QFileDialog::getOpenFileName(
      this, tr("Select Bitmap File"), {},
      tr("Outrage Graphic Files (*.ogf);;32bit Uncompressed Targa (*.tga);;PCX (*.pcx);;All Files (*.*)"));
  if (file.isEmpty())
    return;
  ui->IDC_BRIEF_BT_FILENAME->setText(QFileInfo(file).fileName());
}

void BriefButtonDialog::onChooseFocus() {
  const QString file = QFileDialog::getOpenFileName(
      this, tr("Select Bitmap File"), {},
      tr("Outrage Graphic Files (*.ogf);;32bit Uncompressed Targa (*.tga);;PCX (*.pcx);;All Files (*.*)"));
  if (file.isEmpty())
    return;
  ui->IDC_BRIEF_BT_FILENAME_FOCUS->setText(QFileInfo(file).fileName());
}

void BriefButtonDialog::onFlashChoose() {
  const QString file = QFileDialog::getOpenFileName(
      this, tr("Select Bitmap File"), {},
      tr("Outrage Graphic Files (*.ogf);;32bit Uncompressed Targa (*.tga);;PCX (*.pcx);;All Files (*.*)"));
  if (file.isEmpty())
    return;
  ui->IDC_BRIEF_BT_FLASH_FILENAME->setText(QFileInfo(file).fileName());
}

void BriefButtonDialog::onFlashChooseFocus() {
  const QString file = QFileDialog::getOpenFileName(
      this, tr("Select Bitmap File"), {},
      tr("Outrage Graphic Files (*.ogf);;32bit Uncompressed Targa (*.tga);;PCX (*.pcx);;All Files (*.*)"));
  if (file.isEmpty())
    return;
  ui->IDC_BRIEF_BT_FLASH_FILENAME_FOCUS->setText(QFileInfo(file).fileName());
}

void BriefButtonDialog::onMissionFlags() {
  BriefMissionFlagsDialog dlg(m_desc.mission_mask_set, m_desc.mission_mask_unset, this);
  if (dlg.exec() == QDialog::Accepted) {
    m_desc.mission_mask_set = dlg.setFlags();
    m_desc.mission_mask_unset = dlg.unsetFlags();
  }
}

void BriefButtonDialog::onOk() {
  const std::string filename = ui->IDC_BRIEF_BT_FILENAME->text().toStdString();
  const std::string filenameFocus = ui->IDC_BRIEF_BT_FILENAME_FOCUS->text().toStdString();
  const bool flasherChk = ui->IDC_BRIEF_BT_FLASHER->isChecked();
  const bool glowChk = ui->IDC_BRIEF_BT_GLOW->isChecked();
  const std::string flashFilename = ui->IDC_BRIEF_BT_FLASH_FILENAME->text().toStdString();
  const std::string flashFilenameFocus =
      ui->IDC_BRIEF_BT_FLASH_FILENAME_FOCUS->text().toStdString();

  if (!QFile::exists(QString::fromStdString(filename)) ||
      !QFile::exists(QString::fromStdString(filenameFocus))) {
    QMessageBox::warning(this, tr("Error"), tr("Filename Does Not Exist"));
    return;
  }
  if (flasherChk || glowChk) {
    if (!QFile::exists(QString::fromStdString(flashFilename)) ||
        !QFile::exists(QString::fromStdString(flashFilenameFocus))) {
      QMessageBox::warning(this, tr("Error"), tr("Flash Filename Does Not Exist"));
      return;
    }
  }

  m_desc.osflags = 0;
  m_desc.flasher = (flasherChk || glowChk);
  if (flasherChk)
    m_desc.osflags |= OBF_FLASH;
  if (glowChk)
    m_desc.osflags |= OBF_GLOW;

  if (m_desc.flasher) {
    copyToFilename(m_desc.flash_filename, MAX_FILELEN, flashFilename);
    copyToFilename(m_desc.flash_filename_focus, MAX_FILELEN, flashFilenameFocus);
    if (m_desc.osflags & OBF_FLASH)
      m_desc.flash_time = ui->IDC_BRIEF_BT_FLASHSTART->text().toFloat();
  } else {
    clearFilename(m_desc.flash_filename, MAX_FILELEN);
    clearFilename(m_desc.flash_filename_focus, MAX_FILELEN);
  }

  m_desc.parent_id = ui->IDC_BRIEF_BT_PID->text().toInt();
  m_desc.sibling_id = ui->IDC_BRIEF_BT_SID->text().toInt();
  m_desc.x = ui->IDC_BRIEF_BT_X->text().toInt();
  m_desc.y = ui->IDC_BRIEF_BT_Y->text().toInt();
  m_desc.jump_page = ui->IDC_BRIEF_BT_JUMP_SCREEN->text().toInt();

  int bi = 0;
  for (int i = 0; i < (int)buttonRadios.size(); i++) {
    if (auto *rb = findChild<QRadioButton*>(buttonRadios[i]); rb && rb->isChecked()) {
      bi = i;
      break;
    }
  }
  m_desc.button_type = buttonRadioValues[bi];

  int ci = 0;
  for (int i = 0; i < (int)clickRadios.size(); i++) {
    if (auto *rb = findChild<QRadioButton*>(clickRadios[i]); rb && rb->isChecked()) {
      ci = i;
      break;
    }
  }
  m_desc.click_type = (ci == 0) ? kClickCursorClickUp : ((ci == 1) ? kClickCursorClickDown
                                                                   : kClickCursorHold);

  copyToFilename(m_desc.filename, MAX_FILELEN, filename);
  copyToFilename(m_desc.filename_focus, MAX_FILELEN, filenameFocus);
  accept();
}