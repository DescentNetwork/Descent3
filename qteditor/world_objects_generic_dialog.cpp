#include <QtGlobal>
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

#include "world_objects_generic_dialog.h"
#include "ui_worldobjectsgeneric.h"

#include <QMessageBox>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

#include <cmath>
#include <cstring>

#include <QFileInfo>

#include "bitmap.h"
#include "cfile.h"
#include "d3edit.h"


#include "gametexture.h"
#include "generic_death_dialog.h"
#include "genericpage.h"
#include "manage.h"
#include "mem/mem.h"
#include "objinfo.h"
#include "objinit.h"
#include "object.h"
#include "physics_dialog.h"
#include "polymodel.h"
#include "robotfire.h"
#include "sound_combo.h"
#include "vclip.h"

namespace {
// The object being copied (file-local in the original MFC dialog too).
object_info Copy_object;
bool Copy_object_used = false;
} // namespace

WorldObjectsGenericDialog::WorldObjectsGenericDialog(int objType, int current, QWidget *parent)
    : QDialog(parent), ui(new Ui::WorldObjectsGenericDialog), m_type(objType), m_current(current)
{
  ui->setupUi(this);
  connect(ui->IDC_GENERIC_EDIT_AI, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onEditAI);
  connect(ui->IDC_GENERIC_EDIT_PHYSICS, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onEditPhysics);
  connect(ui->IDC_GENERIC_ADD_NEW, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onAddNew);
  connect(ui->IDC_GENERIC_CHECKED_OUT, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onCheckedOut);
  connect(ui->IDC_GENERIC_CHECKIN, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onCheckIn);
  connect(ui->IDC_GENERIC_DEFINE_ANIMSTATES, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onDefineAnimStates);
  connect(ui->IDC_GENERIC_DELETE, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onDelete);
  connect(ui->IDC_GENERIC_LOCK, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onLock);
  connect(ui->IDC_GENERIC_UNDO_LOCK, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onUndoLock);
  connect(ui->IDC_GENERIC_NEXT, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onNext);
  connect(ui->IDC_GENERIC_PREV, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onPrev);
  connect(ui->IDC_GENERIC_CHANGE_MODEL, &QPushButton::clicked, this, [this]() { onAddNew(); });
  connect(ui->IDC_GENERIC_COPY, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onCopy);
  connect(ui->IDC_GENERIC_PASTE, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onPaste);
  connect(ui->IDC_GENERIC_WEAPON_INFO_BUTTON, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onWeaponInfo);
  connect(ui->IDC_GENERIC_LIGHT_BUTTON, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onLight);
  connect(ui->IDC_DEFAULT_RADIUS_BUTTON, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onDefaultRadius);
  connect(ui->IDC_SELSCRIPT, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onSelScript);
  connect(ui->IDC_COMPILEMODULE, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onCompilemodule);
  connect(ui->IDC_GENERIC_DEATHS, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onGenericDeaths);
  connect(ui->IDC_OVERRIDE, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onOverride);
  connect(ui->IDC_NOLOD, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onNolod);

  connect(ui->IDC_NAME_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &WorldObjectsGenericDialog::onNamePulldownChanged);
  connect(ui->IDC_DEATH_POWERUP1_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &WorldObjectsGenericDialog::onDeathPowerup1Changed);
  connect(ui->IDC_DEATH_POWERUP2_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &WorldObjectsGenericDialog::onDeathPowerup2Changed);
  connect(ui->IDC_GENERIC_EXPLOSION_SOUND_COMBO, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &WorldObjectsGenericDialog::onExplosionSoundChanged);
  connect(ui->IDC_GENERIC_AMBIENT_SOUND_COMBO, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &WorldObjectsGenericDialog::onAmbientSoundChanged);

  connect(ui->IDC_GENERIC_SIZE_EDIT, &QLineEdit::editingFinished, this, [this]() {
    if (m_current == -1)
      return;
    object_info *oi = &Object_info[m_current];
    oi->size = ui->IDC_GENERIC_SIZE_EDIT->text().toFloat();
  });
  connect(ui->IDC_GENERIC_HITPOINT_EDIT, &QLineEdit::editingFinished, this, [this]() {
    if (m_current == -1)
      return;
    object_info *oi = &Object_info[m_current];
    oi->hit_points = ui->IDC_GENERIC_HITPOINT_EDIT->text().toInt();
  });
  connect(ui->IDC_GENERIC_IMPACT_DAMAGE_EDIT, &QLineEdit::editingFinished, this, [this]() {
    if (m_current == -1)
      return;
    object_info *oi = &Object_info[m_current];
    oi->damage = ui->IDC_GENERIC_IMPACT_DAMAGE_EDIT->text().toFloat();
  });
  connect(ui->IDC_GENERIC_IMPACT_SIZE_EDIT, &QLineEdit::editingFinished, this, [this]() {
    if (m_current == -1)
      return;
    object_info *oi = &Object_info[m_current];
    oi->impact_size = ui->IDC_GENERIC_IMPACT_SIZE_EDIT->text().toFloat();
  });
  connect(ui->IDC_GENERIC_IMPACT_TIME_EDIT, &QLineEdit::editingFinished, this, [this]() {
    if (m_current == -1)
      return;
    object_info *oi = &Object_info[m_current];
    oi->impact_time = ui->IDC_GENERIC_IMPACT_TIME_EDIT->text().toFloat();
  });
  connect(ui->IDC_GENERIC_INVEN_ICONNAME, &QLineEdit::editingFinished, this, [this]() {
    if (m_current == -1)
      return;
    onKillfocusInvenIconname();
  });
  connect(ui->IDC_LOD_DISTANCE_EDIT, &QLineEdit::editingFinished, this, [this]() {
    if (m_current == -1)
      return;
    onKillfocusLodDistance();
  });
  connect(ui->IDC_DEATH_POWERUP1_NUM_EDIT, &QLineEdit::editingFinished, this, [this]() {
    if (m_current == -1)
      return;
    object_info *oi = &Object_info[m_current];
    oi->dspew_number[0] = ui->IDC_DEATH_POWERUP1_NUM_EDIT->text().toInt();
  });
  connect(ui->IDC_DEATH_POWERUP1_PERCENT_EDIT, &QLineEdit::editingFinished, this, [this]() {
    if (m_current == -1)
      return;
    object_info *oi = &Object_info[m_current];
    oi->dspew_percent[0] = ui->IDC_DEATH_POWERUP1_PERCENT_EDIT->text().toFloat() / 100.0f;
  });
  connect(ui->IDC_DEATH_POWERUP2_NUM_EDIT, &QLineEdit::editingFinished, this, [this]() {
    if (m_current == -1)
      return;
    object_info *oi = &Object_info[m_current];
    oi->dspew_number[1] = ui->IDC_DEATH_POWERUP2_NUM_EDIT->text().toInt();
  });
  connect(ui->IDC_DEATH_POWERUP2_PERCENT_EDIT, &QLineEdit::editingFinished, this, [this]() {
    if (m_current == -1)
      return;
    object_info *oi = &Object_info[m_current];
    oi->dspew_percent[1] = ui->IDC_DEATH_POWERUP2_PERCENT_EDIT->text().toFloat() / 100.0f;
  });
  connect(ui->IDC_SCRIPTNAME, &QLineEdit::editingFinished, this, [this]() {
    if (m_current == -1)
      return;
    onKillfocusScriptname();
  });
  connect(ui->IDC_SCRIPT_OVERRIDE, &QLineEdit::editingFinished, this, [this]() {
    if (m_current == -1)
      return;
    onKillfocusScriptOverride();
  });
  connect(ui->IDC_RESPAWN_SCALAR_EDIT, &QLineEdit::editingFinished, this, [this]() {
    if (m_current == -1)
      return;
    onKillfocusRespawnScalar();
  });
  connect(ui->IDC_GENERIC_SCORE_EDIT, &QLineEdit::editingFinished, this, [this]() {
    if (m_current == -1)
      return;
    object_info *oi = &Object_info[m_current];
    oi->score = ui->IDC_GENERIC_SCORE_EDIT->text().toInt();
  });
  connect(ui->IDC_GENERIC_AMMO_EDIT, &QLineEdit::editingFinished, this, [this]() {
    if (m_current == -1)
      return;
    object_info *oi = &Object_info[m_current];
    oi->ammo_count = ui->IDC_GENERIC_AMMO_EDIT->text().toInt();
  });

  connect(ui->IDC_GENERIC_USES_AI, &QCheckBox::toggled, this, [this](bool checked) {
    if (m_current == -1)
      return;
    Object_info[m_current].flags.control_ai = checked;
    if (QWidget *w = ui->IDC_GENERIC_EDIT_AI)
      w->setEnabled(checked);
    Mine_changed = true;
  });
  connect(ui->IDC_GENERIC_USES_PHYSICS, &QCheckBox::toggled, this, [this](bool checked) {
    if (m_current == -1)
      return;
    Object_info[m_current].flags.uses_physics = checked;
    if (QWidget *w = ui->IDC_GENERIC_EDIT_PHYSICS)
      w->setEnabled(checked);
    Mine_changed = true;
  });
  connect(ui->IDC_GENERIC_DESTROYABLE, &QCheckBox::toggled, this, [this](bool checked) {
    if (m_current == -1)
      return;
    Object_info[m_current].flags.destroyable = checked;
    if (QWidget *w = ui->IDC_GENERIC_HITPOINT_EDIT)
      w->setEnabled(checked);
    if (QWidget *w = ui->IDC_GENERIC_SCORE_EDIT)
      w->setEnabled(checked);
    Mine_changed = true;
  });
  connect(ui->IDC_GENERIC_AI_SCRIPTED_DEATH, &QCheckBox::toggled, this, [this](bool checked) {
    if (m_current == -1)
      return;
    Object_info[m_current].flags.ai_scripted_death = checked;
    Mine_changed = true;
  });
  connect(ui->IDC_OBJ_CEILING_CHECK, &QCheckBox::toggled, this, [this](bool checked) {
    if (m_current == -1)
      return;
    Object_info[m_current].flags.do_ceiling_check = checked;
    Mine_changed = true;
  });
  connect(ui->IDC_OBJECT_FLY_THROUGH_RENDERED_PORTALS, &QCheckBox::toggled, this, [this](bool checked) {
    if (m_current == -1)
      return;
    Object_info[m_current].flags.ignore_forcefields_and_glass = checked;
    Mine_changed = true;
  });
  connect(ui->IDC_NSC_BUTTON, &QCheckBox::toggled, this, [this](bool checked) {
    if (m_current == -1)
      return;
    Object_info[m_current].flags.no_diff_scale_damage = checked;
    Mine_changed = true;
  });
  connect(ui->IDC_DSMPBD_CHECK, &QCheckBox::toggled, this, [this](bool checked) {
    if (m_current == -1)
      return;
    Object_info[m_current].flags.no_diff_scale_move = checked;
    Mine_changed = true;
  });
  connect(ui->IDC_GENERIC_AMBIENT, &QCheckBox::toggled, this, [this](bool checked) {
    if (m_current == -1)
      return;
    Object_info[m_current].flags.ambient_object = checked;
    Mine_changed = true;
  });
  connect(ui->IDC_GENERIC_INVEN_SELECTABLE, &QCheckBox::toggled, this, [this](bool checked) {
    if (m_current == -1)
      return;
    Object_info[m_current].flags.inven_selectable = checked;
    Mine_changed = true;
  });
  connect(ui->IDC_GENERIC_INVEN_NONUSEABLE, &QCheckBox::toggled, this, [this](bool checked) {
    if (m_current == -1)
      return;
    Object_info[m_current].flags.inven_nonuseable = checked;
    Mine_changed = true;
  });
  connect(ui->IDC_INVEN_NOREMOVE, &QCheckBox::toggled, this, [this](bool checked) {
    if (m_current == -1)
      return;
    Object_info[m_current].flags.inven_noremove = checked;
    Mine_changed = true;
  });
  connect(ui->IDC_INVEN_VISWHENUSED, &QCheckBox::toggled, this, [this](bool checked) {
    if (m_current == -1)
      return;
    Object_info[m_current].flags.inven_viswhenused = checked;
    Mine_changed = true;
  });
  connect(ui->IDC_DEATH_POWERUP_USE2_IF_HAVE1_CHECK, &QCheckBox::toggled, this, &WorldObjectsGenericDialog::onDeathPowerupUse2);
  connect(ui->IDC_GENERIC_DEATH_SPEW_2_IF_ZERO_1, &QCheckBox::toggled, this, &WorldObjectsGenericDialog::onDeathSpew2IfZero1);

  connect(ui->IDC_HIRES_RADIO, &QRadioButton::clicked, this, &WorldObjectsGenericDialog::onHiresRadio);
  connect(ui->IDC_MEDRES_RADIO, &QRadioButton::clicked, this, &WorldObjectsGenericDialog::onMedresRadio);
  connect(ui->IDC_LORES_RADIO, &QRadioButton::clicked, this, &WorldObjectsGenericDialog::onLoresRadio);
  connect(ui->IDC_INVTYPE_GAME, &QRadioButton::clicked, this, &WorldObjectsGenericDialog::onInvtypeGame);
  connect(ui->IDC_INVTYPE_MISSION, &QRadioButton::clicked, this, &WorldObjectsGenericDialog::onInvtypeMission);

  if (QLineEdit *edit = ui->IDC_GENERIC_TYPE_NAME)
    edit->setText(QString::fromStdString(Object_type_names[m_type]));

  m_locked_count = countLockedItems();
  updateDialog();
}

