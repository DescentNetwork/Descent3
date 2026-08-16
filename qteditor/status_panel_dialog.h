#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the status dialog (IDD_STATUS).
class StatusPanelDialog : public Dialog {
  Q_OBJECT
public:
  explicit StatusPanelDialog(QWidget *parent = nullptr);
  ~StatusPanelDialog() override;
};
}
