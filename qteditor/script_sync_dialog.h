#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the syncscripts dialog.
class ScriptSyncDialog : public Dialog {
  Q_OBJECT
public:
  explicit ScriptSyncDialog(QWidget *parent = nullptr);
  ~ScriptSyncDialog() override;
};
}
