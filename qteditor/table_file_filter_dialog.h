#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class TableFileFilterDialog; }
QT_END_NAMESPACE

// Port of the table_file_filter dialog.
class TableFileFilterDialog : public QDialog {
  Q_OBJECT
public:
  explicit TableFileFilterDialog(QWidget *parent = nullptr);
  ~TableFileFilterDialog();
private:
  Ui::TableFileFilterDialog *ui;
};
