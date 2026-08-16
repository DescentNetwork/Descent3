#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the Dallas dialog (IDD_DALLAS_IMPORT_DIALOG).
class DallasImportDialog : public Dialog {
  Q_OBJECT
public:
  explicit DallasImportDialog(QWidget *parent = nullptr);
  ~DallasImportDialog() override;
};
}
