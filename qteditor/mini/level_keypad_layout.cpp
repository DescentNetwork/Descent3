#include "level_keypad_layout.h"
#include "ui_levelkeypad_layout.h"
LevelKeypadLayout::LevelKeypadLayout(QWidget *parent)
    : QDialog(parent), ui(new Ui::LevelKeypadLayout)
{
  ui->setupUi(this);
}

LevelKeypadLayout::~LevelKeypadLayout() { delete ui; }