WorldObjectsGenericDialog::~WorldObjectsGenericDialog() {
  if (Copy_object.description) {
    mem_free(Copy_object.description);
    Copy_object.description = nullptr;
  }
  saveGenericsOnClose();
  ObjReInitAll();
}

void WorldObjectsGenericDialog::setCurrent(int id) { m_current = id; }

bool WorldObjectsGenericDialog::isLocked(int n) {
  return n != -1 && mng_FindTrackLock(Object_info[n].name, PAGETYPE_GENERIC) != -1;
}

int WorldObjectsGenericDialog::countLockedItems() {
  int count = 0;
  int first = GetObjectID(m_type);
  if (first == -1)
    return 0;
  int n = first;
  do {
    if (isLocked(n))
      count++;
    n = GetNextObjectID(n);
  } while (n != first);
  return count;
}

void WorldObjectsGenericDialog::enableDisableAll(bool flag) {
  const char *names[] = {"IDC_GENERIC_NEXT",        "IDC_GENERIC_PREV", "IDC_NAME_PULLDOWN",
                         "IDC_GENERIC_SIZE_EDIT",   "IDC_GENERIC_COPY", "IDC_GENERIC_DELETE",
                         "IDC_GENERIC_LOCK",        "IDC_GENERIC_CHECKIN", "IDC_GENERIC_UNDO_LOCK",
                         "IDC_GENERIC_CHANGE_NAME", "IDC_GENERIC_CHANGE_MODEL", "IDC_GENERIC_EDIT_PHYSICS",
                         "IDC_GENERIC_EDIT_AI",     "IDC_GENERIC_DEATHS", "IDC_GENERIC_WEAPON_INFO_BUTTON",
                         "IDC_GENERIC_LIGHT_BUTTON", "IDC_DEFAULT_RADIUS_BUTTON", "IDC_GENERIC_DEFINE_ANIMSTATES"};
  for (const char *name : names)
    if (QWidget *w = findChild<QWidget*>(name))
      w->setEnabled(flag);
}

