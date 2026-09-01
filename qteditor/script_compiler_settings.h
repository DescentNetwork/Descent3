#pragma once

#include <QString>

class QSettings;

// Editor virtual-compiler configuration.  Stored under the "editor" group
// using the same key names the Win32 editor used so a migrating user keeps
// their settings (EditorCompiler, EditorVCWarningLevel, EditorVCDebugLevel).
namespace ScriptCompilerSettings {

void load(QSettings &s, QString &compilerPath, int &warningLevel, int &debugType);
void save(QSettings &s, const QString &compilerPath, int warningLevel, int debugType);

} // namespace ScriptCompilerSettings
