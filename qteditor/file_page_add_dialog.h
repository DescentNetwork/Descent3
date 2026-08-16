#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the filepageadd dialog.
class FilePageAddDialog : public Dialog {
  Q_OBJECT
public:
  explicit FilePageAddDialog(QWidget *parent = nullptr);
  ~FilePageAddDialog() override;
};
}
