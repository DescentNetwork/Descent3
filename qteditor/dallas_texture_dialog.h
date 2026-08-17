#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class DallasTextureDialog; }
QT_END_NAMESPACE

// Port of the Dallas dialog (IDD_DALLAS_TEXTURE_DIALOG).
class DallasTextureDialog : public QDialog {
  Q_OBJECT
public:
  explicit DallasTextureDialog(QWidget *parent = nullptr);
  ~DallasTextureDialog();
private:
  Ui::DallasTextureDialog *ui;
};
