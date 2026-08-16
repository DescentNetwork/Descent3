#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the ai dialog.
class AISettingsDialog : public Dialog {
  Q_OBJECT
public:
  explicit AISettingsDialog(QWidget *parent = nullptr);
  ~AISettingsDialog() override;
};
}
