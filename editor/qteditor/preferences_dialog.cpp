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

#include "preferences_dialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>

#include "d3edit.h"
#include "game.h"
#include "pilot.h"
#include "renderer.h"
#include "slew.h"

extern bool Cinematics_enabled;

namespace QtEditor {

PreferencesDialog::PreferencesDialog(QWidget *parent) : Dialog(":/editor/preferences.ui", parent) {
  if (QRadioButton *rb = find<QRadioButton>("IDC_WINDOWED")) {
    rb->setChecked(D3EditState.game_render_mode == GM_WINDOWED);
    connect(rb, &QRadioButton::clicked, this, &PreferencesDialog::onWindowed);
  }
  if (QRadioButton *rb = find<QRadioButton>("IDC_FULLSCREEN_SW")) {
    rb->setChecked(D3EditState.game_render_mode == GM_FULLSCREEN_SW);
    connect(rb, &QRadioButton::clicked, this, &PreferencesDialog::onFullScreenSW);
  }
  if (QRadioButton *rb = find<QRadioButton>("IDC_FULLSCREEN_HW")) {
    rb->setChecked(D3EditState.game_render_mode == GM_FULLSCREEN_HW);
    connect(rb, &QRadioButton::clicked, this, &PreferencesDialog::onFullScreenHW);
  }
  if (QCheckBox *cb = find<QCheckBox>("IDC_DEBUG_CHECK"))
    cb->setChecked(D3EditState.fullscreen_debug_state);
  if (QRadioButton *rb = find<QRadioButton>("IDC_USE_OPENGL"))
    rb->setChecked(PreferredRenderer == RENDERER_OPENGL);
  if (QRadioButton *rb = find<QRadioButton>("IDC_USE_GLIDE"))
    rb->setChecked(PreferredRenderer == RENDERER_GLIDE);
  if (QRadioButton *rb = find<QRadioButton>("IDC_USE_D3D"))
    rb->setChecked(PreferredRenderer == RENDERER_DIRECT3D);
  if (QCheckBox *cb = find<QCheckBox>("IDC_BILINEAR_CHECK"))
    cb->setChecked(Render_preferred_state.filtering != 0);
  if (QCheckBox *cb = find<QCheckBox>("IDC_MIPPING_CHECK"))
    cb->setChecked(Render_preferred_state.mipping != 0);
  if (QCheckBox *cb = find<QCheckBox>("IDC_JOYENABLE"))
    cb->setChecked(D3EditState.joy_slewing);
  if (QCheckBox *cb = find<QCheckBox>("IDC_IGC_ENABLED")) {
    cb->setChecked(Cinematics_enabled);
    connect(cb, &QCheckBox::toggled, this, &PreferencesDialog::onIgcToggled);
  }

  if (QSlider *slider = find<QSlider>("IDC_SLEWSLIDER")) {
    slider->setRange(0, 3);
    slider->setValue((int)((Slew_key_speed - 0.5) / 0.5));
  }

  if (QComboBox *combo = find<QComboBox>("IDC_DEFAULT_PILOT")) {
    const std::vector<std::string> pilotlist = PltGetPilots();
    for (const auto &pilot : pilotlist)
      combo->addItem(QString::fromStdString(pilot));
    if (!pilotlist.empty()) {
      const int idx = combo->findText(QString::fromStdString(Default_pilot));
      if (idx >= 0)
        combo->setCurrentIndex(idx);
    }
    PltClearList();
  }

  if (QPushButton *ok = find<QPushButton>("IDOK")) {
    disconnect(ok, &QPushButton::clicked, m_dialog, &QDialog::accept);
    connect(ok, &QPushButton::clicked, this, &PreferencesDialog::onOk);
  }

  if (D3EditState.game_render_mode == GM_FULLSCREEN_HW)
    enableHardwareOptions();
  else
    disableHardwareOptions();
}

PreferencesDialog::~PreferencesDialog() = default;

void PreferencesDialog::disableHardwareOptions() {
  const char *names[] = {"IDC_HARDWARE_ACC", "IDC_USE_OPENGL", "IDC_USE_GLIDE", "IDC_USE_D3D",
                         "IDC_BILINEAR_CHECK", "IDC_MIPPING_CHECK"};
  for (const char *name : names)
    find<QWidget>(name)->setEnabled(false);
}

void PreferencesDialog::enableHardwareOptions() {
  const char *names[] = {"IDC_HARDWARE_ACC", "IDC_USE_OPENGL", "IDC_USE_GLIDE", "IDC_USE_D3D",
                         "IDC_BILINEAR_CHECK", "IDC_MIPPING_CHECK"};
  for (const char *name : names)
    find<QWidget>(name)->setEnabled(true);
}

void PreferencesDialog::onWindowed() {
  if (find<QRadioButton>("IDC_WINDOWED")->isChecked())
    disableHardwareOptions();
  else
    enableHardwareOptions();
}

void PreferencesDialog::onFullScreenSW() {
  if (find<QRadioButton>("IDC_FULLSCREEN_SW")->isChecked())
    disableHardwareOptions();
  else
    enableHardwareOptions();
}

void PreferencesDialog::onFullScreenHW() {
  if (find<QRadioButton>("IDC_FULLSCREEN_HW")->isChecked())
    disableHardwareOptions();
  else
    enableHardwareOptions();
}

void PreferencesDialog::onIgcToggled(bool checked) { Cinematics_enabled = checked; }

void PreferencesDialog::onOk() {
  if (find<QRadioButton>("IDC_WINDOWED")->isChecked())
    D3EditState.game_render_mode = GM_WINDOWED;
  else if (find<QRadioButton>("IDC_FULLSCREEN_SW")->isChecked())
    D3EditState.game_render_mode = GM_FULLSCREEN_SW;
  else if (find<QRadioButton>("IDC_FULLSCREEN_HW")->isChecked())
    D3EditState.game_render_mode = GM_FULLSCREEN_HW;

  D3EditState.fullscreen_debug_state = find<QCheckBox>("IDC_DEBUG_CHECK")->isChecked();

  if (find<QRadioButton>("IDC_USE_OPENGL")->isChecked())
    PreferredRenderer = RENDERER_OPENGL;
  else if (find<QRadioButton>("IDC_USE_GLIDE")->isChecked())
    PreferredRenderer = RENDERER_GLIDE;
  else if (find<QRadioButton>("IDC_USE_D3D")->isChecked())
    PreferredRenderer = RENDERER_DIRECT3D;

  const bool joyslew = find<QCheckBox>("IDC_JOYENABLE")->isChecked();
  if (joyslew != D3EditState.joy_slewing) {
    D3EditState.joy_slewing = joyslew;
    SlewControlInit();
  }

  Render_preferred_state.filtering = find<QCheckBox>("IDC_BILINEAR_CHECK")->isChecked() ? 1 : 0;
  Render_preferred_state.mipping = find<QCheckBox>("IDC_MIPPING_CHECK")->isChecked() ? 1 : 0;

  if (QComboBox *combo = find<QComboBox>("IDC_DEFAULT_PILOT")) {
    if (combo->count())
      Default_pilot = combo->currentText().toStdString();
    else
      Default_pilot = " ";
  }

  Slew_key_speed = (find<QSlider>("IDC_SLEWSLIDER")->value() * 0.5f) + 0.5f;

  m_dialog->accept();
}

}
