#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the levelkeypad_layout dialog.
class LevelKeypadLayout : public Dialog {
  Q_OBJECT
public:
  explicit LevelKeypadLayout(QWidget *parent = nullptr);
  ~LevelKeypadLayout() override;
};
}
