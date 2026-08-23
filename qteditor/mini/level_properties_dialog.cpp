#include "level_properties_dialog.h"
#include "ui_levelproperties.h"
LevelPropertiesDialog::LevelPropertiesDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LevelPropertiesDialog)
{
  ui->setupUi(this);
}

LevelPropertiesDialog::~LevelPropertiesDialog() { delete ui; }
