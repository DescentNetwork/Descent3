#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ScriptWizardDialog; }
QT_END_NAMESPACE

// Port of the scriptwiz dialog.
class ScriptWizardDialog : public QDialog {
  Q_OBJECT
public:
  explicit ScriptWizardDialog(QWidget *parent = nullptr);
  ~ScriptWizardDialog();
private:
  Ui::ScriptWizardDialog *ui;
};
