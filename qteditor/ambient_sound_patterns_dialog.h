#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the ambient sound dialog (IDD_AMBIENTSOUNDPATTERNS).
class AmbientSoundPatternsDialog : public Dialog {
  Q_OBJECT
public:
  explicit AmbientSoundPatternsDialog(QWidget *parent = nullptr);
  ~AmbientSoundPatternsDialog() override;
};
}
