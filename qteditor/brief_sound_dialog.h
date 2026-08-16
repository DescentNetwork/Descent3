#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the briefing element dialog (IDD_BRIEF_ADDSOUND).
class BriefSoundDialog : public Dialog {
  Q_OBJECT
public:
  explicit BriefSoundDialog(QWidget *parent = nullptr);
  ~BriefSoundDialog() override;
};
}
