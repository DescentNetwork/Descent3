#pragma once
#include "qteditor_dialog.h"
namespace QtEditor {
// Port of the Dallas dialog (IDD_DALLAS_STRM_AUDIO_DIALOG).
class DallasStrmAudioDialog : public Dialog {
  Q_OBJECT
public:
  explicit DallasStrmAudioDialog(QWidget *parent = nullptr);
  ~DallasStrmAudioDialog() override;
};
}