void WorldObjectsGenericDialog::updateDialog() {
  if (m_current == -1) {
    enableDisableAll(false);
    if (QPushButton *paste = ui->IDC_GENERIC_PASTE)
      paste->setEnabled(Network_up && Copy_object_used);
    if (!Network_up) {
      for (const char *name : {"IDC_GENERIC_LOCK", "IDC_GENERIC_CHECKIN", "IDC_GENERIC_UNDO_LOCK",
                               "IDC_GENERIC_CHECKED_OUT", "IDC_OVERRIDE"}) {
        if (auto *w = findChild<QPushButton*>(name))
          w->setEnabled(false);
      }
    }
    return;
  }

  if (!Network_up) {
    for (const char *name : {"IDC_GENERIC_LOCK", "IDC_GENERIC_CHECKIN", "IDC_GENERIC_UNDO_LOCK",
                             "IDC_GENERIC_CHECKED_OUT", "IDC_OVERRIDE"}) {
      if (auto *w = findChild<QPushButton*>(name))
        w->setEnabled(false);
    }
  }

  enableDisableAll(true);
  object_info *oi = &Object_info[m_current];
  const int n = m_current;

  char txt[256];

  PageInPolymodel(oi->render_handle);
  if (QLineEdit *edit = ui->IDC_GENERIC_MODEL_NAME_EDIT) {
    if (m_lod == 0)
      edit->setText(QString::fromStdString(Poly_models[oi->render_handle].name));
    else if (m_lod == 1)
    {
      if(oi->med_render_handle == -1)
        edit->setText("No model defined");
      else
        edit->setText(QString::fromStdString(Poly_models[oi->med_render_handle].name));
    }
    else
    {
      if(oi->lo_render_handle == -1)
        edit->setText("No model defined");
      else
        edit->setText(QString::fromStdString(Poly_models[oi->lo_render_handle].name));
    }
  }

  if (QLineEdit *edit = ui->IDC_LOD_DISTANCE_EDIT) {
    if (m_lod == 0)
      edit->setText("0");
    else if (m_lod == 1)
      edit->setText(QString::number(oi->med_lod_distance));
    else
      edit->setText(QString::number(oi->lo_lod_distance));
  }

  const int poly_handle =
      m_lod == 0 ? oi->render_handle : (m_lod == 1 ? oi->med_render_handle : oi->lo_render_handle);
  if (QLabel *label = ui->IDC_NUM_POLYS) {
    if (poly_handle == -1)
      label->setText("Num polys: NA");
    else {
      PageInPolymodel(poly_handle);
      label->setText(QString("Num polys: %1").arg(CountFacesInPolymodel(&Poly_models[poly_handle])));
    }
  }
  if (QLabel *label = ui->IDC_TEXTURE_RAM_TEXT) {
    if (poly_handle == -1)
      label->setText("Texture RAM: NA");
    else {
      int tram = 0;
      poly_model *pm = &Poly_models[poly_handle];
      uint8_t already_used[100];
      memset(already_used, 0, sizeof(already_used));
      for (int i = 0; i < pm->n_models; i++) {
        bsp_info *sm = &pm->submodel[i];
        for (int t = 0; t < sm->num_faces; t++) {
          polyface *fp = &sm->faces[t];
          if (fp->texnum != -1 && !already_used[fp->texnum]) {
            already_used[fp->texnum] = 1;
            const int texnum = pm->textures[fp->texnum];
            texture *texp = &GameTextures[texnum];
            if (texnum == 0)
              continue;
            if (texp->used) {
              int mem_this_tex;
              if (texp->flags.animated) {
                vclip *vc = &GameVClips[texp->bm_handle];
                mem_this_tex = 0;
                for (int j = 0; j < vc->num_frames; j++) {
                  const int bm_handle = vc->frames[j];
                  int m = bm_w(bm_handle, 0) * bm_h(bm_handle, 0);
                  if (bm_mipped(bm_handle))
                    m += m / 3;
                  mem_this_tex += m * 2;
                }
              } else {
                mem_this_tex = bm_w(texp->bm_handle, 0) * bm_h(texp->bm_handle, 0);
                if (bm_mipped(texp->bm_handle))
                  mem_this_tex += mem_this_tex / 3;
                mem_this_tex *= 2;
              }
              tram += mem_this_tex;
            }
          }
        }
      }
      label->setText(QString("Texture RAM: %1K").arg(tram / 1000));
    }
  }

  if (QLineEdit *edit = ui->IDC_RESPAWN_SCALAR_EDIT)
    edit->setText(QString::number(oi->respawn_scalar));
  if (QLineEdit *edit = ui->IDC_GENERIC_IMPACT_SIZE_EDIT)
    edit->setText(QString::number(oi->impact_size));
  if (QLineEdit *edit = ui->IDC_GENERIC_IMPACT_TIME_EDIT)
    edit->setText(QString::number(oi->impact_time));
  if (QLineEdit *edit = ui->IDC_GENERIC_IMPACT_DAMAGE_EDIT)
    edit->setText(QString::number(oi->damage));
  if (QLineEdit *edit = ui->IDC_DEATH_POWERUP1_NUM_EDIT)
    edit->setText(QString::number(oi->dspew_number[0]));
  if (QLineEdit *edit = ui->IDC_DEATH_POWERUP2_NUM_EDIT)
    edit->setText(QString::number(oi->dspew_number[1]));
  if (QLineEdit *edit = ui->IDC_DEATH_POWERUP1_PERCENT_EDIT)
    edit->setText(QString::number(oi->dspew_percent[0] * 100.0f));
  if (QLineEdit *edit = ui->IDC_DEATH_POWERUP2_PERCENT_EDIT)
    edit->setText(QString::number(oi->dspew_percent[1] * 100.0f));

  if (QCheckBox *cb = ui->IDC_GENERIC_USES_PHYSICS)
    cb->setChecked(oi->flags.uses_physics);
  if (QWidget *w = ui->IDC_GENERIC_EDIT_PHYSICS)
    w->setEnabled(oi->flags.uses_physics);
  if (QCheckBox *cb = ui->IDC_GENERIC_USES_AI)
    cb->setChecked(oi->flags.control_ai);
  if (QWidget *w = ui->IDC_GENERIC_EDIT_AI)
    w->setEnabled(oi->flags.control_ai);

  if (QCheckBox *cb = ui->IDC_DEATH_POWERUP_USE2_IF_HAVE1_CHECK)
    cb->setChecked(oi->f_dspew & DSF_ONLY_IF_PLAYER_HAS_OBJ_1);
  if (QCheckBox *cb = ui->IDC_GENERIC_DEATH_SPEW_2_IF_ZERO_1)
    cb->setChecked(oi->f_dspew & DSF_ONLY_IF_NO_1);

  setSoundComboSelected(ui->IDC_GENERIC_EXPLOSION_SOUND_COMBO, oi->sounds[GSI_EXPLODE]);
  setSoundComboSelected(ui->IDC_GENERIC_AMBIENT_SOUND_COMBO, oi->sounds[GSI_AMBIENT]);

  if (QComboBox *combo = ui->IDC_DEATH_POWERUP1_PULLDOWN) {
    QSignalBlocker blocker(combo);
    combo->clear();
    combo->addItem("<none>", -1);
    for (int i = 0; i < MAX_OBJECT_IDS; i++)
      if (Object_info[i].type != OBJ_NONE)
        combo->addItem(QString::fromStdString(Object_info[i].name), i);
    const int sp1 = oi->dspew[0];
    combo->setCurrentIndex(combo->findData(sp1 >= 0 && sp1 < MAX_OBJECT_IDS ? sp1 : -1));
  }
  if (QComboBox *combo = ui->IDC_DEATH_POWERUP2_PULLDOWN) {
    QSignalBlocker blocker(combo);
    combo->clear();
    combo->addItem("<none>", -1);
    for (int i = 0; i < MAX_OBJECT_IDS; i++)
      if (Object_info[i].type != OBJ_NONE)
        combo->addItem(QString::fromStdString(Object_info[i].name), i);
    const int sp2 = oi->dspew[1];
    combo->setCurrentIndex(combo->findData(sp2 >= 0 && sp2 < MAX_OBJECT_IDS ? sp2 : -1));
  }

  if (QWidget *w = ui->IDC_GENERIC_WEAPON_INFO_BUTTON)
    w->setEnabled(Poly_models[oi->render_handle].num_wbs > 0);

  if (QLineEdit *edit = ui->IDC_GENERIC_SIZE_EDIT)
    edit->setText(QString::number(oi->size, 'f', 2));

  if (QCheckBox *cb = ui->IDC_GENERIC_DESTROYABLE)
    cb->setChecked(oi->flags.destroyable);
  if (QCheckBox *cb = ui->IDC_GENERIC_AI_SCRIPTED_DEATH)
    cb->setChecked(oi->flags.ai_scripted_death);
  if (QCheckBox *cb = ui->IDC_OBJ_CEILING_CHECK)
    cb->setChecked(oi->flags.do_ceiling_check);
  if (QCheckBox *cb = ui->IDC_OBJECT_FLY_THROUGH_RENDERED_PORTALS)
    cb->setChecked(oi->flags.ignore_forcefields_and_glass);
  if (QCheckBox *cb = ui->IDC_NSC_BUTTON)
    cb->setChecked(oi->flags.no_diff_scale_damage);
  if (QCheckBox *cb = ui->IDC_DSMPBD_CHECK)
    cb->setChecked(oi->flags.no_diff_scale_move);
  if (QCheckBox *cb = ui->IDC_GENERIC_AMBIENT)
    cb->setChecked(oi->flags.ambient_object);

  const bool destroyable = oi->flags.destroyable;
  if (QLineEdit *edit = ui->IDC_GENERIC_HITPOINT_EDIT) {
    edit->setText(destroyable ? QString::number(oi->hit_points) : "");
    edit->setEnabled(destroyable);
  }
  if (QLineEdit *edit = ui->IDC_GENERIC_SCORE_EDIT) {
    edit->setText(destroyable ? QString::number(oi->score) : "");
    edit->setEnabled(destroyable);
  }
  if (QLineEdit *edit = ui->IDC_GENERIC_AMMO_EDIT)
    edit->setText(QString::number(oi->ammo_count));
  if (QWidget *w = ui->IDC_GENERIC_AMMO_EDIT)
    w->setEnabled(oi->type == OBJ_POWERUP);
  if (QWidget *w = ui->IDC_GENERIC_AMMO_TEXT)
    w->setEnabled(oi->type == OBJ_POWERUP);

  if (QPushButton *checkedOut = ui->IDC_GENERIC_CHECKED_OUT)
    checkedOut->setEnabled(m_locked_count > 0);
  if (QLineEdit *edit = ui->IDC_GENERIC_ID_EDIT)
    edit->setText(QString::number(m_current));

  if (QPushButton *paste = ui->IDC_GENERIC_PASTE)
    paste->setEnabled(Network_up && Copy_object_used);
  if (QPushButton *del = ui->IDC_GENERIC_DELETE)
    del->setEnabled(isLocked(m_current));
  if (QPushButton *lock = ui->IDC_GENERIC_LOCK)
    lock->setEnabled(Network_up && !isLocked(m_current));
  if (QPushButton *checkin = ui->IDC_GENERIC_CHECKIN)
    checkin->setEnabled(Network_up && isLocked(m_current));
  if (QPushButton *undolock = ui->IDC_GENERIC_UNDO_LOCK)
    undolock->setEnabled(Network_up && isLocked(m_current));
  if (QPushButton *addnew = ui->IDC_GENERIC_ADD_NEW)
    addnew->setEnabled(Network_up);

  if (QComboBox *combo = ui->IDC_NAME_PULLDOWN) {
    QSignalBlocker blocker(combo);
    combo->clear();
    const int first = GetObjectID(m_type);
    if (first != -1) {
      int i = first;
      do {
        combo->addItem(QString::fromStdString(Object_info[i].name));
        i = GetNextObjectID(i);
      } while (i != first);
      combo->setCurrentText(QString::fromStdString(Object_info[m_current].name));
    }
  }

  if (QLineEdit *edit = ui->IDC_SCRIPTNAME)
  {
    if(oi->module_name.empty())
      edit->setText("null");
    else
      edit->setText(QString::fromStdString(oi->module_name));
  }
  if (QLineEdit *edit = ui->IDC_SCRIPT_OVERRIDE)
  {
    if(oi->script_name_override.empty())
      edit->setText("null");
    else
      edit->setText(QString::fromStdString(oi->script_name_override));
  }

  if (QCheckBox *cb = ui->IDC_GENERIC_INVEN_SELECTABLE)
    cb->setChecked(oi->flags.inven_selectable);
  if (QCheckBox *cb = ui->IDC_GENERIC_INVEN_NONUSEABLE)
    cb->setChecked(oi->flags.inven_nonuseable);
  if (QCheckBox *cb = ui->IDC_INVEN_NOREMOVE)
    cb->setChecked(oi->flags.inven_noremove);
  if (QCheckBox *cb = ui->IDC_INVEN_VISWHENUSED)
    cb->setChecked(oi->flags.inven_viswhenused);

  const bool mission = oi->flags.inven_type_mission;
  if (QRadioButton *rb = ui->IDC_INVTYPE_MISSION)
    rb->setChecked(mission);
  if (QRadioButton *rb = ui->IDC_INVTYPE_GAME)
    rb->setChecked(!mission);

  if (auto *edit = ui->IDC_GENERIC_INVEN_DESCRIPTION)
  {
    if (oi->description == nullptr || oi->description[0] == '\0')
      edit->setText("<no description>");
    else
      edit->setText(oi->description);
  }
  if (QLineEdit *edit = ui->IDC_GENERIC_INVEN_ICONNAME)
  {
    if(oi->icon_name.empty())
      edit->setText("<no icon>");
    else
      edit->setText(QString::fromStdString(oi->icon_name));
  }

  if (QPushButton *nolod = ui->IDC_NOLOD) {
    if (m_lod == 0)
      nolod->setEnabled(false);
    else if (m_lod == 1)
      nolod->setEnabled(oi->med_render_handle != -1);
    else
      nolod->setEnabled(oi->lo_render_handle != -1);
  }
  if (QRadioButton *rb = ui->IDC_HIRES_RADIO)
    rb->setChecked(m_lod == 0);
  if (QRadioButton *rb = ui->IDC_MEDRES_RADIO)
    rb->setChecked(m_lod == 1);
  if (QRadioButton *rb = ui->IDC_LORES_RADIO)
    rb->setChecked(m_lod == 2);

  (void)txt;
  (void)n;
}

