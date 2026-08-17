#include "brief_bitmap_dialog.h"
#include "ui_brief_addbitmap.h"
BriefBitmapDialog::BriefBitmapDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::BriefBitmapDialog)
{
  ui->setupUi(this);
}
BriefBitmapDialog::~BriefBitmapDialog() { delete ui; }
