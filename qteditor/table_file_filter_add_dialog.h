#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class TableFileFilterAddDialog; }
QT_END_NAMESPACE

// Port of the tablefilefilter_addpage_dlg dialog.
class TableFileFilterAddDialog : public QDialog {
  Q_OBJECT
public:
  explicit TableFileFilterAddDialog(QWidget *parent = nullptr);
  ~TableFileFilterAddDialog();
private:
  Ui::TableFileFilterAddDialog *ui;
};
