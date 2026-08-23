#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class LevelKeypadLayout; }
QT_END_NAMESPACE

// Port of the levelkeypad_layout dialog.
class LevelKeypadLayout : public QDialog {
  Q_OBJECT
public:
  explicit LevelKeypadLayout(QWidget *parent = nullptr);
  ~LevelKeypadLayout();
private:
  Ui::LevelKeypadLayout *ui;
};
