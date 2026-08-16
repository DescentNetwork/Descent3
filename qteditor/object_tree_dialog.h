#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the ambient sound dialog (IDD_OBJTREEDLG).
class ObjectTreeDialog : public Dialog {
  Q_OBJECT
public:
  explicit ObjectTreeDialog(QWidget *parent = nullptr);
  ~ObjectTreeDialog() override;
};
}
