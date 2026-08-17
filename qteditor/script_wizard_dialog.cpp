#include "script_wizard_dialog.h"
#include "ui_scriptwiz.h"
ScriptWizardDialog::ScriptWizardDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ScriptWizardDialog) {
  ui->setupUi(this);
}
ScriptWizardDialog::~ScriptWizardDialog() { delete ui; }
