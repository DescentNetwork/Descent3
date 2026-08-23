#include "brief_sound_dialog.h"
#include "ui_brief_addsound.h"
BriefSoundDialog::BriefSoundDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::BriefSoundDialog)
{
  ui->setupUi(this);
}
BriefSoundDialog::~BriefSoundDialog() { delete ui; }
