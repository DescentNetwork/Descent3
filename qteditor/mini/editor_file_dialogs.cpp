/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "editor_file_dialogs.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cstdlib>



#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>


namespace {

// MFC's CFileDialog uses pipe-delimited "<desc>|<pattern>" pairs terminated by
// a "||" at the end. QFileDialog uses ";;"-delimited "desc (pattern)" entries.
// Both spellings are common in editor/ — see WorldTexturesDialog.cpp, etc.
QString mfcFilterToQt(const char *filter) {
  if (filter == nullptr || *filter == '\0')
    return QString();
  // MFC strings are typically ASCII (the Win32 editor never localised them),
  // so Latin-1 round-trips cleanly without QTextCodec.
  QString s = QString::fromLatin1(filter);
  while (s.endsWith(QStringLiteral("||")))
    s.chop(2);
  const QStringList parts = s.split(QLatin1Char('|'));
  QStringList qt;
  for (int i = 0; i + 1 < parts.size(); i += 2) {
    const QString desc = parts[i];
    const QString pat = parts[i + 1];
    if (desc.isEmpty() && pat.isEmpty())
      continue;
    if (desc.isEmpty()) {
      qt << pat;
    } else if (pat.isEmpty()) {
      qt << desc;
    } else if (desc.contains(QLatin1Char('('))) {
      // Description already lists its globs ("*.tga; *.lbm; *.bbm"); tack the
      // MFC pattern on so Qt's filter dropdown matches the Win32 one.
      qt << QStringLiteral("%1 %2").arg(desc).arg(pat);
    } else {
      qt << QStringLiteral("%1 (%2)").arg(desc).arg(pat);
    }
  }
  return qt.join(QStringLiteral(";;"));
}

// Walk up from `parent` looking for any loaded dialog whose objectName matches
// `id_name`. Win32 resource IDs in the QT editor live as objectName strings
// ("IDC_MAT_CUR_STATIC" etc.), so the MFC `dlg->GetDlgItem(id)` lookup reduces
// to findChild<> by name.
QWidget *findByIdName(QWidget *dlg, const QString &id_name) {
  if (dlg == nullptr || id_name.isEmpty())
    return nullptr;
  return dlg->findChild<QWidget *>(id_name);
}

} // namespace

bool openOrSave(bool save, QWidget *parent, const char *filter, char *pathname,
                char *initialdir, int dirlen) {
  if (pathname == nullptr)
    return false;
  const QString qt_filter = mfcFilterToQt(filter);
  const QString start_dir =
      (initialdir != nullptr && initialdir[0] != '\0')
          ? QString::fromLatin1(initialdir)
          : QString();

  const QString picked = save
                             ? QFileDialog::getSaveFileName(parent, QString(), start_dir,
                                                            qt_filter, nullptr)
                             : QFileDialog::getOpenFileName(parent, QString(), start_dir,
                                                            qt_filter, nullptr);
  if (picked.isEmpty()) {
    // User cancelled: Win32 leaves `pathname` unchanged, and so does this port.
    if (initialdir != nullptr && dirlen > 0) {
      // Update initialdir to the directory QFileDialog last visited, so the
      // next call starts there. Use the original start dir if it was non-empty.
      const QByteArray dir_bytes = start_dir.toLatin1();
      const int n = qMin(dirlen - 1, dir_bytes.size());
      if (n > 0) {
        std::memcpy(initialdir, dir_bytes.constData(), n);
        initialdir[n] = '\0';
      }
    }
    return false;
  }

  const QByteArray path_bytes = picked.toLatin1();
  const int n = qMin(int {PATH_MAX - 1}, path_bytes.size());
  std::memcpy(pathname, path_bytes.constData(), n);
  pathname[n] = '\0';

  if (initialdir != nullptr && dirlen > 0) {
    const QString new_dir = QFileInfo(picked).absolutePath();
    const QByteArray dir_bytes = new_dir.toLatin1();
    const int m = qMin(dirlen - 1, dir_bytes.size());
    if (m > 0) {
      std::memcpy(initialdir, dir_bytes.constData(), m);
      initialdir[m] = '\0';
    }
  }
  return true;
}

bool OpenFileDialog(QWidget *parent, const char *filter, char *pathname,
                    char *initialdir, int dirlen) {
  return openOrSave(false, parent, filter, pathname, initialdir, dirlen);
}

bool SaveFileDialog(QWidget *parent, const char *filter, char *pathname,
                    char *initialdir, int dirlen) {
  return openOrSave(true, parent, filter, pathname, initialdir, dirlen);
}

void PrintToDlgItem(QWidget *dlg, const char *id_name, const char *fmt, ...) {
  if (dlg == nullptr || id_name == nullptr)
    return;
  QWidget *w = findByIdName(dlg, QString::fromLatin1(id_name));
  if (w == nullptr)
    return;
  char buf[4096];
  std::va_list arg;
  va_start(arg, fmt);
  const int n = std::vsnprintf(buf, sizeof(buf), fmt, arg);
  va_end(arg);
  if (n < 0)
    return;
  const QString text = QString::fromLatin1(buf, qMin(n, int {sizeof(buf) - 1}));
  if (auto *label = qobject_cast<QLabel *>(w))
    label->setText(text);
  else if (auto *edit = qobject_cast<QLineEdit *>(w))
    edit->setText(text);
  else
    w->setProperty("text", text);
}