void WorldObjectsGenericDialog::onEditAI() {
  if (m_current == -1)
    return;
  QMessageBox::information(this, "AI Properties", "The AI Properties dialog has not been ported yet.");
}

void WorldObjectsGenericDialog::onEditPhysics() {
  if (m_current == -1)
    return;
  PhysicsDialog dlg(&Object_info[m_current].phys_info, this);
  dlg.exec();
}

void WorldObjectsGenericDialog::onAddNew() {
  if (!Network_up) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Sorry babe, the network is down.  This action is a no-no.\n");
    return;
  }

  QString Current_model_dir; // get from settings
  const QString pathname =
      QFileDialog::getOpenFileName(this, "Select model", Current_model_dir, "Descent III files (*.pof *.oof)");
  if (pathname.isEmpty())
    return;

  QFileInfo fileInfo(pathname);
  const QByteArray pathBytes = pathname.toLocal8Bit();
  const char *fname = fileInfo.baseName().toLocal8Bit().constData();

  std::filesystem::path tmp = ChangePolyModelName(pathname.toStdString());
  if (FindPolyModelName(fname) != -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You must rename your model to something else because there is already a model with that name!");
    return;
  }

  const int img_handle = LoadPolyModel(pathname.toStdString(), 0);
  if (img_handle < 0) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't open that model file.");
    return;
  }

  char cur_name[100];
  snprintf(cur_name, sizeof(cur_name), "%s", fname);
  char *t = strchr(cur_name, '.');
  if (t == nullptr)
    t = cur_name + strlen(cur_name);
  *t = 0;

  // Make sure the name isn't already in use.
  int c = 1;
  while (FindObjectIDName(std::string(cur_name)) != -1)
    snprintf(cur_name, sizeof(cur_name), "%s%d", fname, ++c);
  cur_name[0] = toupper(cur_name[0]);

  if (FindObjectIDName(std::string(cur_name)) != -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "That name is taken, please choose another.");
    return;
  }

  const int object_handle = AllocObjectID(m_type, true, true, true);
  if (object_handle == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot add object: There are no free object slots.");
    return;
  }

  Object_info[object_handle].name = cur_name;
  Object_info[object_handle].render_handle = img_handle;
  ComputeDefaultSize(Object_info[object_handle].type, img_handle, &Object_info[object_handle].size);
  Object_info[object_handle].flags = object_info_flags_t{};
  Object_info[object_handle].flags.destroyable = true;
  Object_info[object_handle].hit_points = 100;
  memset(&Object_info[object_handle].lighting_info, 0, sizeof(light_info));
  Object_info[object_handle].lighting_info.timebits = 0xFFFFFFFF;
  Object_info[object_handle].lighting_info.lighting_render_type =
      (m_type == OBJ_BUILDING) ? LRT_LIGHTMAPS : LRT_GOURAUD;

  std::filesystem::path destname = LocalModelsDir / Poly_models[Object_info[object_handle].render_handle].name;
  std::filesystem::copy(std::filesystem::path(pathname.toStdString()), (destname), std::filesystem::copy_options::overwrite_existing);

  mng_AllocTrackLock(cur_name, PAGETYPE_GENERIC);
  m_current = object_handle;
  RemapStaticIDs();
  updateDialog();
}

