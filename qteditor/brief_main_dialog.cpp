#include "brief_main_dialog.h"
#include "ui_brief_main.h"



BriefMainDialog::BriefMainDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::BriefMainDialog)
{
  ui->setupUi(this);
}

BriefMainDialog::~BriefMainDialog() { delete ui; }

