#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the scriptstudio dialog.
class ScriptStudioDialog : public Dialog {
  Q_OBJECT
public:
  explicit ScriptStudioDialog(QWidget *parent = nullptr);
  ~ScriptStudioDialog() override;
};
}
