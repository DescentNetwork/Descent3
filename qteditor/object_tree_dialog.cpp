#include "object_tree_dialog.h"
#include "ui_objtreedlg.h"

#include <QMessageBox>

#include "d3edit.h"
#include "object.h"
#include "object_external_struct.h"
#include "objinfo.h"
#include "object_ops.h"


using namespace std::string_literals;

namespace {
const char *kCategoryNames[] = {
    "Clipboard", "Powerups", "Robots", "Buildings", "Door",
    "Clutter", "Players", "Cameras", "Sound sources", "Miscellaneous"};

int categoryForType(object_type_e type) {
  switch (type) {
  case object_type_e::powerup:
    return 1;
  case object_type_e::robot:
    return 2;
  case object_type_e::building:
    return 3;
  case object_type_e::clutter:
    return 5;
  case object_type_e::door:
    return 4;
  case object_type_e::player:
    return 6;
  case object_type_e::camera:
    return 7;
  case object_type_e::soundsource:
    return 8;
  case object_type_e::viewer:
    return -1;
  default:
    return 9;
  }
}
} // namespace

ObjectTreeDialog::ObjectTreeDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ObjectTreeDialog), m_objectHandle(-1) {
  ui->setupUi(this);

  ui->IDC_GOTO->setEnabled(false);
  ui->IDC_DELETE->setEnabled(false);

  connect(ui->IDC_OBJTREE, &QTreeWidget::currentItemChanged, this,
          [this]() { onSelectionChanged(); });
  connect(ui->IDC_GOTO, &QPushButton::clicked, this, &ObjectTreeDialog::onGoTo);
  connect(ui->IDC_DELETE, &QPushButton::clicked, this, &ObjectTreeDialog::onDelete);
  connect(ui->IDC_CLEAR, &QPushButton::clicked, this, &ObjectTreeDialog::onClearAll);

  if (auto *okBtn = findChild<QPushButton *>(QStringLiteral("IDOK")))
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
  if (auto *cancelBtn = findChild<QPushButton *>(QStringLiteral("IDCANCEL")))
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

  Refresh();
}

ObjectTreeDialog::~ObjectTreeDialog() { delete ui; }

QString ObjectTreeDialog::makeInfoStr(const object *obj) {
  QString str;
  if (obj->type == object_type_e::powerup || obj->type == object_type_e::robot || obj->type == object_type_e::building ||
      obj->type == object_type_e::clutter) {
    str = QString("%1-(0x%2)[%3](%4) ")
              .arg(static_cast<int>(OBJNUM(obj)))
              .arg(obj->handle, 0, 16)
              .arg(QString::fromStdString(Object_info[obj->id].name))
              .arg(QString::fromStdString(obj->name.empty() ? "No Name Given"s : obj->name));
  } else {
    str = QString("%1-(0x%2)[%3](%4) ")
              .arg(static_cast<int>(OBJNUM(obj)))
              .arg(obj->handle, 0, 16)
              .arg(QString::fromStdString(Object_type_names[obj_type_index(obj->type)]))
              .arg(QString::fromStdString(obj->name.empty() ? "No Name Given"s : obj->name));
  }

  if (obj->is_outside())
    str += QString("cell %1").arg(roomnum_cell(obj->roomnum));
  else
    str += QString("room %1").arg(static_cast<int>(*obj->roomnum));

  return str;
}

void ObjectTreeDialog::Refresh() {
  QTreeWidget *tree = ui->IDC_OBJTREE;
  tree->clear();
  m_objectHandle = -1;

  QTreeWidgetItem *topItems[10];
  for (int i = 0; i < 10; i++)
    topItems[i] = new QTreeWidgetItem(tree, {QString::fromLatin1(kCategoryNames[i])});

  for (int i = 0; i <= Highest_object_index; i++) {
    if (Objects[i].flags.dead)
      continue;

    int cat = categoryForType(Objects[i].type);
    if (cat < 0)
      continue;

    QString info = makeInfoStr(&Objects[i]);
    QTreeWidgetItem *child = new QTreeWidgetItem(topItems[cat], {info});
    child->setData(0, Qt::UserRole, Objects[i].handle);
  }

  tree->expandAll();
}

int ObjectTreeDialog::GetObjectSelected() const { return m_objectHandle; }

void ObjectTreeDialog::onSelectionChanged() {
  QTreeWidgetItem *item = ui->IDC_OBJTREE->currentItem();
  if (!item || !item->parent()) {
    m_objectHandle = -1;
  } else {
    m_objectHandle = item->data(0, Qt::UserRole).toInt();
  }

  bool hasSelection = (m_objectHandle >= 0);
  ui->IDC_GOTO->setEnabled(hasSelection);
  ui->IDC_DELETE->setEnabled(hasSelection);
}

void ObjectTreeDialog::onGoTo() {
  int handle = GetObjectSelected();
  if (handle < 0)
    return;

  object *obj = ObjGet(handle);
  if (!obj)
    return;

  if (obj->is_outside() && app.view_mode != state::viewer::terrain) {
    app.view_mode = state::viewer::terrain;
  } else if (!obj->is_outside() && app.view_mode != state::viewer::mine) {
    app.view_mode = state::viewer::mine;
  }

  ObjSetPos(*Viewer_object, obj->pos, obj->roomnum, obj->orient, false);
  EditorStatus("Viewer moved to object %d", OBJNUM(obj));
  app.Viewer_moved = true;
}

void ObjectTreeDialog::onDelete() {
  int handle = GetObjectSelected();
  if (handle < 0)
    return;

  object *obj = ObjGet(handle);
  if (!obj)
    return;

  app.Cur_object_index = OBJNUM(obj);
  HObjectDelete();
  Refresh();
}

void ObjectTreeDialog::onClearAll()
{
  if(QMessageBox::question(this, "Clear All Objects", "This will delete ALL objects except doors.  Are you sure?") == QMessageBox::Yes)
  {
    for (int i = 0; i <= Highest_object_index; i++) {
      if (&Objects[i] == Player_object)
        continue;
      if (&Objects[i] == Viewer_object)
        continue;
      if (Objects[i].type == object_type_e::door)
        continue;
      if (Objects[i].type != object_type_e::none)
        ObjDelete(i);
    }

    app.World_changed = true;
    app.Cur_object_index.reset();
    Refresh();
  }
}
