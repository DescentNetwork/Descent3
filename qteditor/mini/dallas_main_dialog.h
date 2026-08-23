#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class DallasMainDialog; }
QT_END_NAMESPACE

// Port of the dallas_main_dialog dialog.
class DallasMainDialog : public QDialog {
  Q_OBJECT
public:
  explicit DallasMainDialog(QWidget *parent = nullptr);
  ~DallasMainDialog();
private:
  Ui::DallasMainDialog *ui;
};
