#include "brief_main_dialog.h"

#include <QLineEdit>

namespace QtEditor {

BriefMainDialog::BriefMainDialog(QWidget *parent) : Dialog(":/ui/brief_main.ui", parent) {
  if (QLineEdit *edit = find<QLineEdit>("IDC_BRIEF_TITLE"))
    edit->setText("Untitled Briefing");
}

BriefMainDialog::~BriefMainDialog() = default;

}
