#include "cust_default_script_dialog.h"
#include "ui_custdefscript.h"
CustDefaultScriptDialog::CustDefaultScriptDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::CustDefaultScriptDialog)
{
  ui->setupUi(this);
}
CustDefaultScriptDialog::~CustDefaultScriptDialog() { delete ui; }
