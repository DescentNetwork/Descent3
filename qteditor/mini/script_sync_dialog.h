#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ScriptSyncDialog; }
QT_END_NAMESPACE

// Port of the syncscripts dialog.
class ScriptSyncDialog : public QDialog {
  Q_OBJECT
public:
  explicit ScriptSyncDialog(QWidget *parent = nullptr);
  ~ScriptSyncDialog();
private:
  Ui::ScriptSyncDialog *ui;
};
