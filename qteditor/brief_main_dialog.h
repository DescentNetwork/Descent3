#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of CBriefEdit (IDD_BRIEF_MAIN): the briefing editor's main screen list.
class BriefMainDialog : public Dialog {
  Q_OBJECT
public:
  explicit BriefMainDialog(QWidget *parent = nullptr);
  ~BriefMainDialog() override;
};
}
