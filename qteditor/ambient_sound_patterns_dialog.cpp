#include "ambient_sound_patterns_dialog.h"
#include "ui_ambientsoundpatterns.h"
AmbientSoundPatternsDialog::AmbientSoundPatternsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AmbientSoundPatternsDialog)
{
  ui->setupUi(this);
}
AmbientSoundPatternsDialog::~AmbientSoundPatternsDialog() { delete ui; }
