#include "compile_all_dialog.h"
#include "ui_compileall.h"
CompileAllDialog::CompileAllDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::CompileAllDialog)
{
  ui->setupUi(this);
}
CompileAllDialog::~CompileAllDialog() { delete ui; }
