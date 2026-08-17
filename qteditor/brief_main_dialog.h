#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class BriefMainDialog; }
QT_END_NAMESPACE

// Port of CBriefEdit (IDD_BRIEF_MAIN): the briefing editor's main screen list.
class BriefMainDialog : public QDialog {
  Q_OBJECT
public:
  explicit BriefMainDialog(QWidget *parent = nullptr);
  ~BriefMainDialog();
private:
  Ui::BriefMainDialog *ui;
};
