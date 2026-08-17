#include "dallas_main_dialog.h"
#include "ui_dallas_main_dialog.h"
DallasMainDialog::DallasMainDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::DallasMainDialog)
{
  ui->setupUi(this);
}
DallasMainDialog::~DallasMainDialog() { delete ui; }
