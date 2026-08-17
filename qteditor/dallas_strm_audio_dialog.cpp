#include "dallas_strm_audio_dialog.h"
#include "ui_dallas_strm_audio_dialog.h"
DallasStrmAudioDialog::DallasStrmAudioDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::DallasStrmAudioDialog)
{
  ui->setupUi(this);
}
DallasStrmAudioDialog::~DallasStrmAudioDialog() { delete ui; }
