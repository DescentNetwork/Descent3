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

struct soundsource_info_s;

QT_BEGIN_NAMESPACE
namespace Ui { class SoundSourceDialog; }
QT_END_NAMESPACE


// Port of CSoundSourceDialog (IDD_SOUNDSOURCE_DIALOG): picks the sound and
// volume for a sound-source object.
class SoundSourceDialog : public QDialog {
  Q_OBJECT
public:
  explicit SoundSourceDialog(soundsource_info_s *data, QWidget *parent = nullptr);
  ~SoundSourceDialog();

private slots:
  void onSelect();
  void onOk();

private:
  Ui::SoundSourceDialog *ui;
  soundsource_info_s *m_data;
};

