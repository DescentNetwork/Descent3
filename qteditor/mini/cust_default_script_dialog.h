#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class CustDefaultScriptDialog; }
QT_END_NAMESPACE

// Port of the custdefscript dialog.
class CustDefaultScriptDialog : public QDialog {
  Q_OBJECT
public:
  explicit CustDefaultScriptDialog(QWidget *parent = nullptr);
  ~CustDefaultScriptDialog();
private:
  Ui::CustDefaultScriptDialog *ui;
};
