#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class BriefBitmapDialog; }
QT_END_NAMESPACE

// Port of the briefing element dialog (IDD_BRIEF_ADDBITMAP).
class BriefBitmapDialog : public QDialog {
  Q_OBJECT
public:
  explicit BriefBitmapDialog(QWidget *parent = nullptr);
  ~BriefBitmapDialog();
private:
  Ui::BriefBitmapDialog *ui;
};
