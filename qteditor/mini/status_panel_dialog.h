#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class StatusPanelDialog; }
QT_END_NAMESPACE

// Port of the status dialog (IDD_STATUS).
class StatusPanelDialog : public QDialog {
  Q_OBJECT
public:
  explicit StatusPanelDialog(QWidget *parent = nullptr);
  ~StatusPanelDialog();
private:
  Ui::StatusPanelDialog *ui;
};
