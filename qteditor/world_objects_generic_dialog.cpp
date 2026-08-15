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

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

#include <cmath>
#include <cstring>

#include "bitmap.h"
#include "cfile.h"
#include "qt_messagebox.h"
#include "d3edit.h"
#include "ddio.h"
#include "gametexture.h"
#include "generic_death_dialog.h"
#include "genericpage.h"
#include "manage.h"
#include "mem.h"
#include "objinfo.h"
#include "objinit.h"
#include "object.h"
#include "physics_dialog.h"
#include "polymodel.h"
#include "qt_debug.h"
#include "robotfire.h"
#include "sound_combo.h"
#include "vclip.h"

extern char Current_model_dir[_MAX_PATH];

namespace QtEditor {

namespace {
// The object being copied (file-local in the original MFC dialog too).
object_info Copy_object;
bool Copy_object_used = false;
} // namespace

WorldObjectsGenericDialog::WorldObjectsGenericDialog(int objType, int current, QWidget *parent)
    : Dialog(":/ui/worldobjectsgeneric.ui", parent), m_type(objType), m_current(current) {
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_EDIT_AI"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onEditAI);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_EDIT_PHYSICS"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onEditPhysics);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_ADD_NEW"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onAddNew);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_CHECKED_OUT"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onCheckedOut);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_CHECKIN"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onCheckIn);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_DEFINE_ANIMSTATES"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onDefineAnimStates);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_DELETE"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onDelete);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_LOCK"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onLock);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_UNDO_LOCK"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onUndoLock);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_NEXT"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onNext);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_PREV"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onPrev);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_CHANGE_MODEL"))
    connect(b, &QPushButton::clicked, this, [this]() { onAddNew(); });
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_COPY"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onCopy);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_PASTE"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onPaste);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_WEAPON_INFO_BUTTON"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onWeaponInfo);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_LIGHT_BUTTON"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onLight);
  if (QPushButton *b = find<QPushButton>("IDC_DEFAULT_RADIUS_BUTTON"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onDefaultRadius);
  if (QPushButton *b = find<QPushButton>("IDC_SELSCRIPT"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onSelScript);
  if (QPushButton *b = find<QPushButton>("IDC_COMPILEMODULE"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onCompilemodule);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_DEATHS"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onGenericDeaths);
  if (QPushButton *b = find<QPushButton>("IDC_OVERRIDE"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onOverride);
  if (QPushButton *b = find<QPushButton>("IDC_NOLOD"))
    connect(b, &QPushButton::clicked, this, &WorldObjectsGenericDialog::onNolod);

  if (QComboBox *combo = find<QComboBox>("IDC_NAME_PULLDOWN"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &WorldObjectsGenericDialog::onNamePulldownChanged);
  if (QComboBox *combo = find<QComboBox>("IDC_DEATH_POWERUP1_PULLDOWN"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &WorldObjectsGenericDialog::onDeathPowerup1Changed);
  if (QComboBox *combo = find<QComboBox>("IDC_DEATH_POWERUP2_PULLDOWN"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &WorldObjectsGenericDialog::onDeathPowerup2Changed);
  if (QComboBox *combo = find<QComboBox>("IDC_GENERIC_EXPLOSION_SOUND_COMBO"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &WorldObjectsGenericDialog::onExplosionSoundChanged);
  if (QComboBox *combo = find<QComboBox>("IDC_GENERIC_AMBIENT_SOUND_COMBO"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &WorldObjectsGenericDialog::onAmbientSoundChanged);

  const char *edits[] = {"IDC_GENERIC_SIZE_EDIT", "IDC_GENERIC_HITPOINT_EDIT", "IDC_GENERIC_IMPACT_DAMAGE_EDIT",
                         "IDC_GENERIC_IMPACT_SIZE_EDIT", "IDC_GENERIC_IMPACT_TIME_EDIT", "IDC_GENERIC_INVEN_DESCRIPTION",
                         "IDC_GENERIC_INVEN_ICONNAME", "IDC_LOD_DISTANCE_EDIT", "IDC_DEATH_POWERUP1_NUM_EDIT",
                         "IDC_DEATH_POWERUP1_PERCENT_EDIT", "IDC_DEATH_POWERUP2_NUM_EDIT",
                         "IDC_DEATH_POWERUP2_PERCENT_EDIT", "IDC_SCRIPTNAME", "IDC_SCRIPT_OVERRIDE",
                         "IDC_RESPAWN_SCALAR_EDIT", "IDC_GENERIC_SCORE_EDIT", "IDC_GENERIC_AMMO_EDIT"};
  for (const char *name : edits) {
    if (QLineEdit *edit = find<QLineEdit>(name))
      connect(edit, &QLineEdit::editingFinished, this, [this, name]() {
        if (m_current == -1)
          return;
        object_info *oi = &Object_info[m_current];
        if (QString::compare(name, "IDC_GENERIC_SIZE_EDIT") == 0)
          oi->size = find<QLineEdit>(name)->text().toFloat();
        else if (QString::compare(name, "IDC_GENERIC_HITPOINT_EDIT") == 0)
          oi->hit_points = find<QLineEdit>(name)->text().toInt();
        else if (QString::compare(name, "IDC_GENERIC_IMPACT_DAMAGE_EDIT") == 0)
          oi->damage = find<QLineEdit>(name)->text().toFloat();
        else if (QString::compare(name, "IDC_GENERIC_IMPACT_SIZE_EDIT") == 0)
          oi->impact_size = find<QLineEdit>(name)->text().toFloat();
        else if (QString::compare(name, "IDC_GENERIC_IMPACT_TIME_EDIT") == 0)
          oi->impact_time = find<QLineEdit>(name)->text().toFloat();
        else if (QString::compare(name, "IDC_GENERIC_INVEN_DESCRIPTION") == 0)
          onKillfocusInvenDescription();
        else if (QString::compare(name, "IDC_GENERIC_INVEN_ICONNAME") == 0)
          onKillfocusInvenIconname();
        else if (QString::compare(name, "IDC_LOD_DISTANCE_EDIT") == 0)
          onKillfocusLodDistance();
        else if (QString::compare(name, "IDC_DEATH_POWERUP1_NUM_EDIT") == 0)
          oi->dspew_number[0] = find<QLineEdit>(name)->text().toInt();
        else if (QString::compare(name, "IDC_DEATH_POWERUP1_PERCENT_EDIT") == 0)
          oi->dspew_percent[0] = find<QLineEdit>(name)->text().toFloat() / 100.0f;
        else if (QString::compare(name, "IDC_DEATH_POWERUP2_NUM_EDIT") == 0)
          oi->dspew_number[1] = find<QLineEdit>(name)->text().toInt();
        else if (QString::compare(name, "IDC_DEATH_POWERUP2_PERCENT_EDIT") == 0)
          oi->dspew_percent[1] = find<QLineEdit>(name)->text().toFloat() / 100.0f;
        else if (QString::compare(name, "IDC_SCRIPTNAME") == 0)
          onKillfocusScriptname();
        else if (QString::compare(name, "IDC_SCRIPT_OVERRIDE") == 0)
          onKillfocusScriptOverride();
        else if (QString::compare(name, "IDC_RESPAWN_SCALAR_EDIT") == 0)
          onKillfocusRespawnScalar();
        else if (QString::compare(name, "IDC_GENERIC_SCORE_EDIT") == 0)
          oi->score = find<QLineEdit>(name)->text().toInt();
        else if (QString::compare(name, "IDC_GENERIC_AMMO_EDIT") == 0)
          oi->ammo_count = find<QLineEdit>(name)->text().toInt();
      });
  }

  const struct {
    const char *name;
    uint32_t flag;
  } checks[] = {
      {"IDC_GENERIC_USES_AI", OIF_CONTROL_AI}, {"IDC_GENERIC_USES_PHYSICS", OIF_USES_PHYSICS},
      {"IDC_GENERIC_DESTROYABLE", OIF_DESTROYABLE}, {"IDC_GENERIC_AI_SCRIPTED_DEATH", OIF_AI_SCRIPTED_DEATH},
      {"IDC_OBJ_CEILING_CHECK", OIF_DO_CEILING_CHECK},
      {"IDC_OBJECT_FLY_THROUGH_RENDERED_PORTALS", OIF_IGNORE_FORCEFIELDS_AND_GLASS},
      {"IDC_NSC_BUTTON", OIF_NO_DIFF_SCALE_DAMAGE}, {"IDC_DSMPBD_CHECK", OIF_NO_DIFF_SCALE_MOVE},
      {"IDC_GENERIC_AMBIENT", OIF_AMBIENT_OBJECT}, {"IDC_GENERIC_INVEN_SELECTABLE", OIF_INVEN_SELECTABLE},
      {"IDC_GENERIC_INVEN_NONUSEABLE", OIF_INVEN_NONUSEABLE}, {"IDC_INVEN_NOREMOVE", OIF_INVEN_NOREMOVE},
      {"IDC_INVEN_VISWHENUSED", OIF_INVEN_VISWHENUSED},
  };
  for (const auto &c : checks) {
    if (QCheckBox *cb = find<QCheckBox>(c.name))
      connect(cb, &QCheckBox::toggled, this, [this, c](bool checked) { setFlag(c.flag, c.name, checked); });
  }
  if (QCheckBox *cb = find<QCheckBox>("IDC_DEATH_POWERUP_USE2_IF_HAVE1_CHECK"))
    connect(cb, &QCheckBox::toggled, this, &WorldObjectsGenericDialog::onDeathPowerupUse2);
  if (QCheckBox *cb = find<QCheckBox>("IDC_GENERIC_DEATH_SPEW_2_IF_ZERO_1"))
    connect(cb, &QCheckBox::toggled, this, &WorldObjectsGenericDialog::onDeathSpew2IfZero1);

  if (QRadioButton *rb = find<QRadioButton>("IDC_HIRES_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &WorldObjectsGenericDialog::onHiresRadio);
  if (QRadioButton *rb = find<QRadioButton>("IDC_MEDRES_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &WorldObjectsGenericDialog::onMedresRadio);
  if (QRadioButton *rb = find<QRadioButton>("IDC_LORES_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &WorldObjectsGenericDialog::onLoresRadio);
  if (QRadioButton *rb = find<QRadioButton>("IDC_INVTYPE_GAME"))
    connect(rb, &QRadioButton::clicked, this, &WorldObjectsGenericDialog::onInvtypeGame);
  if (QRadioButton *rb = find<QRadioButton>("IDC_INVTYPE_MISSION"))
    connect(rb, &QRadioButton::clicked, this, &WorldObjectsGenericDialog::onInvtypeMission);

  if (QLineEdit *edit = find<QLineEdit>("IDC_GENERIC_TYPE_NAME"))
    edit->setText(Object_type_names[m_type]);

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

void WorldObjectsGenericDialog::setFlag(uint32_t flag, const char *checkName, bool checked) {
  if (m_current == -1)
    return;
  if (checked)
    Object_info[m_current].flags |= flag;
  else
    Object_info[m_current].flags &= ~flag;

  if (QString::compare(checkName, "IDC_GENERIC_USES_AI") == 0) {
    if (QWidget *w = find<QWidget>("IDC_GENERIC_EDIT_AI"))
      w->setEnabled(checked);
  } else if (QString::compare(checkName, "IDC_GENERIC_USES_PHYSICS") == 0) {
    if (QWidget *w = find<QWidget>("IDC_GENERIC_EDIT_PHYSICS"))
      w->setEnabled(checked);
  } else if (QString::compare(checkName, "IDC_GENERIC_DESTROYABLE") == 0) {
    if (QWidget *w = find<QWidget>("IDC_GENERIC_HITPOINT_EDIT"))
      w->setEnabled(checked);
    if (QWidget *w = find<QWidget>("IDC_GENERIC_SCORE_EDIT"))
      w->setEnabled(checked);
  }
}

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
    if (QWidget *w = find<QWidget>(name))
      w->setEnabled(flag);
}

void WorldObjectsGenericDialog::updateDialog() {
  if (m_current == -1) {
    enableDisableAll(false);
    if (QPushButton *paste = find<QPushButton>("IDC_GENERIC_PASTE"))
      paste->setEnabled(Network_up && Copy_object_used);
    if (!Network_up) {
      for (const char *name : {"IDC_GENERIC_LOCK", "IDC_GENERIC_CHECKIN", "IDC_GENERIC_UNDO_LOCK",
                               "IDC_GENERIC_CHECKED_OUT", "IDC_OVERRIDE"}) {
        if (auto *w = find<QPushButton>(name))
          w->setEnabled(false);
      }
    }
    return;
  }

  if (!Network_up) {
    for (const char *name : {"IDC_GENERIC_LOCK", "IDC_GENERIC_CHECKIN", "IDC_GENERIC_UNDO_LOCK",
                             "IDC_GENERIC_CHECKED_OUT", "IDC_OVERRIDE"}) {
      if (auto *w = find<QPushButton>(name))
        w->setEnabled(false);
    }
  }

  enableDisableAll(true);
  object_info *oi = &Object_info[m_current];
  const int n = m_current;

  char txt[256];

  PageInPolymodel(oi->render_handle);
  if (QLineEdit *edit = find<QLineEdit>("IDC_GENERIC_MODEL_NAME_EDIT")) {
    if (m_lod == 0)
      edit->setText(Poly_models[oi->render_handle].name);
    else if (m_lod == 1)
      edit->setText(oi->med_render_handle == -1 ? "No model defined" : Poly_models[oi->med_render_handle].name);
    else
      edit->setText(oi->lo_render_handle == -1 ? "No model defined" : Poly_models[oi->lo_render_handle].name);
  }

  if (QLineEdit *edit = find<QLineEdit>("IDC_LOD_DISTANCE_EDIT")) {
    if (m_lod == 0)
      edit->setText("0");
    else if (m_lod == 1)
      edit->setText(QString::number(oi->med_lod_distance));
    else
      edit->setText(QString::number(oi->lo_lod_distance));
  }

  const int poly_handle =
      m_lod == 0 ? oi->render_handle : (m_lod == 1 ? oi->med_render_handle : oi->lo_render_handle);
  if (QLabel *label = find<QLabel>("IDC_NUM_POLYS")) {
    if (poly_handle == -1)
      label->setText("Num polys: NA");
    else {
      PageInPolymodel(poly_handle);
      label->setText(QString("Num polys: %1").arg(CountFacesInPolymodel(&Poly_models[poly_handle])));
    }
  }
  if (QLabel *label = find<QLabel>("IDC_TEXTURE_RAM_TEXT")) {
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
              if (texp->flags & TF_ANIMATED) {
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

  if (QLineEdit *edit = find<QLineEdit>("IDC_RESPAWN_SCALAR_EDIT"))
    edit->setText(QString::number(oi->respawn_scalar));
  if (QLineEdit *edit = find<QLineEdit>("IDC_GENERIC_IMPACT_SIZE_EDIT"))
    edit->setText(QString::number(oi->impact_size));
  if (QLineEdit *edit = find<QLineEdit>("IDC_GENERIC_IMPACT_TIME_EDIT"))
    edit->setText(QString::number(oi->impact_time));
  if (QLineEdit *edit = find<QLineEdit>("IDC_GENERIC_IMPACT_DAMAGE_EDIT"))
    edit->setText(QString::number(oi->damage));
  if (QLineEdit *edit = find<QLineEdit>("IDC_DEATH_POWERUP1_NUM_EDIT"))
    edit->setText(QString::number(oi->dspew_number[0]));
  if (QLineEdit *edit = find<QLineEdit>("IDC_DEATH_POWERUP2_NUM_EDIT"))
    edit->setText(QString::number(oi->dspew_number[1]));
  if (QLineEdit *edit = find<QLineEdit>("IDC_DEATH_POWERUP1_PERCENT_EDIT"))
    edit->setText(QString::number(oi->dspew_percent[0] * 100.0f));
  if (QLineEdit *edit = find<QLineEdit>("IDC_DEATH_POWERUP2_PERCENT_EDIT"))
    edit->setText(QString::number(oi->dspew_percent[1] * 100.0f));

  if (QCheckBox *cb = find<QCheckBox>("IDC_GENERIC_USES_PHYSICS"))
    cb->setChecked(oi->flags & OIF_USES_PHYSICS);
  if (QWidget *w = find<QWidget>("IDC_GENERIC_EDIT_PHYSICS"))
    w->setEnabled(oi->flags & OIF_USES_PHYSICS);
  if (QCheckBox *cb = find<QCheckBox>("IDC_GENERIC_USES_AI"))
    cb->setChecked(oi->flags & OIF_CONTROL_AI);
  if (QWidget *w = find<QWidget>("IDC_GENERIC_EDIT_AI"))
    w->setEnabled(oi->flags & OIF_CONTROL_AI);

  if (QCheckBox *cb = find<QCheckBox>("IDC_DEATH_POWERUP_USE2_IF_HAVE1_CHECK"))
    cb->setChecked(oi->f_dspew & DSF_ONLY_IF_PLAYER_HAS_OBJ_1);
  if (QCheckBox *cb = find<QCheckBox>("IDC_GENERIC_DEATH_SPEW_2_IF_ZERO_1"))
    cb->setChecked(oi->f_dspew & DSF_ONLY_IF_NO_1);

  setSoundComboSelected(find<QComboBox>("IDC_GENERIC_EXPLOSION_SOUND_COMBO"), oi->sounds[GSI_EXPLODE]);
  setSoundComboSelected(find<QComboBox>("IDC_GENERIC_AMBIENT_SOUND_COMBO"), oi->sounds[GSI_AMBIENT]);

  if (QComboBox *combo = find<QComboBox>("IDC_DEATH_POWERUP1_PULLDOWN")) {
    QSignalBlocker blocker(combo);
    combo->clear();
    combo->addItem("<none>", -1);
    for (int i = 0; i < MAX_OBJECT_IDS; i++)
      if (Object_info[i].type != OBJ_NONE)
        combo->addItem(Object_info[i].name, i);
    const int sp1 = oi->dspew[0];
    combo->setCurrentIndex(combo->findData(sp1 >= 0 && sp1 < MAX_OBJECT_IDS ? sp1 : -1));
  }
  if (QComboBox *combo = find<QComboBox>("IDC_DEATH_POWERUP2_PULLDOWN")) {
    QSignalBlocker blocker(combo);
    combo->clear();
    combo->addItem("<none>", -1);
    for (int i = 0; i < MAX_OBJECT_IDS; i++)
      if (Object_info[i].type != OBJ_NONE)
        combo->addItem(Object_info[i].name, i);
    const int sp2 = oi->dspew[1];
    combo->setCurrentIndex(combo->findData(sp2 >= 0 && sp2 < MAX_OBJECT_IDS ? sp2 : -1));
  }

  if (QWidget *w = find<QWidget>("IDC_GENERIC_WEAPON_INFO_BUTTON"))
    w->setEnabled(Poly_models[oi->render_handle].num_wbs > 0);

  if (QLineEdit *edit = find<QLineEdit>("IDC_GENERIC_SIZE_EDIT"))
    edit->setText(QString::number(oi->size, 'f', 2));

  if (QCheckBox *cb = find<QCheckBox>("IDC_GENERIC_DESTROYABLE"))
    cb->setChecked(oi->flags & OIF_DESTROYABLE);
  if (QCheckBox *cb = find<QCheckBox>("IDC_GENERIC_AI_SCRIPTED_DEATH"))
    cb->setChecked(oi->flags & OIF_AI_SCRIPTED_DEATH);
  if (QCheckBox *cb = find<QCheckBox>("IDC_OBJ_CEILING_CHECK"))
    cb->setChecked(oi->flags & OIF_DO_CEILING_CHECK);
  if (QCheckBox *cb = find<QCheckBox>("IDC_OBJECT_FLY_THROUGH_RENDERED_PORTALS"))
    cb->setChecked(oi->flags & OIF_IGNORE_FORCEFIELDS_AND_GLASS);
  if (QCheckBox *cb = find<QCheckBox>("IDC_NSC_BUTTON"))
    cb->setChecked(oi->flags & OIF_NO_DIFF_SCALE_DAMAGE);
  if (QCheckBox *cb = find<QCheckBox>("IDC_DSMPBD_CHECK"))
    cb->setChecked(oi->flags & OIF_NO_DIFF_SCALE_MOVE);
  if (QCheckBox *cb = find<QCheckBox>("IDC_GENERIC_AMBIENT"))
    cb->setChecked(oi->flags & OIF_AMBIENT_OBJECT);

  const bool destroyable = (oi->flags & OIF_DESTROYABLE) != 0;
  if (QLineEdit *edit = find<QLineEdit>("IDC_GENERIC_HITPOINT_EDIT")) {
    edit->setText(destroyable ? QString::number(oi->hit_points) : "");
    edit->setEnabled(destroyable);
  }
  if (QLineEdit *edit = find<QLineEdit>("IDC_GENERIC_SCORE_EDIT")) {
    edit->setText(destroyable ? QString::number(oi->score) : "");
    edit->setEnabled(destroyable);
  }
  if (QLineEdit *edit = find<QLineEdit>("IDC_GENERIC_AMMO_EDIT"))
    edit->setText(QString::number(oi->ammo_count));
  if (QWidget *w = find<QWidget>("IDC_GENERIC_AMMO_EDIT"))
    w->setEnabled(oi->type == OBJ_POWERUP);
  if (QWidget *w = find<QWidget>("IDC_GENERIC_AMMO_TEXT"))
    w->setEnabled(oi->type == OBJ_POWERUP);

  if (QPushButton *checkedOut = find<QPushButton>("IDC_GENERIC_CHECKED_OUT"))
    checkedOut->setEnabled(m_locked_count > 0);
  if (QLineEdit *edit = find<QLineEdit>("IDC_GENERIC_ID_EDIT"))
    edit->setText(QString::number(m_current));

  if (QPushButton *paste = find<QPushButton>("IDC_GENERIC_PASTE"))
    paste->setEnabled(Network_up && Copy_object_used);
  if (QPushButton *del = find<QPushButton>("IDC_GENERIC_DELETE"))
    del->setEnabled(isLocked(m_current));
  if (QPushButton *lock = find<QPushButton>("IDC_GENERIC_LOCK"))
    lock->setEnabled(Network_up && !isLocked(m_current));
  if (QPushButton *checkin = find<QPushButton>("IDC_GENERIC_CHECKIN"))
    checkin->setEnabled(Network_up && isLocked(m_current));
  if (QPushButton *undolock = find<QPushButton>("IDC_GENERIC_UNDO_LOCK"))
    undolock->setEnabled(Network_up && isLocked(m_current));
  if (QPushButton *addnew = find<QPushButton>("IDC_GENERIC_ADD_NEW"))
    addnew->setEnabled(Network_up);

  if (QComboBox *combo = find<QComboBox>("IDC_NAME_PULLDOWN")) {
    QSignalBlocker blocker(combo);
    combo->clear();
    const int first = GetObjectID(m_type);
    if (first != -1) {
      int i = first;
      do {
        combo->addItem(Object_info[i].name);
        i = GetNextObjectID(i);
      } while (i != first);
      combo->setCurrentText(Object_info[m_current].name);
    }
  }

  if (QLineEdit *edit = find<QLineEdit>("IDC_SCRIPTNAME"))
    edit->setText(oi->module_name[0] ? oi->module_name : "null");
  if (QLineEdit *edit = find<QLineEdit>("IDC_SCRIPT_OVERRIDE"))
    edit->setText(oi->script_name_override[0] ? oi->script_name_override : "null");

  if (QCheckBox *cb = find<QCheckBox>("IDC_GENERIC_INVEN_SELECTABLE"))
    cb->setChecked(oi->flags & OIF_INVEN_SELECTABLE);
  if (QCheckBox *cb = find<QCheckBox>("IDC_GENERIC_INVEN_NONUSEABLE"))
    cb->setChecked(oi->flags & OIF_INVEN_NONUSEABLE);
  if (QCheckBox *cb = find<QCheckBox>("IDC_INVEN_NOREMOVE"))
    cb->setChecked(oi->flags & OIF_INVEN_NOREMOVE);
  if (QCheckBox *cb = find<QCheckBox>("IDC_INVEN_VISWHENUSED"))
    cb->setChecked(oi->flags & OIF_INVEN_VISWHENUSED);

  const bool mission = (oi->flags & OIF_INVEN_TYPE_MISSION) != 0;
  if (QRadioButton *rb = find<QRadioButton>("IDC_INVTYPE_MISSION"))
    rb->setChecked(mission);
  if (QRadioButton *rb = find<QRadioButton>("IDC_INVTYPE_GAME"))
    rb->setChecked(!mission);

  if (QLineEdit *edit = find<QLineEdit>("IDC_GENERIC_INVEN_DESCRIPTION"))
    edit->setText(oi->description ? oi->description : "<no description>");
  if (QLineEdit *edit = find<QLineEdit>("IDC_GENERIC_INVEN_ICONNAME"))
    edit->setText(oi->icon_name[0] ? oi->icon_name : "<no icon>");

  if (QPushButton *nolod = find<QPushButton>("IDC_NOLOD")) {
    if (m_lod == 0)
      nolod->setEnabled(false);
    else if (m_lod == 1)
      nolod->setEnabled(oi->med_render_handle != -1);
    else
      nolod->setEnabled(oi->lo_render_handle != -1);
  }
  if (QRadioButton *rb = find<QRadioButton>("IDC_HIRES_RADIO"))
    rb->setChecked(m_lod == 0);
  if (QRadioButton *rb = find<QRadioButton>("IDC_MEDRES_RADIO"))
    rb->setChecked(m_lod == 1);
  if (QRadioButton *rb = find<QRadioButton>("IDC_LORES_RADIO"))
    rb->setChecked(m_lod == 2);

  (void)txt;
  (void)n;
}

void WorldObjectsGenericDialog::onEditAI() {
  if (m_current == -1)
    return;
  QMessageBox::information(m_dialog, "AI Properties", "The AI Properties dialog has not been ported yet.");
}

void WorldObjectsGenericDialog::onEditPhysics() {
  if (m_current == -1)
    return;
  PhysicsDialog dlg(&Object_info[m_current].phys_info, m_dialog);
  dlg.exec();
}

void WorldObjectsGenericDialog::onUsesAI(bool checked) { setFlag(OIF_CONTROL_AI, "IDC_GENERIC_USES_AI", checked); }

void WorldObjectsGenericDialog::onUsesPhysics(bool checked) {
  setFlag(OIF_USES_PHYSICS, "IDC_GENERIC_USES_PHYSICS", checked);
}

void WorldObjectsGenericDialog::onAddNew() {
  if (!Network_up) {
    OutrageMessageBox("Sorry babe, the network is down.  This action is a no-no.\n");
    return;
  }

  const QString pathname =
      QFileDialog::getOpenFileName(m_dialog, "Select model", Current_model_dir, "Descent III files (*.pof *.oof)");
  if (pathname.isEmpty())
    return;

  const QByteArray pathBytes = pathname.toLocal8Bit();
  char dir[260], fname[260], ext[32];
  ddio_SplitPath(pathBytes.constData(), dir, fname, ext);

  std::filesystem::path tmp = ChangePolyModelName(pathname.toStdString());
  if (FindPolyModelName(fname) != -1) {
    OutrageMessageBox("You must rename your model to something else because there is already a model with that name!");
    return;
  }

  const int img_handle = LoadPolyModel(pathname.toStdString(), 0);
  if (img_handle < 0) {
    OutrageMessageBox("Couldn't open that model file.");
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
  while (FindObjectIDName(cur_name) != -1)
    snprintf(cur_name, sizeof(cur_name), "%s%d", fname, ++c);
  cur_name[0] = toupper(cur_name[0]);

  if (FindObjectIDName(cur_name) != -1) {
    OutrageMessageBox("That name is taken, please choose another.");
    return;
  }

  const int object_handle = AllocObjectID(m_type, true, true, true);
  if (object_handle == -1) {
    OutrageMessageBox("Cannot add object: There are no free object slots.");
    return;
  }

  snprintf(Object_info[object_handle].name, sizeof(Object_info[object_handle].name), "%s", cur_name);
  Object_info[object_handle].render_handle = img_handle;
  ComputeDefaultSize(Object_info[object_handle].type, img_handle, &Object_info[object_handle].size);
  Object_info[object_handle].flags = OIF_DESTROYABLE;
  Object_info[object_handle].hit_points = 100;
  memset(&Object_info[object_handle].lighting_info, 0, sizeof(light_info));
  Object_info[object_handle].lighting_info.timebits = 0xFFFFFFFF;
  Object_info[object_handle].lighting_info.lighting_render_type =
      (m_type == OBJ_BUILDING) ? LRT_LIGHTMAPS : LRT_GOURAUD;

  std::filesystem::path destname = LocalModelsDir / Poly_models[Object_info[object_handle].render_handle].name;
  cf_CopyFile(destname, pathname.toStdString());

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
        str += GlobalTrackLocks[i].name;
        str += "\n";
        total++;
      }
    }
  }
  if (total != 0)
    QMessageBox::information(m_dialog, "Objects", str);
}

void WorldObjectsGenericDialog::onCheckIn() {
  if (m_current == -1)
    return;
  if (!mng_MakeLocker())
    return;

  mngs_Pagelock temp_pl;
  snprintf(temp_pl.name, sizeof(temp_pl.name), "%s", Object_info[m_current].name);
  temp_pl.pagetype = PAGETYPE_GENERIC;

  const int r = mng_CheckIfPageOwned(&temp_pl, TableUser);
  if (r < 0)
    OutrageMessageBox(ErrorString);
  else if (r == 0)
    OutrageMessageBox(InfoString);
  else {
    snprintf(temp_pl.holder, sizeof(temp_pl.holder), "UNLOCKED");
    if (!mng_ReplacePagelock(temp_pl.name, &temp_pl)) {
      QMessageBox::critical(m_dialog, "Error!", ErrorString);
      mng_EraseLocker();
      return;
    }
    if (!mng_ReplacePage(Object_info[m_current].name, Object_info[m_current].name, m_current, PAGETYPE_GENERIC, 0)) {
      OutrageMessageBox(ErrorString);
    } else {
      std::filesystem::path srcname = LocalModelsDir / Poly_models[Object_info[m_current].render_handle].name;
      std::filesystem::path destname = NetModelsDir / Poly_models[Object_info[m_current].render_handle].name;
      cf_CopyFile(destname, srcname);
      if (Object_info[m_current].med_render_handle != -1) {
        srcname = LocalModelsDir / Poly_models[Object_info[m_current].med_render_handle].name;
        destname = NetModelsDir / Poly_models[Object_info[m_current].med_render_handle].name;
        cf_CopyFile(destname, srcname);
      }
      if (Object_info[m_current].lo_render_handle != -1) {
        srcname = LocalModelsDir / Poly_models[Object_info[m_current].lo_render_handle].name;
        destname = NetModelsDir / Poly_models[Object_info[m_current].lo_render_handle].name;
        cf_CopyFile(destname, srcname);
      }
      OutrageMessageBox("Object checked in.");
      ASSERT(mng_DeletePage(Object_info[m_current].name, PAGETYPE_GENERIC, 1) == 1);
      mng_EraseLocker();
      const int p = mng_FindTrackLock(Object_info[m_current].name, PAGETYPE_GENERIC);
      ASSERT(p != -1);
      mng_FreeTrackLock(p);
    }
  }
  mng_EraseLocker();
  updateDialog();
}

void WorldObjectsGenericDialog::onDefineAnimStates() {
  if (m_current == -1)
    return;
  QMessageBox::information(m_dialog, "Anim States", "The Anim States dialog has not been ported yet.");
}

void WorldObjectsGenericDialog::onDelete() {
  if (m_current == -1)
    return;
  const int tl = mng_FindTrackLock(Object_info[m_current].name, PAGETYPE_GENERIC);
  if (tl == -1) {
    OutrageMessageBox("This object is not yours to delete.  Lock first.");
    return;
  }
  if (QMessageBox::question(m_dialog, "Delete object",
                            QString("Are you sure you want to delete this object? %1").arg(Object_info[m_current].name)) !=
      QMessageBox::Yes)
    return;
  if (!mng_MakeLocker())
    return;

  mngs_Pagelock pl;
  snprintf(pl.name, sizeof(pl.name), "%s", Object_info[m_current].name);
  pl.pagetype = PAGETYPE_GENERIC;

  if (mng_CheckIfPageOwned(&pl, TableUser) != 1) {
    mng_FreeTrackLock(tl);
    if (!mng_DeletePage(Object_info[m_current].name, PAGETYPE_GENERIC, 1))
      Int3();
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
  OutrageMessageBox("Object deleted.");
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
  snprintf(temp_pl.name, sizeof(temp_pl.name), "%s", Object_info[m_current].name);
  temp_pl.pagetype = PAGETYPE_GENERIC;

  const int r = mng_CheckIfPageLocked(&temp_pl);
  if (r == 2) {
    if (OutrageMessageBox(MBOX_YESNO,
                          "This page is not even in the table file, or the database maybe corrupt.  Override to "
                          "'Unlocked'? (Select NO if you don't know what you're doing)") == 1) {
      snprintf(temp_pl.holder, sizeof(temp_pl.holder), "UNLOCKED");
      if (!mng_ReplacePagelock(temp_pl.name, &temp_pl))
        QMessageBox::critical(m_dialog, "Error!", ErrorString);
    }
  } else if (r < 0) {
    OutrageMessageBox(ErrorString);
  } else if (r == 1) {
    OutrageMessageBox(InfoString);
  } else {
    snprintf(temp_pl.holder, sizeof(temp_pl.holder), "%s", TableUser);
    if (!mng_ReplacePagelock(temp_pl.name, &temp_pl)) {
      QMessageBox::critical(m_dialog, "Error!", ErrorString);
      mng_EraseLocker();
      return;
    }
    if (mng_FindSpecificGenericPage(temp_pl.name, &page)) {
      if (mng_AssignGenericPageToObjInfo(&page, m_current)) {
        if (!mng_ReplacePage(Object_info[m_current].name, Object_info[m_current].name, m_current, PAGETYPE_GENERIC, 1)) {
          OutrageMessageBox("There was problem writing that page locally!");
          mng_EraseLocker();
          return;
        }
        OutrageMessageBox("Object locked.");
      } else {
        OutrageMessageBox("There was a problem loading this object.");
      }
      mng_AllocTrackLock(Object_info[m_current].name, PAGETYPE_GENERIC);
    } else {
      OutrageMessageBox("Couldn't find that object in the table file!");
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
  if (OutrageMessageBox(MBOX_YESNO,
                        "Are you sure you want to undo your lock and lose any changes you may have made?") != 1)
    return;
  if (!mng_MakeLocker())
    return;

  mngs_Pagelock pl;
  mngs_generic_page page;
  snprintf(pl.name, sizeof(pl.name), "%s", Object_info[m_current].name);
  pl.pagetype = PAGETYPE_GENERIC;

  mng_FreeTrackLock(tl);
  if (!mng_DeletePage(Object_info[m_current].name, PAGETYPE_GENERIC, 1))
    Int3();
  if (!mng_FindSpecificGenericPage(pl.name, &page))
    Int3();
  if (!mng_AssignGenericPageToObjInfo(&page, m_current))
    Int3();
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
  QComboBox *combo = find<QComboBox>("IDC_NAME_PULLDOWN");
  const int i = FindObjectIDName(combo->currentText().toLocal8Bit().constData());
  if (i == -1)
    return;
  m_current = i;
  updateDialog();
}

void WorldObjectsGenericDialog::onKillfocusSize() {
  if (m_current != -1)
    Object_info[m_current].size = find<QLineEdit>("IDC_GENERIC_SIZE_EDIT")->text().toFloat();
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
    if (OutrageMessageBox(MBOX_YESNO, "You are about to paste a %s object as a %s.  Is this OK?",
                          Object_type_names[Copy_object.type], Object_type_names[m_type]) != 1)
      return;
  }

  char temp_name[PAGENAME_LEN + 10];
  snprintf(temp_name, sizeof(temp_name), "%s", Copy_object.name);
  if (FindObjectIDName(temp_name) != -1) {
    int c = 2;
    snprintf(temp_name, sizeof(temp_name), "CopyOf%s", Copy_object.name);
    while (FindObjectIDName(temp_name) != -1)
      snprintf(temp_name, sizeof(temp_name), "Copy%dOf%s", c++, Copy_object.name);
  }

  const int n = AllocObjectID(m_type, true, true, true);
  if (n == -1) {
    OutrageMessageBox("Cannot paste object: There are no free object slots.");
    return;
  }

  Object_info[n] = Copy_object;
  Object_info[n].description = Copy_object.description;
  Copy_object.description = nullptr;
  Object_info[n].type = m_type;
  snprintf(Object_info[n].name, sizeof(Object_info[n].name), "%s", temp_name);
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

void WorldObjectsGenericDialog::onDestroyableToggled(bool checked) {
  setFlag(OIF_DESTROYABLE, "IDC_GENERIC_DESTROYABLE", checked);
  updateDialog();
}

void WorldObjectsGenericDialog::onKillfocusHitpoints() {
  if (m_current != -1)
    Object_info[m_current].hit_points = find<QLineEdit>("IDC_GENERIC_HITPOINT_EDIT")->text().toInt();
}

void WorldObjectsGenericDialog::onWeaponInfo() {
  if (m_current == -1)
    return;
  extern void editRobotWeapons(otype_wb_info *wb, poly_model *pm, QWidget *parent);
  editRobotWeapons(Object_info[m_current].static_wb, GetPolymodelPointer(Object_info[m_current].render_handle),
                   m_dialog);
}

void WorldObjectsGenericDialog::onLight() {
  if (m_current == -1)
    return;
  QMessageBox::information(m_dialog, "Lighting", "The generic light dialog has not been ported yet.");
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
  const QString filename = QFileDialog::getOpenFileName(m_dialog, "Select script module", LocalScriptDir,
                                                       "D3 Compiled Scripts (*.dll)");
  if (filename.isEmpty())
    return;
  const QFileInfo info(filename);
  snprintf(Object_info[m_current].module_name, sizeof(Object_info[m_current].module_name), "%s.dll",
           info.completeBaseName().toLocal8Bit().constData());
  updateDialog();
}

void WorldObjectsGenericDialog::onKillfocusImpactDamage() {
  if (m_current != -1)
    Object_info[m_current].damage = find<QLineEdit>("IDC_GENERIC_IMPACT_DAMAGE_EDIT")->text().toFloat();
}

void WorldObjectsGenericDialog::onKillfocusImpactSize() {
  if (m_current != -1)
    Object_info[m_current].impact_size = find<QLineEdit>("IDC_GENERIC_IMPACT_SIZE_EDIT")->text().toFloat();
}

void WorldObjectsGenericDialog::onNolod() {
  if (m_current == -1)
    return;
  if (m_lod == 0) {
    QMessageBox::warning(m_dialog, "No LOD", "You must have a hi-res model.");
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
    Object_info[m_current].impact_time = find<QLineEdit>("IDC_GENERIC_IMPACT_TIME_EDIT")->text().toFloat();
}

void WorldObjectsGenericDialog::onExplosionSoundChanged() {
  if (m_current != -1)
    Object_info[m_current].sounds[GSI_EXPLODE] = soundComboSelected(find<QComboBox>("IDC_GENERIC_EXPLOSION_SOUND_COMBO"));
}

void WorldObjectsGenericDialog::onAmbientSoundChanged() {
  if (m_current != -1)
    Object_info[m_current].sounds[GSI_AMBIENT] = soundComboSelected(find<QComboBox>("IDC_GENERIC_AMBIENT_SOUND_COMBO"));
}

void WorldObjectsGenericDialog::onKillfocusInvenDescription() {
  if (m_current == -1)
    return;
  QLineEdit *edit = find<QLineEdit>("IDC_GENERIC_INVEN_DESCRIPTION");
  const QByteArray text = edit->text().toLocal8Bit();
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
  const QByteArray text = find<QLineEdit>("IDC_GENERIC_INVEN_ICONNAME")->text().toLocal8Bit();
  snprintf(Object_info[m_current].icon_name, sizeof(Object_info[m_current].icon_name), "%s", text.constData());
}

void WorldObjectsGenericDialog::onOverride() {
  if (m_current == -1)
    return;
  mngs_Pagelock temp_pl;
  snprintf(temp_pl.name, sizeof(temp_pl.name), "%s", Object_info[m_current].name);
  temp_pl.pagetype = PAGETYPE_GENERIC;
  mng_OverrideToUnlocked(&temp_pl);
}

void WorldObjectsGenericDialog::onInvenSelectable(bool checked) {
  setFlag(OIF_INVEN_SELECTABLE, "IDC_GENERIC_INVEN_SELECTABLE", checked);
}
void WorldObjectsGenericDialog::onInvenNonuseable(bool checked) {
  setFlag(OIF_INVEN_NONUSEABLE, "IDC_GENERIC_INVEN_NONUSEABLE", checked);
}

void WorldObjectsGenericDialog::onKillfocusLodDistance() {
  if (m_current == -1)
    return;
  const float dist = find<QLineEdit>("IDC_LOD_DISTANCE_EDIT")->text().toFloat();
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
    Object_info[m_current].flags &= ~OIF_INVEN_TYPE_MISSION;
  updateDialog();
}
void WorldObjectsGenericDialog::onInvtypeMission() {
  if (m_current != -1)
    Object_info[m_current].flags |= OIF_INVEN_TYPE_MISSION;
  updateDialog();
}
void WorldObjectsGenericDialog::onInvenNoremove(bool checked) {
  setFlag(OIF_INVEN_NOREMOVE, "IDC_INVEN_NOREMOVE", checked);
}
void WorldObjectsGenericDialog::onInvenViswhenused(bool checked) {
  setFlag(OIF_INVEN_VISWHENUSED, "IDC_INVEN_VISWHENUSED", checked);
}

void WorldObjectsGenericDialog::onDeathPowerup1Changed() {
  if (m_current != -1)
    Object_info[m_current].dspew[0] =
        find<QComboBox>("IDC_DEATH_POWERUP1_PULLDOWN")->currentData().toInt();
}
void WorldObjectsGenericDialog::onDeathPowerup1NumEdited() {
  if (m_current != -1)
    Object_info[m_current].dspew_number[0] = find<QLineEdit>("IDC_DEATH_POWERUP1_NUM_EDIT")->text().toInt();
}
void WorldObjectsGenericDialog::onDeathPowerup1PercentEdited() {
  if (m_current != -1)
    Object_info[m_current].dspew_percent[0] =
        find<QLineEdit>("IDC_DEATH_POWERUP1_PERCENT_EDIT")->text().toFloat() / 100.0f;
}
void WorldObjectsGenericDialog::onDeathPowerup2Changed() {
  if (m_current != -1)
    Object_info[m_current].dspew[1] =
        find<QComboBox>("IDC_DEATH_POWERUP2_PULLDOWN")->currentData().toInt();
}
void WorldObjectsGenericDialog::onDeathPowerup2NumEdited() {
  if (m_current != -1)
    Object_info[m_current].dspew_number[1] = find<QLineEdit>("IDC_DEATH_POWERUP2_NUM_EDIT")->text().toInt();
}
void WorldObjectsGenericDialog::onDeathPowerup2PercentEdited() {
  if (m_current != -1)
    Object_info[m_current].dspew_percent[1] =
        find<QLineEdit>("IDC_DEATH_POWERUP2_PERCENT_EDIT")->text().toFloat() / 100.0f;
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
  const QByteArray text = find<QLineEdit>("IDC_SCRIPTNAME")->text().toLocal8Bit();
  if (strcmp(text.constData(), "null") == 0)
    Object_info[m_current].module_name[0] = '\0';
  else
    snprintf(Object_info[m_current].module_name, sizeof(Object_info[m_current].module_name), "%s", text.constData());
  updateDialog();
}

void WorldObjectsGenericDialog::onCompilemodule() {
  if (m_current == -1)
    return;
  QMessageBox::information(m_dialog, "Compile", "The quick compile dialog has not been ported yet.");
}

void WorldObjectsGenericDialog::onKillfocusScriptOverride() {
  if (m_current == -1)
    return;
  const QByteArray text = find<QLineEdit>("IDC_SCRIPT_OVERRIDE")->text().toLocal8Bit();
  if (strcmp(text.constData(), "null") == 0)
    Object_info[m_current].script_name_override[0] = '\0';
  else
    snprintf(Object_info[m_current].script_name_override, sizeof(Object_info[m_current].script_name_override), "%s",
             text.constData());
  updateDialog();
}

void WorldObjectsGenericDialog::onAiScriptedDeath(bool checked) {
  setFlag(OIF_AI_SCRIPTED_DEATH, "IDC_GENERIC_AI_SCRIPTED_DEATH", checked);
}

void WorldObjectsGenericDialog::onKillfocusRespawnScalar() {
  if (m_current == -1)
    return;
  float val = find<QLineEdit>("IDC_RESPAWN_SCALAR_EDIT")->text().toFloat();
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
  GenericDeathDialog dlg(&Object_info[m_current], m_dialog);
  dlg.exec();
}

void WorldObjectsGenericDialog::onKillfocusScore() {
  if (m_current != -1)
    Object_info[m_current].score = find<QLineEdit>("IDC_GENERIC_SCORE_EDIT")->text().toInt();
}
void WorldObjectsGenericDialog::onKillfocusAmmo() {
  if (m_current != -1)
    Object_info[m_current].ammo_count = find<QLineEdit>("IDC_GENERIC_AMMO_EDIT")->text().toInt();
}
void WorldObjectsGenericDialog::onObjCeiling(bool checked) {
  setFlag(OIF_DO_CEILING_CHECK, "IDC_OBJ_CEILING_CHECK", checked);
}
void WorldObjectsGenericDialog::onFlyThroughRenderedPortals(bool checked) {
  setFlag(OIF_IGNORE_FORCEFIELDS_AND_GLASS, "IDC_OBJECT_FLY_THROUGH_RENDERED_PORTALS", checked);
}
void WorldObjectsGenericDialog::onNsc(bool checked) {
  setFlag(OIF_NO_DIFF_SCALE_DAMAGE, "IDC_NSC_BUTTON", checked);
}
void WorldObjectsGenericDialog::onDsmpbd(bool checked) {
  setFlag(OIF_NO_DIFF_SCALE_MOVE, "IDC_DSMPBD_CHECK", checked);
}
void WorldObjectsGenericDialog::onAmbient(bool checked) {
  setFlag(OIF_AMBIENT_OBJECT, "IDC_GENERIC_AMBIENT", checked);
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

}
