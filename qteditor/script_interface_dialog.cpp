#include "script_interface_dialog.h"
#include "ui_scriptinterface.h"
ScriptInterfaceDialog::ScriptInterfaceDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ScriptInterfaceDialog)
{
  ui->setupUi(this);
}
ScriptInterfaceDialog::~ScriptInterfaceDialog() { delete ui; }
