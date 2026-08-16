#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the compileall dialog.
class CompileAllDialog : public Dialog {
  Q_OBJECT
public:
  explicit CompileAllDialog(QWidget *parent = nullptr);
  ~CompileAllDialog() override;
};
}
