#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the lightingstatus dialog.
class LightingStatusDialog : public Dialog {
  Q_OBJECT
public:
  explicit LightingStatusDialog(QWidget *parent = nullptr);
  ~LightingStatusDialog() override;
};
}
