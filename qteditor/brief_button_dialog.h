#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the briefing element dialog (IDD_BRIEF_ADDBUTTON).
class BriefButtonDialog : public Dialog {
  Q_OBJECT
public:
  explicit BriefButtonDialog(QWidget *parent = nullptr);
  ~BriefButtonDialog() override;
};
}