void WorldObjectsGenericDialog::onCheckedOut() {
  QString str = QString("User %1 has these objects held locally:\n\n").arg(TableUser);
  int total = 0;
  for (int i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used && GlobalTrackLocks[i].pagetype == PAGETYPE_GENERIC) {
      const int n = FindObjectIDName(GlobalTrackLocks[i].name);
      if (n != -1 && Object_info[n].type == m_type) {
        str += "   ";
        str += QString::fromStdString(GlobalTrackLocks[i].name);
        str += "\n";
        total++;
      }
    }
  }
  if (total != 0)
    QMessageBox::information(this, "Objects", str);
}

void WorldObjectsGenericDialog::onCheckIn() {
  if (m_current == -1)
    return;
  if (!mng_MakeLocker())
    return;

  mngs_Pagelock temp_pl;
  temp_pl.name = Object_info[m_current].name;
  temp_pl.pagetype = PAGETYPE_GENERIC;

  const int r = mng_CheckIfPageOwned(&temp_pl, TableUser.toStdString());
  if (r < 0)
    QMessageBox::critical(this, "Error!", ErrorString);
  else if (r == 0)
    QMessageBox::information(this, "Information", InfoString);
  else {
    temp_pl.holder = "UNLOCKED";
    if (!mng_ReplacePagelock(temp_pl.name, &temp_pl)) {
      QMessageBox::critical(this, "Error!", ErrorString);
      mng_EraseLocker();
      return;
    }
    if (!mng_ReplacePage(Object_info[m_current].name, Object_info[m_current].name, m_current, PAGETYPE_GENERIC, 0)) {
      QMessageBox::critical(this, "Error!", ErrorString);
    } else {
      std::filesystem::path srcname = LocalModelsDir / Poly_models[Object_info[m_current].render_handle].name;
      std::filesystem::path destname = NetModelsDir / Poly_models[Object_info[m_current].render_handle].name;
      std::filesystem::copy((srcname), (destname), std::filesystem::copy_options::overwrite_existing);
      if (Object_info[m_current].med_render_handle != -1) {
        srcname = LocalModelsDir / Poly_models[Object_info[m_current].med_render_handle].name;
        destname = NetModelsDir / Poly_models[Object_info[m_current].med_render_handle].name;
        std::filesystem::copy((srcname), (destname), std::filesystem::copy_options::overwrite_existing);
      }
      if (Object_info[m_current].lo_render_handle != -1) {
        srcname = LocalModelsDir / Poly_models[Object_info[m_current].lo_render_handle].name;
        destname = NetModelsDir / Poly_models[Object_info[m_current].lo_render_handle].name;
        std::filesystem::copy((srcname), (destname), std::filesystem::copy_options::overwrite_existing);
      }
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Object checked in.");
      Q_ASSERT(mng_DeletePage(Object_info[m_current].name, PAGETYPE_GENERIC, 1) == 1);
      mng_EraseLocker();
      const int p = mng_FindTrackLock(Object_info[m_current].name, PAGETYPE_GENERIC);
      Q_ASSERT(p != -1);
      mng_FreeTrackLock(p);
    }
  }
  mng_EraseLocker();
  updateDialog();
}

