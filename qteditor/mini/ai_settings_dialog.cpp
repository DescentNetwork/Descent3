#include "ai_settings_dialog.h"
#include "ui_ai.h"

AISettingsDialog::AISettingsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AISettingsDialog)
{
  ui->setupUi(this);
}

AISettingsDialog::~AISettingsDialog() { delete ui; }
