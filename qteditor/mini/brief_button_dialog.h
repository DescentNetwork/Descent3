#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class BriefButtonDialog; }
QT_END_NAMESPACE

// Port of the briefing element dialog (IDD_BRIEF_ADDBUTTON).
class BriefButtonDialog : public QDialog {
  Q_OBJECT
public:
  explicit BriefButtonDialog(QWidget *parent = nullptr);
  ~BriefButtonDialog();
private:
  Ui::BriefButtonDialog *ui;
};