void WorldObjectsGenericDialog::onDefineAnimStates() {
  if (m_current == -1)
    return;
  QMessageBox::information(this, "Anim States", "The Anim States dialog has not been ported yet.");
}

void WorldObjectsGenericDialog::onDelete() {
  if (m_current == -1)
    return;
  const int tl = mng_FindTrackLock(Object_info[m_current].name, PAGETYPE_GENERIC);
  if (tl == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "This object is not yours to delete.  Lock first.");
    return;
  }
  if (QMessageBox::question(this, "Delete object",
                            QString("Are you sure you want to delete this object? %1").arg(QString::fromStdString(Object_info[m_current].name))) !=
      QMessageBox::Yes)
    return;
  if (!mng_MakeLocker())
    return;

  mngs_Pagelock pl;
  pl.name = Object_info[m_current].name;
  pl.pagetype = PAGETYPE_GENERIC;

  if (mng_CheckIfPageOwned(&pl, TableUser.toStdString()) != 1) {
    mng_FreeTrackLock(tl);
    Q_ASSERT(mng_DeletePage(Object_info[m_current].name, PAGETYPE_GENERIC, 1));
  } else {
    mng_FreeTrackLock(tl);
    mng_DeletePage(Object_info[m_current].name, PAGETYPE_GENERIC, 0);
    mng_DeletePage(Object_info[m_current].name, PAGETYPE_GENERIC, 1);
    mng_DeletePagelock(Object_info[m_current].name, PAGETYPE_GENERIC);
  }

  const int old_current = m_current;
  m_current = GetNextObjectID(m_current);
  if (m_current == old_current)
    m_current = -1;
  FreePolyModel(Object_info[old_current].render_handle);
  if (Object_info[old_current].med_render_handle != -1)
    FreePolyModel(Object_info[old_current].med_render_handle);
  if (Object_info[old_current].lo_render_handle != -1)
    FreePolyModel(Object_info[old_current].lo_render_handle);
  FreeObjectID(old_current);
  mng_EraseLocker();
  QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Object deleted.");
  RemapStaticIDs();
  updateDialog();
}

void WorldObjectsGenericDialog::onLock() {
  if (m_current == -1)
    return;
  if (!mng_MakeLocker())
    return;

  mngs_Pagelock temp_pl;
  mngs_generic_page page;
  temp_pl.name = Object_info[m_current].name;
  temp_pl.pagetype = PAGETYPE_GENERIC;

  const int r = mng_CheckIfPageLocked(&temp_pl);
  if (r == 2) {
    if (QMessageBox::question(this, "Are you sure?",
                          "This page is not even in the table file, or the database maybe corrupt.  Override to "
                              "'Unlocked'? (Select NO if you don't know what you're doing)") == QMessageBox::Yes) {
      temp_pl.holder = "UNLOCKED";
      if (!mng_ReplacePagelock(temp_pl.name, &temp_pl))
        QMessageBox::critical(this, "Error!", ErrorString);
    }
  } else if (r < 0) {
    QMessageBox::critical(this, "Error!", ErrorString);
  } else if (r == 1) {
    QMessageBox::information(this, "Information", InfoString);
  } else {
    temp_pl.holder = TableUser.toStdString();
    if (!mng_ReplacePagelock(temp_pl.name, &temp_pl)) {
      QMessageBox::critical(this, "Error!", ErrorString);
      mng_EraseLocker();
      return;
    }
    if (mng_FindSpecificGenericPage(temp_pl.name, &page)) {
      if (mng_AssignGenericPageToObjInfo(&page, m_current)) {
        if (!mng_ReplacePage(Object_info[m_current].name, Object_info[m_current].name, m_current, PAGETYPE_GENERIC, 1)) {
          QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was problem writing that page locally!");
          mng_EraseLocker();
          return;
        }
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Object locked.");
      } else {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was a problem loading this object.");
      }
      mng_AllocTrackLock(Object_info[m_current].name, PAGETYPE_GENERIC);
    } else {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't find that object in the table file!");
    }
  }
  mng_EraseLocker();
  updateDialog();
}

