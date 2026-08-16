#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the levelproperties dialog.
class LevelPropertiesDialog : public Dialog {
  Q_OBJECT
public:
  explicit LevelPropertiesDialog(QWidget *parent = nullptr);
  ~LevelPropertiesDialog() override;
};
}
