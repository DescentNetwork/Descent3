#include "anim_states_dialog.h"
#include "ui_animstates.h"
AnimStatesDialog::AnimStatesDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AnimStatesDialog)
{
  ui->setupUi(this);
}
AnimStatesDialog::~AnimStatesDialog() { delete ui; }
