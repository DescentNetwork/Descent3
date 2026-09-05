#include "compile_all_dialog.h"
#include "ui_compileall.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QRadioButton>
#include <QTextEdit>

#include "manage.h"
#include "ScriptCompilerAPI.h"

#include <filesystem>


CompileAllDialog::CompileAllDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::CompileAllDialog)
{
  ui->setupUi(this);

  if (QPushButton *selectAll = ui->IDC_SELECTALL) {
    connect(selectAll, &QPushButton::clicked, this, [this]() {
      if (ui->IDC_LIST)
        ui->IDC_LIST->selectAll();
    });
  }

  if (QPushButton *build = ui->IDC_BUILD) {
    connect(build, &QPushButton::clicked, this, &CompileAllDialog::buildAll);
  }

  if (QPushButton *done = ui->IDOK) {
    connect(done, &QPushButton::clicked, this, &CompileAllDialog::accept);
  }
}

CompileAllDialog::~CompileAllDialog() { delete ui; }

void CompileAllDialog::buildAll() {
  QTextEdit *output = ui->IDC_OUTPUT;
  if (!ui->IDC_LIST)
    return;

  if (output) {
    output->clear();
    output->append("Compiling selected modules...");
  }

  int compiled = 0;
  int failed = 0;
  for (int i = 0; i < ui->IDC_LIST->count(); ++i) {
    QListWidgetItem *item = ui->IDC_LIST->item(i);
    if (!item || !item->isSelected())
      continue;

    const QString module = item->text();
    const std::filesystem::path source = LocalScriptDir / (module.toStdString() + ".cpp");

    tCompilerInfo ci;
    ci.source_filename = source.string();
    ci.script_type = ST_LEVEL;
    ci.callback = [output, module](char *str) {
      if (output)
        output->append(QString::fromUtf8(str));
    };

    const int result = ScriptCompile(&ci);
    if (result == CERR_NOERR) {
      ++compiled;
      if (output)
        output->append(QString("OK: %1").arg(module));
    } else {
      ++failed;
      QString why;
      switch (result) {
      case CERR_SOURCENOEXIST:
        why = "source does not exist";
        break;
      case CERR_NOCOMPILERDEFINED:
        why = "no compiler configured";
        break;
      case CERR_COMPILERMISSING:
        why = "compiler missing";
        break;
      default:
        why = QString("result %1").arg(result);
        break;
      }
      if (output)
        output->append(QString("FAILED: %1 (%2)").arg(module, why));
    }
  }

  if (output)
    output->append(QString("\nDone: %1 compiled, %2 failed.").arg(compiled).arg(failed));
}
