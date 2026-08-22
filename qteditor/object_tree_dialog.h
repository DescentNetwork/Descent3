#pragma once

#include <QDialog>
#include <QTreeWidget>
#include <QTreeWidgetItem>

struct object;

QT_BEGIN_NAMESPACE
namespace Ui { class ObjectTreeDialog; }
QT_END_NAMESPACE

class ObjectTreeDialog : public QDialog {
  Q_OBJECT
public:
  explicit ObjectTreeDialog(QWidget *parent = nullptr);
  ~ObjectTreeDialog();

  void Refresh();
  int GetObjectSelected() const;

private slots:
  void onSelectionChanged();
  void onGoTo();
  void onDelete();
  void onClearAll();

private:
  static QString makeInfoStr(const object *obj);

  Ui::ObjectTreeDialog *ui;
  int m_objectHandle;
};
