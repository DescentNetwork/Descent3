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

#include <cstdint>
#include <string>

#include "brief_mission_flags_dialog.h"

BriefButtonDialog::BriefButtonDialog(TCBUTTONDESC *desc, QWidget *parent)
    : QDialog(parent), ui(new Ui::BriefButtonDialog)
{
  ui->setupUi(this);
  m_desc = TCBUTTONDESC{};

  if (desc) {
    m_desc.filename = desc->filename;
    m_desc.filename_focus = desc->filename_focus;
    if (desc->flasher) {
      m_desc.flasher = true;
      m_desc.flash_time = desc->flash_time;
      m_desc.flash_filename = desc->flash_filename;
      m_desc.flash_filename_focus = desc->flash_filename_focus;
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
    m_desc.button_type = tc_button_type::next_page;
    m_desc.click_type = tc_click_type::click_down;
  }

  ui->IDC_BRIEF_BT_FILENAME->setText(QString::fromStdString(m_desc.filename));
  ui->IDC_BRIEF_BT_FILENAME_FOCUS->setText(QString::fromStdString(m_desc.filename_focus));
  ui->IDC_BRIEF_BT_FLASH_FILENAME->setText(QString::fromStdString(m_desc.flash_filename));
  ui->IDC_BRIEF_BT_FLASH_FILENAME_FOCUS->setText(QString::fromStdString(m_desc.flash_filename_focus));
  ui->IDC_BRIEF_BT_ID->setText("0");
  ui->IDC_BRIEF_BT_PID->setText(QString::number(m_desc.parent_id));
  ui->IDC_BRIEF_BT_SID->setText(QString::number(m_desc.sibling_id));
  ui->IDC_BRIEF_BT_X->setText(QString::number(m_desc.x));
  ui->IDC_BRIEF_BT_Y->setText(QString::number(m_desc.y));
  ui->IDC_BRIEF_BT_JUMP_SCREEN->setText(QString::number(m_desc.jump_page));
  ui->IDC_BRIEF_BT_GLOW->setChecked(m_desc.osflags.glow);
  ui->IDC_BRIEF_BT_FLASHER->setChecked(m_desc.osflags.flash);
  ui->IDC_BRIEF_BT_FLASHSTART->setText(QString::number(m_desc.osflags.flash ? m_desc.flash_time : 0));

  ui->IDC_BRIEF_BT_SCROLLUP->setChecked(m_desc.button_type == tc_button_type::up_arrow);
  ui->IDC_BRIEF_BT_SCROLLDOWN->setChecked(m_desc.button_type == tc_button_type::down_arrow);
  ui->IDC_BRIEF_BT_NEXTSCREEN->setChecked(m_desc.button_type == tc_button_type::next_page);
  ui->IDC_BRIEF_BT_PREVSCREEN->setChecked(m_desc.button_type == tc_button_type::prev_page);
  ui->IDC_BRIEF_BT_QUIT->setChecked(m_desc.button_type == tc_button_type::quit);
  ui->IDC_BRIEF_BT_JUMP->setChecked(m_desc.button_type == tc_button_type::jump);
  ui->IDC_BRIEF_BT_MOUSEUP->setChecked(m_desc.click_type == tc_click_type::click_up);
  ui->IDC_BRIEF_BT_MOUSEDOWN->setChecked(m_desc.click_type == tc_click_type::click_down);
  ui->IDC_BRIEF_BT_MOUSEHOLD->setChecked(m_desc.click_type == tc_click_type::hold);

  connect(ui->IDC_BRIEF_BT_CHOOSE, &QPushButton::clicked, this, &BriefButtonDialog::onChoose);
  connect(ui->IDC_BRIEF_BT_CHOOSE_FOCUS, &QPushButton::clicked, this, &BriefButtonDialog::onChooseFocus);
  connect(ui->IDC_BRIEF_BT_FLASH_CHOOSE, &QPushButton::clicked, this, &BriefButtonDialog::onFlashChoose);
  connect(ui->IDC_BRIEF_BT_FLASH_CHOOSE_FOCUS, &QPushButton::clicked, this, &BriefButtonDialog::onFlashChooseFocus);
  connect(ui->IDC_MISSIONFLAGS, &QPushButton::clicked, this, &BriefButtonDialog::onMissionFlags);

  connect(ui->IDC_BRIEF_BT_FLASHER, &QCheckBox::toggled, this, &BriefButtonDialog::onFlasherToggled);
  connect(ui->IDC_BRIEF_BT_GLOW, &QCheckBox::toggled, this, &BriefButtonDialog::onGlowToggled);
  connect(ui->IDC_BRIEF_BT_SCROLLUP,&QRadioButton::toggled,this,&BriefButtonDialog::updateStates);
  connect(ui->IDC_BRIEF_BT_SCROLLDOWN,&QRadioButton::toggled,this,&BriefButtonDialog::updateStates);
  connect(ui->IDC_BRIEF_BT_NEXTSCREEN,&QRadioButton::toggled,this,&BriefButtonDialog::updateStates);
  connect(ui->IDC_BRIEF_BT_PREVSCREEN,&QRadioButton::toggled,this,&BriefButtonDialog::updateStates);
  connect(ui->IDC_BRIEF_BT_QUIT,&QRadioButton::toggled,this,&BriefButtonDialog::updateStates);
  connect(ui->IDC_BRIEF_BT_JUMP,&QRadioButton::toggled,this,&BriefButtonDialog::updateStates);

  updateStates();

  connect(ui->IDOK, &QPushButton::clicked, this, &BriefButtonDialog::onOk);
}

BriefButtonDialog::~BriefButtonDialog() { delete ui; }

void BriefButtonDialog::updateStates() {
  const bool active = ui->IDC_BRIEF_BT_FLASHER->isChecked() || ui->IDC_BRIEF_BT_GLOW->isChecked();
  ui->IDC_BRIEF_BT_FLASH_FILENAME->setEnabled(active);
  ui->IDC_BRIEF_BT_FLASH_CHOOSE->setEnabled(active);
  ui->IDC_BRIEF_BT_FLASH_FILENAME_FOCUS->setEnabled(active);
  ui->IDC_BRIEF_BT_FLASHSTART->setEnabled(ui->IDC_BRIEF_BT_FLASHER->isChecked());
  ui->IDC_BRIEF_BT_JUMP_SCREEN->setEnabled(ui->IDC_BRIEF_BT_JUMP->isChecked());
}

void BriefButtonDialog::onFlasherToggled(bool checked) {
  if (checked)
    m_desc.osflags.flash = true;
  else
    m_desc.osflags.flash = false;
  updateStates();
}

void BriefButtonDialog::onGlowToggled(bool checked) {
  if (checked)
    m_desc.osflags.glow = true;
  else
    m_desc.osflags.glow = false;
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
  m_desc.osflags.flash = ui->IDC_BRIEF_BT_FLASHER->isChecked();
  m_desc.osflags.glow = ui->IDC_BRIEF_BT_GLOW->isChecked();
  const std::string flashFilename = ui->IDC_BRIEF_BT_FLASH_FILENAME->text().toStdString();
  const std::string flashFilenameFocus =
      ui->IDC_BRIEF_BT_FLASH_FILENAME_FOCUS->text().toStdString();

  if (!QFile::exists(QString::fromStdString(filename)) ||
      !QFile::exists(QString::fromStdString(filenameFocus))) {
    QMessageBox::warning(this, tr("Error"), tr("Filename Does Not Exist"));
    return;
  }
  if (m_desc.osflags.flash || m_desc.osflags.glow) {
    if (!QFile::exists(QString::fromStdString(flashFilename)) ||
        !QFile::exists(QString::fromStdString(flashFilenameFocus))) {
      QMessageBox::warning(this, tr("Error"), tr("Flash Filename Does Not Exist"));
      return;
    }
  }

  m_desc.osflags = {};
  m_desc.flasher = (m_desc.osflags.flash || m_desc.osflags.glow);

  if (m_desc.flasher) {
    m_desc.flash_filename = flashFilename;
    m_desc.flash_filename_focus = flashFilenameFocus;
    if (m_desc.osflags.flash)
      m_desc.flash_time = ui->IDC_BRIEF_BT_FLASHSTART->text().toFloat();
  } else {
    m_desc.flash_filename.clear();
    m_desc.flash_filename_focus.clear();
  }

  m_desc.parent_id = ui->IDC_BRIEF_BT_PID->text().toInt();
  m_desc.sibling_id = ui->IDC_BRIEF_BT_SID->text().toInt();
  m_desc.x = ui->IDC_BRIEF_BT_X->text().toInt();
  m_desc.y = ui->IDC_BRIEF_BT_Y->text().toInt();
  m_desc.jump_page = ui->IDC_BRIEF_BT_JUMP_SCREEN->text().toInt();

  if (ui->IDC_BRIEF_BT_SCROLLUP->isChecked())
    m_desc.button_type = tc_button_type::up_arrow;
  else if (ui->IDC_BRIEF_BT_SCROLLDOWN->isChecked())
    m_desc.button_type = tc_button_type::down_arrow;
  else if (ui->IDC_BRIEF_BT_NEXTSCREEN->isChecked())
    m_desc.button_type = tc_button_type::next_page;
  else if (ui->IDC_BRIEF_BT_PREVSCREEN->isChecked())
    m_desc.button_type = tc_button_type::prev_page;
  else if (ui->IDC_BRIEF_BT_QUIT->isChecked())
    m_desc.button_type = tc_button_type::quit;
  else if (ui->IDC_BRIEF_BT_JUMP->isChecked())
    m_desc.button_type = tc_button_type::jump;
  else
    m_desc.button_type = tc_button_type::next_page;

  if (ui->IDC_BRIEF_BT_MOUSEUP->isChecked())
    m_desc.click_type = tc_click_type::click_up;
  else if (ui->IDC_BRIEF_BT_MOUSEDOWN->isChecked())
    m_desc.click_type = tc_click_type::click_down;
  else
    m_desc.click_type = tc_click_type::hold;

  m_desc.filename = filename;
  m_desc.filename_focus = filenameFocus;
  accept();
}
