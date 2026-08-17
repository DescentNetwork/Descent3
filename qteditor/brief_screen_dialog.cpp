#include "brief_screen_dialog.h"
#include "ui_brief_addscreen.h"


BriefScreenDialog::BriefScreenDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::BriefScreenDialog)
{
  ui->setupUi(this);
}

BriefScreenDialog::~BriefScreenDialog() { delete ui; }

