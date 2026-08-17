#include "dallas_import_dialog.h"
#include "ui_dallas_import_dialog.h"
DallasImportDialog::DallasImportDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::DallasImportDialog)
{
  ui->setupUi(this);
}
DallasImportDialog::~DallasImportDialog() { delete ui; }
