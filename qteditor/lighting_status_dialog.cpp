#include "lighting_status_dialog.h"
#include "ui_lightingstatus.h"

#include <QMessageBox>

#include "radiosity.h"

LightingStatusDialog::LightingStatusDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LightingStatusDialog) {
  ui->setupUi(this);

  connect(ui->IDC_STOP_LIGHTING, &QPushButton::clicked, this, &LightingStatusDialog::onStopClicked);
  connect(&timer_, &QTimer::timeout, this, &LightingStatusDialog::onTimer);

  timer_.start(500);
}

LightingStatusDialog::~LightingStatusDialog() {
  timer_.stop();
  delete ui;
}

void LightingStatusDialog::onTimer() {
  char str[100];
  snprintf(str, sizeof(str), "Percentage complete: %.2f %%", (1.0 - rad_Convergence) * 100.0);
  ui->IDC_LIGHTING_COMPLETE->setText(str);
}

void LightingStatusDialog::onStopClicked() {
  auto reply = QMessageBox::question(this, "Light Question", "Are you sure you wish to stop lighting?",
                                     QMessageBox::Yes | QMessageBox::No);
  if (reply == QMessageBox::No)
    return;

  OutrageMessageBox("Lighting will stop at the next iteration, this might take a minute...");
  rad_DoneCalculating = 1;
}
