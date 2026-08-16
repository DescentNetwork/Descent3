#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the custdefscript dialog.
class CustDefaultScriptDialog : public Dialog {
  Q_OBJECT
public:
  explicit CustDefaultScriptDialog(QWidget *parent = nullptr);
  ~CustDefaultScriptDialog() override;
};
}
