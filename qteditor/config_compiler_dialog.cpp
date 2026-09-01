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

  if (QPushButton *b = ui->IDC_BROWSE) {
    connect(b, &QPushButton::clicked, this, [this]() {
      const QString start = ui->IDC_COMPILER ? ui->IDC_COMPILER->text() : QString();
      const QString path = QFileDialog::getOpenFileName(
          this, tr("Select Script Compiler"), start, tr("Applications (*);;All files (*)"));
      if (!path.isEmpty() && ui->IDC_COMPILER)
        ui->IDC_COMPILER->setText(path);
    });
  }

  if (QPushButton *done = ui->IDOK) {
    connect(done, &QPushButton::clicked, this, &ConfigCompilerDialog::accept);
  }

  QSettings settings;
  QString compiler;
  int warning = kDefaultWarning;
  int debug = kDefaultDebug;
  ScriptCompilerSettings::load(settings, compiler, warning, debug);

  if (ui->IDC_COMPILER)
    ui->IDC_COMPILER->setText(compiler);

  if (QRadioButton *none = ui->IDC_WARN_NONE)
    none->setChecked(warning == 0);
  if (QRadioButton *low = ui->IDC_WARN_LOW)
    low->setChecked(warning == 1);
  if (QRadioButton *med = ui->IDC_WARN_MED)
    med->setChecked(warning == 2);
  if (QRadioButton *high = ui->IDC_WARN_HIGH)
    high->setChecked(warning == 3);
  if (QRadioButton *highest = ui->IDC_RADIO8)
    highest->setChecked(warning == 4 || warning == kDefaultWarning);

  if (QRadioButton *opt = ui->IDC_OPTIMIZE)
    opt->setChecked(debug == 0);
  if (QRadioButton *coff = ui->IDC_COFF)
    coff->setChecked(debug == 1);
  if (QRadioButton *c7 = ui->IDC_C7)
    c7->setChecked(debug == 2 || debug == kDefaultDebug);
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

  QDialog::accept();
}
