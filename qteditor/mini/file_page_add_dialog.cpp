#include "file_page_add_dialog.h"
#include "ui_filepageadd.h"
FilePageAddDialog::FilePageAddDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::FilePageAddDialog)
{
  ui->setupUi(this);
}
FilePageAddDialog::~FilePageAddDialog() { delete ui; }
