#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class BriefMovieDialog; }
QT_END_NAMESPACE

// Port of the briefing element dialog (IDD_BRIEF_ADDMOVIE).
class BriefMovieDialog : public QDialog {
  Q_OBJECT
public:
  explicit BriefMovieDialog(QWidget *parent = nullptr);
  ~BriefMovieDialog();
private:
  Ui::BriefMovieDialog *ui;
};
