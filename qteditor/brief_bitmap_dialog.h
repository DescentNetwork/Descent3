#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the briefing element dialog (IDD_BRIEF_ADDBITMAP).
class BriefBitmapDialog : public Dialog {
  Q_OBJECT
public:
  explicit BriefBitmapDialog(QWidget *parent = nullptr);
  ~BriefBitmapDialog() override;
};
}
