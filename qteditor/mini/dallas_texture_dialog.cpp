#include "dallas_texture_dialog.h"
#include "ui_dallas_texture_dialog.h"
DallasTextureDialog::DallasTextureDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::DallasTextureDialog)
{
  ui->setupUi(this);
}
DallasTextureDialog::~DallasTextureDialog() { delete ui; }
