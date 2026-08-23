#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class AISettingsDialog; }
QT_END_NAMESPACE

// Port of the ai dialog.
class AISettingsDialog : public QDialog {
  Q_OBJECT
public:
  explicit AISettingsDialog(QWidget *parent = nullptr);
  ~AISettingsDialog();
private:
  Ui::AISettingsDialog *ui;
};
