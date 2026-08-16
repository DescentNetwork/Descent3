#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the animstates dialog.
class AnimStatesDialog : public Dialog {
  Q_OBJECT
public:
  explicit AnimStatesDialog(QWidget *parent = nullptr);
  ~AnimStatesDialog() override;
};
}
