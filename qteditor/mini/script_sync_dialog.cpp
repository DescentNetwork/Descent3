#include "script_sync_dialog.h"
#include "ui_syncscripts.h"
ScriptSyncDialog::ScriptSyncDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ScriptSyncDialog) {
  ui->setupUi(this);
}
ScriptSyncDialog::~ScriptSyncDialog() { delete ui; }
