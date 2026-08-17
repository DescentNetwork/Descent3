#include "brief_button_dialog.h"
#include "ui_brief_addbutton.h"
BriefButtonDialog::BriefButtonDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::BriefButtonDialog)
{
  ui->setupUi(this);
}
BriefButtonDialog::~BriefButtonDialog() { delete ui; }
