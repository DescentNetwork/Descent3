#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ObjectTreeDialog; }
QT_END_NAMESPACE

// Port of the ambient sound dialog (IDD_OBJTREEDLG).
class ObjectTreeDialog : public QDialog {
  Q_OBJECT
public:
  explicit ObjectTreeDialog(QWidget *parent = nullptr);
  ~ObjectTreeDialog();
private:
  Ui::ObjectTreeDialog *ui;
};
