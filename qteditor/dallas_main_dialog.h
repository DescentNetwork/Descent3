#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the dallas_main_dialog dialog.
class DallasMainDialog : public Dialog {
  Q_OBJECT
public:
  explicit DallasMainDialog(QWidget *parent = nullptr);
  ~DallasMainDialog() override;
};
}
