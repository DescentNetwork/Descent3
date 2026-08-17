#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class LocalizeScriptDialog; }
QT_END_NAMESPACE

// Port of the localizescript dialog.
class LocalizeScriptDialog : public QDialog {
  Q_OBJECT
public:
  explicit LocalizeScriptDialog(QWidget *parent = nullptr);
  ~LocalizeScriptDialog();
private:
  Ui::LocalizeScriptDialog *ui;
};
