#pragma once

#include <QDialog>
#include <string>

#include "brief_model.h"

QT_BEGIN_NAMESPACE
namespace Ui { class BriefScreenDialog; }
QT_END_NAMESPACE

// Port of CBriefScreenEdit (IDD_BRIEF_ADDS): add/edit a briefing screen
// (layout predef selection + description).
class BriefScreenDialog : public QDialog {
  Q_OBJECT
public:
  explicit BriefScreenDialog(QWidget *parent = nullptr);
  ~BriefScreenDialog();

  std::string layout() const { return m_layout; }
  void setLayout(const std::string &layout);

  uint32_t setFlags() const { return m_set; }
  uint32_t unsetFlags() const { return m_unset; }

private slots:
  void onLayoutChanged(int index);
  void onMissionFlags();

private:
  Ui::BriefScreenDialog *ui;
  std::string m_layout;
  uint32_t m_set = 0, m_unset = 0;
};