void WorldObjectsGenericDialog::onUndoLock() {
  if (m_current == -1)
    return;
  const int tl = mng_FindTrackLock(Object_info[m_current].name, PAGETYPE_GENERIC);
  if (tl == -1)
    return;
  if (QMessageBox::question(this, "Are you sure?",
                            "Are you sure you want to undo your lock and lose any changes you may have made?") == QMessageBox::No)
    return;
  if (!mng_MakeLocker())
    return;

  mngs_Pagelock pl;
  mngs_generic_page page;
  pl.name = Object_info[m_current].name;
  pl.pagetype = PAGETYPE_GENERIC;

  mng_FreeTrackLock(tl);
  Q_ASSERT(mng_DeletePage(Object_info[m_current].name, PAGETYPE_GENERIC, 1));
  Q_ASSERT(mng_FindSpecificGenericPage(pl.name, &page));
  Q_ASSERT(mng_AssignGenericPageToObjInfo(&page, m_current));
  mng_EraseLocker();
  updateDialog();
}

void WorldObjectsGenericDialog::onNext() {
  m_current = GetNextObjectID(m_current);
  m_lod = 0;
  updateDialog();
}

void WorldObjectsGenericDialog::onPrev() {
  m_current = GetPrevObjectID(m_current);
  m_lod = 0;
  updateDialog();
}

void WorldObjectsGenericDialog::onNamePulldownChanged() {
  QComboBox *combo = ui->IDC_NAME_PULLDOWN;
  const int i = FindObjectIDName(combo->currentText().toStdString());
  if (i == -1)
    return;
  m_current = i;
  updateDialog();
}

void WorldObjectsGenericDialog::onKillfocusSize() {
  if (m_current != -1)
    Object_info[m_current].size = ui->IDC_GENERIC_SIZE_EDIT->text().toFloat();
}

void WorldObjectsGenericDialog::onCopy() {
  if (m_current == -1)
    return;
  if (Copy_object.description) {
    mem_free(Copy_object.description);
    Copy_object.description = nullptr;
  }
  Copy_object = Object_info[m_current];
  Copy_object.description =
      mem_strdup(Object_info[m_current].description ? Object_info[m_current].description : "<no description>");
  Copy_object_used = true;
  updateDialog();
}

void WorldObjectsGenericDialog::onPaste() {
  if (!Network_up || !Copy_object_used)
    return;
  if (Copy_object.type != m_type) {
    if (QMessageBox::question(this, "Are you sure?", "You are about to paste a %s object as a %s.  Is this OK?",
                              Object_type_names[Copy_object.type], Object_type_names[m_type]) == QMessageBox::No)
      return;
  }

  std::string temp_name = Copy_object.name;
  if (FindObjectIDName(temp_name) != -1) {
    int c = 2;
    temp_name = "CopyOf" + Copy_object.name;
    while (FindObjectIDName(temp_name) != -1)
      temp_name = "Copy" + std::to_string(c++) + "Of" + Copy_object.name;
  }

  const int n = AllocObjectID(m_type, true, true, true);
  if (n == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot paste object: There are no free object slots.");
    return;
  }

  Object_info[n] = Copy_object;
  Object_info[n].description = Copy_object.description;
  Copy_object.description = nullptr;
  Object_info[n].type = m_type;
  Object_info[n].name = temp_name;
  Poly_models[Object_info[m_current].render_handle].used++;
  if (Object_info[m_current].med_render_handle != -1)
    Poly_models[Object_info[m_current].med_render_handle].used++;
  if (Object_info[m_current].lo_render_handle != -1)
    Poly_models[Object_info[m_current].lo_render_handle].used++;
  m_current = n;
  mng_AllocTrackLock(Object_info[n].name, PAGETYPE_GENERIC);
  RemapStaticIDs();
  updateDialog();
}

void WorldObjectsGenericDialog::onKillfocusHitpoints() {
  if (m_current != -1)
    Object_info[m_current].hit_points = ui->IDC_GENERIC_HITPOINT_EDIT->text().toInt();
}

void WorldObjectsGenericDialog::onWeaponInfo() {
  if (m_current == -1)
    return;
  extern void editRobotWeapons(otype_wb_info *wb, poly_model *pm, QWidget *parent);
  editRobotWeapons(Object_info[m_current].static_wb, GetPolymodelPointer(Object_info[m_current].render_handle),
                   this);
}

void WorldObjectsGenericDialog::onLight() {
  if (m_current == -1)
    return;
  QMessageBox::information(this, "Lighting", "The generic light dialog has not been ported yet.");
}

void WorldObjectsGenericDialog::onDefaultRadius() {
  if (m_current == -1)
    return;
  ComputeDefaultSize(Object_info[m_current].type, Object_info[m_current].render_handle, &Object_info[m_current].size);
  updateDialog();
}

void WorldObjectsGenericDialog::onSelScript() {
  if (m_current == -1)
    return;
  const QString filename = QFileDialog::getOpenFileName(this, "Select script module", QString::fromStdString(LocalScriptDir.string()),
                                                       "D3 Compiled Scripts (*.dll)");
  if (filename.isEmpty())
    return;
  const QFileInfo info(filename);
  Object_info[m_current].module_name = info.completeBaseName().toStdString() + ".dll";
  updateDialog();
}

void WorldObjectsGenericDialog::onKillfocusImpactDamage() {
  if (m_current != -1)
    Object_info[m_current].damage = ui->IDC_GENERIC_IMPACT_DAMAGE_EDIT->text().toFloat();
}

void WorldObjectsGenericDialog::onKillfocusImpactSize() {
  if (m_current != -1)
    Object_info[m_current].impact_size = ui->IDC_GENERIC_IMPACT_SIZE_EDIT->text().toFloat();
}

void WorldObjectsGenericDialog::onNolod() {
  if (m_current == -1)
    return;
  if (m_lod == 0) {
    QMessageBox::warning(this, "No LOD", "You must have a hi-res model.");
    return;
  }
  if (m_lod == 1) {
    FreePolyModel(Object_info[m_current].med_render_handle);
    Object_info[m_current].med_render_handle = -1;
  } else {
    FreePolyModel(Object_info[m_current].lo_render_handle);
    Object_info[m_current].lo_render_handle = -1;
  }
  updateDialog();
}

void WorldObjectsGenericDialog::onHiresRadio() {
  m_lod = 0;
  updateDialog();
}
void WorldObjectsGenericDialog::onMedresRadio() {
  m_lod = 1;
  updateDialog();
}
void WorldObjectsGenericDialog::onLoresRadio() {
  m_lod = 2;
  updateDialog();
}

void WorldObjectsGenericDialog::onKillfocusImpactTime() {
  if (m_current != -1)
    Object_info[m_current].impact_time = ui->IDC_GENERIC_IMPACT_TIME_EDIT->text().toFloat();
}

void WorldObjectsGenericDialog::onExplosionSoundChanged() {
  if (m_current != -1)
    Object_info[m_current].sounds[GSI_EXPLODE] = soundComboSelected(ui->IDC_GENERIC_EXPLOSION_SOUND_COMBO);
}

void WorldObjectsGenericDialog::onAmbientSoundChanged() {
  if (m_current != -1)
    Object_info[m_current].sounds[GSI_AMBIENT] = soundComboSelected(ui->IDC_GENERIC_AMBIENT_SOUND_COMBO);
}

