#include "object_tree_dialog.h"
#include "ui_objtreedlg.h"
ObjectTreeDialog::ObjectTreeDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ObjectTreeDialog) {
  ui->setupUi(this);
}
ObjectTreeDialog::~ObjectTreeDialog() { delete ui; }
