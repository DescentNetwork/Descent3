#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class AmbientSoundPatternsDialog; }
QT_END_NAMESPACE

// Port of the ambient sound dialog (IDD_AMBIENTSOUNDPATTERNS).
class AmbientSoundPatternsDialog : public QDialog {
  Q_OBJECT
public:
  explicit AmbientSoundPatternsDialog(QWidget *parent = nullptr);
  ~AmbientSoundPatternsDialog();
private:
  Ui::AmbientSoundPatternsDialog *ui;
};
