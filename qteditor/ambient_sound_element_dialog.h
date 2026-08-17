#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class AmbientSoundElementDialog; }
QT_END_NAMESPACE

// Port of the ambient sound dialog (IDD_AMBIENTSOUNDELEMENT).
class AmbientSoundElementDialog : public QDialog {
  Q_OBJECT
public:
  explicit AmbientSoundElementDialog(QWidget *parent = nullptr);
  ~AmbientSoundElementDialog();
private:
  Ui::AmbientSoundElementDialog *ui;
};
