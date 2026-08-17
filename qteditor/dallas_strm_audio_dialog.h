#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class DallasStrmAudioDialog; }
QT_END_NAMESPACE

// Port of the Dallas dialog (IDD_DALLAS_STRM_AUDIO_DIALOG).
class DallasStrmAudioDialog : public QDialog {
  Q_OBJECT
public:
  explicit DallasStrmAudioDialog(QWidget *parent = nullptr);
  ~DallasStrmAudioDialog();
private:
  Ui::DallasStrmAudioDialog *ui;
};
