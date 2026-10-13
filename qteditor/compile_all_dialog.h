#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class CompileAllDialog; }
QT_END_NAMESPACE

class QListWidgetItem;

// Port of the compileall dialog.
class CompileAllDialog : public QDialog {
  Q_OBJECT
public:
  explicit CompileAllDialog(QWidget *parent = nullptr);
  ~CompileAllDialog();
private slots:
  void buildAll();
private:
  Ui::CompileAllDialog *ui;
};
