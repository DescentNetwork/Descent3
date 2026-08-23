#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class BriefSoundDialog; }
QT_END_NAMESPACE

// Port of the briefing element dialog (IDD_BRIEF_ADDSOUND).
class BriefSoundDialog : public QDialog {
  Q_OBJECT
public:
  explicit BriefSoundDialog(QWidget *parent = nullptr);
  ~BriefSoundDialog();
private:
  Ui::BriefSoundDialog *ui;
};
