#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class TableFileEditDialog; }
QT_END_NAMESPACE

// Port of the table_file_editor dialog.
class TableFileEditDialog : public QDialog {
  Q_OBJECT
public:
  explicit TableFileEditDialog(QWidget *parent = nullptr);
  ~TableFileEditDialog();
private:
  Ui::TableFileEditDialog *ui;
};
