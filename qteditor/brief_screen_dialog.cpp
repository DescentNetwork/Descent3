#include "brief_screen_dialog.h"

#include <QComboBox>
#include <QMessageBox>
#include <QPushButton>

#include "ui_brief_addscreen.h"
#include "brief_mission_flags_dialog.h"

BriefScreenDialog::BriefScreenDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::BriefScreenDialog)
{
  ui->setupUi(this);

  // Populate the layout predef combo from the parsed BriefingLayouts.txt.
  if (auto *combo = ui->IDC_BRIEF_ADDS_LAYOUT_LIST) {
    combo->clear();
    combo->addItem(QStringLiteral("(none)"));
    if (PBlayouts && PBnum_layouts && *PBnum_layouts > 0) {
      for (int i = 0; i < *PBnum_layouts; i++)
        combo->addItem(QString::fromStdString(PBlayouts[i].filename));
    }
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &BriefScreenDialog::onLayoutChanged);
  }
  if (auto *btn = ui->IDC_MISSIONFLAGS)
    connect(btn, &QPushButton::clicked, this, &BriefScreenDialog::onMissionFlags);
}

BriefScreenDialog::~BriefScreenDialog() { delete ui; }

void BriefScreenDialog::setLayout(const std::string &layout) {
  m_layout = layout;
  if (auto *combo = ui->IDC_BRIEF_ADDS_LAYOUT_LIST) {
    for (int i = 0; i < combo->count(); i++) {
      if (combo->itemText(i).toStdString() == layout) {
        combo->setCurrentIndex(i);
        return;
      }
    }
  }
}

void BriefScreenDialog::onLayoutChanged(int index) {
  if (auto *combo = ui->IDC_BRIEF_ADDS_LAYOUT_LIST)
    m_layout = (index > 0) ? combo->itemText(index).toStdString() : std::string();
}

void BriefScreenDialog::onMissionFlags() {
  BriefMissionFlagsDialog dlg(m_set, m_unset, this);
  if (dlg.exec() == QDialog::Accepted) {
    m_set = dlg.setFlags();
    m_unset = dlg.unsetFlags();
  }
}
