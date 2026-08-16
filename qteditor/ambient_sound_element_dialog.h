#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the ambient sound dialog (IDD_AMBIENTSOUNDELEMENT).
class AmbientSoundElementDialog : public Dialog {
  Q_OBJECT
public:
  explicit AmbientSoundElementDialog(QWidget *parent = nullptr);
  ~AmbientSoundElementDialog() override;
};
}
