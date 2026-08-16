#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the Dallas dialog (IDD_DALLAS_TEXTURE_DIALOG).
class DallasTextureDialog : public Dialog {
  Q_OBJECT
public:
  explicit DallasTextureDialog(QWidget *parent = nullptr);
  ~DallasTextureDialog() override;
};
}
