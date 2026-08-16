#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the tablefilefilter_addpage_dlg dialog.
class TableFileFilterAddDialog : public Dialog {
  Q_OBJECT
public:
  explicit TableFileFilterAddDialog(QWidget *parent = nullptr);
  ~TableFileFilterAddDialog() override;
};
}
