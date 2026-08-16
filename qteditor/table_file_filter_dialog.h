#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the table_file_filter dialog.
class TableFileFilterDialog : public Dialog {
  Q_OBJECT
public:
  explicit TableFileFilterDialog(QWidget *parent = nullptr);
  ~TableFileFilterDialog() override;
};
}
