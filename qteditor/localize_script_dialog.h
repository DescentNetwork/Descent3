#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the localizescript dialog.
class LocalizeScriptDialog : public Dialog {
  Q_OBJECT
public:
  explicit LocalizeScriptDialog(QWidget *parent = nullptr);
  ~LocalizeScriptDialog() override;
};
}
