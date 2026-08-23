#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ScriptInterfaceDialog; }
QT_END_NAMESPACE

// Port of the scriptinterface dialog.
class ScriptInterfaceDialog : public QDialog {
  Q_OBJECT
public:
  explicit ScriptInterfaceDialog(QWidget *parent = nullptr);
  ~ScriptInterfaceDialog();
private:
  Ui::ScriptInterfaceDialog *ui;
};
