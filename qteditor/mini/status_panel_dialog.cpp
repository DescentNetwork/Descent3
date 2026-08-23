#include "status_panel_dialog.h"
#include "ui_status.h"
StatusPanelDialog::StatusPanelDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::StatusPanelDialog)
{
  ui->setupUi(this);
}

StatusPanelDialog::~StatusPanelDialog() { delete ui; }
