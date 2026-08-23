#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class AnimStatesDialog; }
QT_END_NAMESPACE

// Port of the animstates dialog.
class AnimStatesDialog : public QDialog {
  Q_OBJECT
public:
  explicit AnimStatesDialog(QWidget *parent = nullptr);
  ~AnimStatesDialog();
private:
  Ui::AnimStatesDialog *ui;
};
