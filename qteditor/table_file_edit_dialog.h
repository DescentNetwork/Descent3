#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the table_file_editor dialog.
class TableFileEditDialog : public Dialog {
  Q_OBJECT
public:
  explicit TableFileEditDialog(QWidget *parent = nullptr);
  ~TableFileEditDialog() override;
};
}
