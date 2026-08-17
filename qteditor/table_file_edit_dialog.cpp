#include "table_file_edit_dialog.h"
#include "ui_table_file_editor.h"
TableFileEditDialog::TableFileEditDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::TableFileEditDialog)
{
  ui->setupUi(this);
}
TableFileEditDialog::~TableFileEditDialog() { delete ui; }
