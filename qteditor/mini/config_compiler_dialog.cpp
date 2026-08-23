#include "config_compiler_dialog.h"
#include "ui_configcompiler.h"
ConfigCompilerDialog::ConfigCompilerDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ConfigCompilerDialog)
{
  ui->setupUi(this);
}
ConfigCompilerDialog::~ConfigCompilerDialog() { delete ui; }
