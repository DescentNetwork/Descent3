#include "localize_script_dialog.h"
#include "ui_localizescript.h"
LocalizeScriptDialog::LocalizeScriptDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LocalizeScriptDialog)
{
  ui->setupUi(this);
}
LocalizeScriptDialog::~LocalizeScriptDialog() { delete ui; }
