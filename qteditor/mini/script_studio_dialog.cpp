#include "script_studio_dialog.h"
#include "ui_scriptstudio.h"
ScriptStudioDialog::ScriptStudioDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ScriptStudioDialog) {
  ui->setupUi(this);
}
ScriptStudioDialog::~ScriptStudioDialog() { delete ui; }