void WorldObjectsGenericDialog::onKillfocusInvenDescription() {
  if (m_current == -1)
    return;
  auto *edit = ui->IDC_GENERIC_INVEN_DESCRIPTION;
  const QByteArray text = edit->toPlainText().toLocal8Bit();
  if (Object_info[m_current].description) {
    mem_free(Object_info[m_current].description);
    Object_info[m_current].description = nullptr;
  }
  if (text.size() > 0 && QString::compare(text.constData(), "<no description>") != 0) {
    Object_info[m_current].description = mem_strdup(text.constData());
  } else {
    edit->setText("<no description>");
  }
}

void WorldObjectsGenericDialog::onKillfocusInvenIconname() {
  if (m_current == -1)
    return;
  const QString text = ui->IDC_GENERIC_INVEN_ICONNAME->text();
  Object_info[m_current].icon_name = text.toStdString();
}

void WorldObjectsGenericDialog::onOverride() {
  if (m_current == -1)
    return;
  mngs_Pagelock temp_pl;
  temp_pl.name = Object_info[m_current].name;
  temp_pl.pagetype = PAGETYPE_GENERIC;
  mng_OverrideToUnlocked(&temp_pl);
}

void WorldObjectsGenericDialog::onKillfocusLodDistance() {
  if (m_current == -1)
    return;
  const float dist = ui->IDC_LOD_DISTANCE_EDIT->text().toFloat();
  if (dist < 0)
    return;
  if (m_lod == 1)
    Object_info[m_current].med_lod_distance = dist;
  else if (m_lod == 2)
    Object_info[m_current].lo_lod_distance = dist;
  updateDialog();
}

void WorldObjectsGenericDialog::onInvtypeGame() {
  if (m_current != -1)
    Object_info[m_current].flags.inven_type_mission = false;
  updateDialog();
}
void WorldObjectsGenericDialog::onInvtypeMission() {
  if (m_current != -1)
    Object_info[m_current].flags.inven_type_mission = true;
  updateDialog();
}

void WorldObjectsGenericDialog::onDeathPowerup1Changed() {
  if (m_current != -1)
    Object_info[m_current].dspew[0] =
        ui->IDC_DEATH_POWERUP1_PULLDOWN->currentData().toInt();
}
void WorldObjectsGenericDialog::onDeathPowerup1NumEdited() {
  if (m_current != -1)
    Object_info[m_current].dspew_number[0] = ui->IDC_DEATH_POWERUP1_NUM_EDIT->text().toInt();
}
void WorldObjectsGenericDialog::onDeathPowerup1PercentEdited() {
  if (m_current != -1)
    Object_info[m_current].dspew_percent[0] =
        ui->IDC_DEATH_POWERUP1_PERCENT_EDIT->text().toFloat() / 100.0f;
}
void WorldObjectsGenericDialog::onDeathPowerup2Changed() {
  if (m_current != -1)
    Object_info[m_current].dspew[1] =
        ui->IDC_DEATH_POWERUP2_PULLDOWN->currentData().toInt();
}
void WorldObjectsGenericDialog::onDeathPowerup2NumEdited() {
  if (m_current != -1)
    Object_info[m_current].dspew_number[1] = ui->IDC_DEATH_POWERUP2_NUM_EDIT->text().toInt();
}
void WorldObjectsGenericDialog::onDeathPowerup2PercentEdited() {
  if (m_current != -1)
    Object_info[m_current].dspew_percent[1] =
        ui->IDC_DEATH_POWERUP2_PERCENT_EDIT->text().toFloat() / 100.0f;
}
void WorldObjectsGenericDialog::onDeathPowerupUse2(bool checked) {
  if (m_current == -1)
    return;
  if (checked)
    Object_info[m_current].f_dspew |= DSF_ONLY_IF_PLAYER_HAS_OBJ_1;
  else
    Object_info[m_current].f_dspew &= ~DSF_ONLY_IF_PLAYER_HAS_OBJ_1;
}
void WorldObjectsGenericDialog::onDeathSpew2IfZero1(bool checked) {
  if (m_current == -1)
    return;
  if (checked)
    Object_info[m_current].f_dspew |= DSF_ONLY_IF_NO_1;
  else
    Object_info[m_current].f_dspew &= ~DSF_ONLY_IF_NO_1;
}

void WorldObjectsGenericDialog::onKillfocusScriptname() {
  if (m_current == -1)
    return;
  const QString text = ui->IDC_SCRIPTNAME->text();
  if (text == "null")
    Object_info[m_current].module_name.clear();
  else
    Object_info[m_current].module_name = text.toStdString();
  updateDialog();
}

void WorldObjectsGenericDialog::onCompilemodule() {
  if (m_current == -1)
    return;
  QMessageBox::information(this, "Compile", "The quick compile dialog has not been ported yet.");
}

void WorldObjectsGenericDialog::onKillfocusScriptOverride() {
  if (m_current == -1)
    return;
  const QString text = ui->IDC_SCRIPT_OVERRIDE->text();
  if (text == "null")
    Object_info[m_current].script_name_override.clear();
  else
    Object_info[m_current].script_name_override = text.toStdString();
  updateDialog();
}

void WorldObjectsGenericDialog::onKillfocusRespawnScalar() {
  if (m_current == -1)
    return;
  float val = ui->IDC_RESPAWN_SCALAR_EDIT->text().toFloat();
  if (val < -1)
    val = -1;
  if (val > 10000)
    val = 10000;
  Object_info[m_current].respawn_scalar = val;
  updateDialog();
}

void WorldObjectsGenericDialog::onGenericDeaths() {
  if (m_current == -1)
    return;
  GenericDeathDialog dlg(&Object_info[m_current], this);
  dlg.exec();
}

void WorldObjectsGenericDialog::onKillfocusScore() {
  if (m_current != -1)
    Object_info[m_current].score = ui->IDC_GENERIC_SCORE_EDIT->text().toInt();
}
void WorldObjectsGenericDialog::onKillfocusAmmo() {
  if (m_current != -1)
    Object_info[m_current].ammo_count = ui->IDC_GENERIC_AMMO_EDIT->text().toInt();
}

// Saves the locked generics to the local pagefile (original SaveGenericsOnClose).
void WorldObjectsGenericDialog::saveGenericsOnClose() {
  if (!Network_up)
    return;
  for (int i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used == 1 && GlobalTrackLocks[i].pagetype == PAGETYPE_GENERIC) {
      const int t = FindObjectIDName(GlobalTrackLocks[i].name);
      if (t != -1)
        mng_ReplacePage(Object_info[t].name, Object_info[t].name, t, PAGETYPE_GENERIC, 1);
    }
  }
}

int editGenericObject(int objType, int initialCurrent, QWidget *parent) {
  WorldObjectsGenericDialog dlg(objType, initialCurrent, parent);
  dlg.exec();
  return dlg.current();
}

