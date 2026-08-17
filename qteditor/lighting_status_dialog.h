#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class LightingStatusDialog; }
QT_END_NAMESPACE

// Port of the lightingstatus dialog.
class LightingStatusDialog : public QDialog {
  Q_OBJECT
public:
  explicit LightingStatusDialog(QWidget *parent = nullptr);
  ~LightingStatusDialog();
private:
  Ui::LightingStatusDialog *ui;
};
