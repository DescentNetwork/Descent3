#include "table_file_filter_dialog.h"
#include "ui_table_file_filter.h"
TableFileFilterDialog::TableFileFilterDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::TableFileFilterDialog) {
  ui->setupUi(this);
}
TableFileFilterDialog::~TableFileFilterDialog() { delete ui; }
