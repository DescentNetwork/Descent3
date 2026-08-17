#include "lighting_status_dialog.h"
#include "ui_lightingstatus.h"
LightingStatusDialog::LightingStatusDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LightingStatusDialog)
{
  ui->setupUi(this);
}

LightingStatusDialog::~LightingStatusDialog() { delete ui; }
