#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the configcompiler dialog.
class ConfigCompilerDialog : public Dialog {
  Q_OBJECT
public:
  explicit ConfigCompilerDialog(QWidget *parent = nullptr);
  ~ConfigCompilerDialog() override;
};
}
