#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the briefing element dialog (IDD_BRIEF_ADDMOVIE).
class BriefMovieDialog : public Dialog {
  Q_OBJECT
public:
  explicit BriefMovieDialog(QWidget *parent = nullptr);
  ~BriefMovieDialog() override;
};
}
