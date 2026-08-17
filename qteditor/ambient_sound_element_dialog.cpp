#include "ambient_sound_element_dialog.h"
#include "ui_ambientsoundelement.h"
AmbientSoundElementDialog::AmbientSoundElementDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AmbientSoundElementDialog)
{
  ui->setupUi(this);
}
AmbientSoundElementDialog::~AmbientSoundElementDialog() { delete ui; }
