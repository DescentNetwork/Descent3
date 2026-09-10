#include "config_compiler_dialog.h"
#include "ui_configcompiler.h"

#include "script_compiler_settings.h"

#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>

namespace {
const int kDefaultWarning = 3; // Highest
const int kDefaultDebug = 2;   // C7
} // namespace

ConfigCompilerDialog::ConfigCompilerDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ConfigCompilerDialog)
{
  ui->setupUi(this);

      connect(ui->IDC_BROWSE, &QPushButton::clicked, this, [this]() {
      const QString start = ui->IDC_COMPILER ? ui->IDC_COMPILER->text() : QString();
      const QString path = QFileDialog::getOpenFileName(
          this, tr("Select Script Compiler"), start, tr("Applications (*);;All files (*)"));
      if (!path.isEmpty() && ui->IDC_COMPILER)
        ui->IDC_COMPILER->setText(path);
    });

  connect(ui->IDOK, &QPushButton::clicked, this, &ConfigCompilerDialog::accept);

  QSettings settings;
  QString compiler;
  int warning = kDefaultWarning;
  int debug = kDefaultDebug;
  ScriptCompilerSettings::load(settings, compiler, warning, debug);

  if (ui->IDC_COMPILER)
    ui->IDC_COMPILER->setText(compiler);

  ui->IDC_WARN_NONE->setChecked(warning == 0);
  ui->IDC_WARN_LOW->setChecked(warning == 1);
  ui->IDC_WARN_MED->setChecked(warning == 2);
  ui->IDC_WARN_HIGH->setChecked(warning == 3);
  ui->IDC_RADIO8->setChecked(warning == 4 || warning == kDefaultWarning);

  ui->IDC_OPTIMIZE->setChecked(debug == 0);
  ui->IDC_COFF->setChecked(debug == 1);
  ui->IDC_C7->setChecked(debug == 2 || debug == kDefaultDebug);
}

ConfigCompilerDialog::~ConfigCompilerDialog() { delete ui; }

void ConfigCompilerDialog::accept() {
  int warning = kDefaultWarning;
  if (ui->IDC_WARN_NONE && ui->IDC_WARN_NONE->isChecked())
    warning = 0;
  else if (ui->IDC_WARN_LOW && ui->IDC_WARN_LOW->isChecked())
    warning = 1;
  else if (ui->IDC_WARN_MED && ui->IDC_WARN_MED->isChecked())
    warning = 2;
  else if (ui->IDC_WARN_HIGH && ui->IDC_WARN_HIGH->isChecked())
    warning = 3;
  else if (ui->IDC_RADIO8 && ui->IDC_RADIO8->isChecked())
    warning = 4;

  int debug = kDefaultDebug;
  if (ui->IDC_OPTIMIZE && ui->IDC_OPTIMIZE->isChecked())
    debug = 0;
  else if (ui->IDC_COFF && ui->IDC_COFF->isChecked())
    debug = 1;
  else if (ui->IDC_C7 && ui->IDC_C7->isChecked())
    debug = 2;

  QSettings settings;
  ScriptCompilerSettings::save(settings, ui->IDC_COMPILER ? ui->IDC_COMPILER->text() : QString(), warning, debug);

  QDialog::accepted();
}
