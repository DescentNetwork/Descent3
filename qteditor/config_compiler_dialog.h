#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ConfigCompilerDialog; }
QT_END_NAMESPACE

// Port of the configcompiler dialog.
class ConfigCompilerDialog : public QDialog {
  Q_OBJECT
public:
  explicit ConfigCompilerDialog(QWidget *parent = nullptr);
  ~ConfigCompilerDialog();
protected:
  void accept() override;
private:
  Ui::ConfigCompilerDialog *ui;
};
