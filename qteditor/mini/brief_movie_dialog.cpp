#include "brief_movie_dialog.h"
#include "ui_brief_addmovie.h"
BriefMovieDialog::BriefMovieDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::BriefMovieDialog)
{
  ui->setupUi(this);
}
BriefMovieDialog::~BriefMovieDialog() { delete ui; }
