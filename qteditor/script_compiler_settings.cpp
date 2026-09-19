#include "script_compiler_settings.h"

#include <QSettings>

namespace {
const int kDefaultWarning = 3; // Highest
const int kDefaultDebug = 2;   // C7
} // namespace

namespace ScriptCompilerSettings {

void load(QSettings &s, QString &compilerPath, int &warningLevel, int &debugType) {
  s.beginGroup(QStringLiteral("editor"));
  compilerPath = s.value(QStringLiteral("EditorCompiler")).toString();
  warningLevel = s.value(QStringLiteral("EditorVCWarningLevel"), kDefaultWarning).toInt();
  debugType = s.value(QStringLiteral("EditorVCDebugLevel"), kDefaultDebug).toInt();
  s.endGroup();
}

void save(QSettings &s, const QString &compilerPath, int warningLevel, int debugType) {
  s.beginGroup(QStringLiteral("editor"));
  s.setValue(QStringLiteral("EditorCompiler"), compilerPath);
  s.setValue(QStringLiteral("EditorVCWarningLevel"), warningLevel);
  s.setValue(QStringLiteral("EditorVCDebugLevel"), debugType);
  s.endGroup();
  s.sync();
}

} // namespace ScriptCompilerSettings
