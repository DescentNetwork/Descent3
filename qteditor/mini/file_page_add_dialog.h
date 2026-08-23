#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class FilePageAddDialog; }
QT_END_NAMESPACE

// Port of the filepageadd dialog.
class FilePageAddDialog : public QDialog {
  Q_OBJECT
public:
  explicit FilePageAddDialog(QWidget *parent = nullptr);
  ~FilePageAddDialog();
private:
  Ui::FilePageAddDialog *ui;
};
