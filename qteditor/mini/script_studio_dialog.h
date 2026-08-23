#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ScriptStudioDialog; }
QT_END_NAMESPACE

// Port of the scriptstudio dialog.
class ScriptStudioDialog : public QDialog {
  Q_OBJECT
public:
  explicit ScriptStudioDialog(QWidget *parent = nullptr);
  ~ScriptStudioDialog();
private:
  Ui::ScriptStudioDialog *ui;
};
