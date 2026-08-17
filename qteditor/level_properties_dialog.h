#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class LevelPropertiesDialog; }
QT_END_NAMESPACE

// Port of the levelproperties dialog.
class LevelPropertiesDialog : public QDialog {
  Q_OBJECT
public:
  explicit LevelPropertiesDialog(QWidget *parent = nullptr);
  ~LevelPropertiesDialog();
private:
  Ui::LevelPropertiesDialog *ui;
};
