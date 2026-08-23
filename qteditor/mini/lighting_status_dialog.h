#pragma once

#include <QDialog>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class LightingStatusDialog; }
QT_END_NAMESPACE

// Port of CLightingStatus (editor/LightingStatus.cpp).
// Shows a percentage label updated by a 500 ms timer and a Stop button
// that sets rad_DoneCalculating = 1 so the radiosity loop exits at the
// next iteration boundary.
class LightingStatusDialog : public QDialog {
  Q_OBJECT
public:
  explicit LightingStatusDialog(QWidget *parent = nullptr);
  ~LightingStatusDialog();

private slots:
  void onTimer();
  void onStopClicked();

private:
  Ui::LightingStatusDialog *ui;
  QTimer timer_;
};
