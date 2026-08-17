#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class DallasImportDialog; }
QT_END_NAMESPACE

// Port of the Dallas dialog (IDD_DALLAS_IMPORT_DIALOG).
class DallasImportDialog : public QDialog {
  Q_OBJECT
public:
  explicit DallasImportDialog(QWidget *parent = nullptr);
  ~DallasImportDialog();
private:
  Ui::DallasImportDialog *ui;
};
