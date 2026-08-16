#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the scriptinterface dialog.
class ScriptInterfaceDialog : public Dialog {
  Q_OBJECT
public:
  explicit ScriptInterfaceDialog(QWidget *parent = nullptr);
  ~ScriptInterfaceDialog() override;
};
}
