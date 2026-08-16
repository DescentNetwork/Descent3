#include "brief_screen_dialog.h"

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>

namespace QtEditor {

BriefScreenDialog::BriefScreenDialog(QWidget *parent) : Dialog(":/ui/brief_addscreen.ui", parent) {
  // Bind the layout list from the briefing layouts available in the data set.
  if (QComboBox *combo = find<QComboBox>("IDC_BRIEF_ADDS_LAYOUT_LIST"))
    combo->addItem("standard");
}

BriefScreenDialog::~BriefScreenDialog() = default;

}
