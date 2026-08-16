#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the scriptwiz dialog.
class ScriptWizardDialog : public Dialog {
  Q_OBJECT
public:
  explicit ScriptWizardDialog(QWidget *parent = nullptr);
  ~ScriptWizardDialog() override;
};
}
