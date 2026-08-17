#include "table_file_filter_add_dialog.h"
#include "ui_tablefilefilter_addpage_dlg.h"
TableFileFilterAddDialog::TableFileFilterAddDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::TableFileFilterAddDialog)
{
  ui->setupUi(this);
}
TableFileFilterAddDialog::~TableFileFilterAddDialog() { delete ui; }
