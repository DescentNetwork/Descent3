#include "brief_main_dialog.h"

#include <QComboBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include "ui_brief_main.h"

#include "brief_bitmap_dialog.h"
#include "brief_button_dialog.h"
#include "brief_manage_dialog.h"
#include "brief_mission_flags_dialog.h"
#include "brief_movie_dialog.h"
#include "brief_screen_dialog.h"
#include "brief_sound_dialog.h"
#include "brief_text_edit_dialog.h"

BriefMainDialog::BriefMainDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::BriefMainDialog), m_screen(-1), m_effect(-1)
{
  ui->setupUi(this);
  BriefEditInitScreens();
  Briefing_globals = BriefGlobalValues{};

  connect(ui->IDC_BRIEF_SCREEN_LIST, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &BriefMainDialog::onScreenSelected);
  connect(ui->IDC_BRIEF_SCREEN_ADD, &QPushButton::clicked, this, &BriefMainDialog::onScreenAdd);
  connect(ui->IDC_BRIEF_SCREEN_EDIT, &QPushButton::clicked, this, &BriefMainDialog::onScreenEdit);
  connect(ui->IDC_BRIEF_SCREEN_DEL, &QPushButton::clicked, this, &BriefMainDialog::onScreenDelete);
  connect(ui->IDC_BRIEF_EFFECT_TEXT, &QPushButton::clicked, this, &BriefMainDialog::onEffectText);
  connect(ui->IDC_BRIEF_EFFECT_BITMAP, &QPushButton::clicked, this, &BriefMainDialog::onEffectBitmap);
  connect(ui->IDC_BRIEF_EFFECT_MOVIE, &QPushButton::clicked, this, &BriefMainDialog::onEffectMovie);
  connect(ui->IDC_BRIEF_EFFECT_SOUND, &QPushButton::clicked, this, &BriefMainDialog::onEffectSound);
  connect(ui->IDC_BRIEF_EFFECT_BUTTON, &QPushButton::clicked, this, &BriefMainDialog::onEffectButton);
  connect(ui->IDC_BRIEF_EFFECT_EDIT, &QPushButton::clicked, this, &BriefMainDialog::onEffectEdit);
  connect(ui->IDC_BRIEF_EFFECT_DEL, &QPushButton::clicked, this, &BriefMainDialog::onEffectDelete);
  connect(ui->IDC_BRIEF_SAVE, &QPushButton::clicked, this, &BriefMainDialog::onSave);
  connect(ui->IDC_LOAD, &QPushButton::clicked, this, &BriefMainDialog::onLoad);
  connect(ui->IDC_MANAGE, &QPushButton::clicked, this, &BriefMainDialog::onManage);
  connect(ui->IDC_BRIEF_TITLE, &QLineEdit::editingFinished, this, &BriefMainDialog::applyGlobals);
  connect(ui->IDC_BRIEF_STATIC, &QLineEdit::editingFinished, this, &BriefMainDialog::applyGlobals);
  connect(ui->IDC_BRIEF_GLITCH, &QLineEdit::editingFinished, this, &BriefMainDialog::applyGlobals);

  refreshScreenList();
  refreshGlobals();
}

BriefMainDialog::~BriefMainDialog() {
  BriefEditFreeScreens();
  delete ui;
}

int BriefMainDialog::currentScreenIndex() const {
  // The screen list is populated in chain order (root then next...).
  int node = Briefing_root_screen;
  int idx = 0;
  while (node != -1) {
    if (idx == m_screen)
      return node;
    node = Briefing_screens[node].next;
    idx++;
  }
  return -1;
}

void BriefMainDialog::refreshScreenList() {
  auto *combo = ui->IDC_BRIEF_SCREEN_LIST;
  {
    QSignalBlocker blocker(combo);
    combo->clear();
    int node = Briefing_root_screen;
    int idx = 0;
    while (node != -1 && idx < MAX_TELCOM_SCREENS) {
      if (Briefing_screens[node].used)
        combo->addItem(QStringLiteral("Screen %1").arg(idx));
      node = Briefing_screens[node].next;
      idx++;
    }
  }
  if (m_screen < 0)
    m_screen = 0;
  combo->setCurrentIndex(m_screen);
  refreshEffectList();
}

void BriefMainDialog::refreshEffectList() {
  auto *combo = ui->IDC_BRIEF_EFFECT_LIST;
  {
    QSignalBlocker blocker(combo);
    combo->clear();
    int scr = currentScreenIndex();
    if (scr == -1)
      return;
    int efx = Briefing_screens[scr].root_effect;
    int idx = 0;
    while (efx != -1 && idx < MAX_EFFECTS_PER_SCREEN) {
      const tBriefEffect *e = &Briefing_screens[scr].effects[efx];
      QString label;
      switch (e->type) {
      case BE_TEXT: label = "Text"; break;
      case BE_BMP: label = "Bitmap"; break;
      case BE_MOVIE: label = "Movie"; break;
      case BE_SND: label = "Sound"; break;
      case BE_BUTTON: label = "Button"; break;
      default: label = "None"; break;
      }
      if (!e->description.empty())
        label += QStringLiteral(": %1").arg(QString::fromStdString(e->description));
      combo->addItem(label);
      efx = e->next;
      idx++;
    }
  }
  m_effect = combo->currentIndex();
}

void BriefMainDialog::refreshGlobals() {
  ui->IDC_BRIEF_TITLE->setText(QString::fromStdString(Briefing_globals.title));
  ui->IDC_BRIEF_STATIC->setText(QString::number(Briefing_globals.static_val));
  ui->IDC_BRIEF_GLITCH->setText(QString::number(Briefing_globals.glitch_val));
}

void BriefMainDialog::applyGlobals() {
  Briefing_globals.title = ui->IDC_BRIEF_TITLE->text().toStdString();
  Briefing_globals.static_val = ui->IDC_BRIEF_STATIC->text().toFloat();
  Briefing_globals.glitch_val = ui->IDC_BRIEF_GLITCH->text().toFloat();
}

void BriefMainDialog::onScreenSelected(int index) {
  if (index < 0)
    return;
  m_screen = index;
  refreshEffectList();
}

void BriefMainDialog::onScreenAdd() {
  BriefScreenDialog dlg(this);
  if (dlg.exec() != QDialog::Accepted)
    return;

  int slot = -1;
  for (int i = 0; i < MAX_TELCOM_SCREENS; i++) {
    if (!Briefing_screens[i].used) {
      slot = i;
      break;
    }
  }
  if (slot == -1) {
    QMessageBox::information(this, "Error", "Out of screens.");
    return;
  }

  Briefing_screens[slot].used = true;
  Briefing_screens[slot].layout = dlg.layout();
  // Append to the screen chain.
  if (Briefing_root_screen == -1) {
    Briefing_root_screen = slot;
    Briefing_screens[slot].prev = Briefing_screens[slot].next = -1;
  } else {
    int node = Briefing_root_screen;
    while (Briefing_screens[node].next != -1)
      node = Briefing_screens[node].next;
    Briefing_screens[node].next = slot;
    Briefing_screens[slot].prev = node;
    Briefing_screens[slot].next = -1;
  }
  refreshScreenList();
}

void BriefMainDialog::onScreenEdit() {
  int scr = currentScreenIndex();
  if (scr == -1) {
    QMessageBox::information(this, "Error", "No screens to edit.");
    return;
  }
  BriefScreenDialog dlg(this);
  dlg.setLayout(Briefing_screens[scr].layout);
  if (dlg.exec() != QDialog::Accepted)
    return;
  Briefing_screens[scr].layout = dlg.layout();
  refreshScreenList();
}

void BriefMainDialog::onScreenDelete() {
  int scr = currentScreenIndex();
  if (scr == -1)
    return;
  if (QMessageBox::question(this, "Confirmation", "Are you sure you want to delete this screen?") != QMessageBox::Yes)
    return;

  int next = Briefing_screens[scr].next;
  int prev = Briefing_screens[scr].prev;
  if (prev == -1)
    Briefing_root_screen = next;
  else
    Briefing_screens[prev].next = next;
  if (next != -1)
    Briefing_screens[next].prev = prev;

  BriefEditFreeScreen(&Briefing_screens[scr]);
  m_screen = 0;
  refreshScreenList();
}

// Allocates a new effect on the current screen; returns the array slot or -1.
int BriefMainDialog::allocateEffectOnCurrentScreen() {
  int scr = currentScreenIndex();
  if (scr == -1)
    return -1;
  // Find a free effect slot.
  for (int i = 0; i < MAX_EFFECTS_PER_SCREEN; i++) {
    if (!Briefing_screens[scr].effects[i].used) {
      tBriefEffect *efx = &Briefing_screens[scr].effects[i];
      efx->init();
      efx->used = true;
      if (Briefing_screens[scr].root_effect == -1) {
        Briefing_screens[scr].root_effect = i;
      } else {
        efx->next = Briefing_screens[scr].root_effect;
        Briefing_screens[scr].effects[Briefing_screens[scr].root_effect].prev = i;
        Briefing_screens[scr].root_effect = i;
      }
      return i;
    }
  }
  return -1;
}

void BriefMainDialog::openEffectAdd(int effectType) {
  int scr = currentScreenIndex();
  if (scr == -1) {
    QMessageBox::information(this, "Error", "No screens to add an effect to.");
    return;
  }
  int slot = allocateEffectOnCurrentScreen();
  if (slot == -1)
    return;
  tBriefEffect *efx = &Briefing_screens[scr].effects[slot];
  efx->type = effectType;

  switch (effectType) {
  case BE_TEXT: {
    BriefTextEditDialog dlg(scr, nullptr, std::string(), 0, this);
    if (dlg.exec() == QDialog::Accepted) {
      efx->desc.text_desc = dlg.result();
      efx->description = dlg.descText().toStdString();
      efx->id = dlg.id();
      efx->text = dlg.textBuffer();
    } else {
      BriefEditFreeEffect(efx);
    }
  } break;
  case BE_BMP: {
    BriefBitmapDialog dlg(nullptr, this);
    if (dlg.exec() == QDialog::Accepted) {
      efx->desc.bmp_desc = dlg.result();
      efx->description = dlg.description().toStdString();
    } else {
      BriefEditFreeEffect(efx);
    }
  } break;
  case BE_MOVIE: {
    BriefMovieDialog dlg(nullptr, this);
    if (dlg.exec() == QDialog::Accepted) {
      efx->desc.movie_desc = dlg.result();
      efx->description = dlg.description().toStdString();
    } else {
      BriefEditFreeEffect(efx);
    }
  } break;
  case BE_SND: {
    BriefSoundDialog dlg(nullptr, this);
    if (dlg.exec() == QDialog::Accepted) {
      efx->desc.snd_desc = dlg.result();
      efx->description = dlg.description().toStdString();
    } else {
      BriefEditFreeEffect(efx);
    }
  } break;
  case BE_BUTTON: {
    BriefButtonDialog dlg(nullptr, this);
    if (dlg.exec() == QDialog::Accepted) {
      efx->desc.button_desc = dlg.result();
      efx->description = dlg.buttonDescription().toStdString();
    } else {
      BriefEditFreeEffect(efx);
    }
  } break;
  default:
    break;
  }

  refreshEffectList();
}

void BriefMainDialog::openEffectEdit(int arraySlot) {
  int scr = currentScreenIndex();
  if (scr == -1 || arraySlot < 0)
    return;
  tBriefEffect *efx = &Briefing_screens[scr].effects[arraySlot];
  switch (efx->type) {
  case BE_TEXT: {
    BriefTextEditDialog dlg(scr, &efx->desc.text_desc, efx->text, efx->id, this);
    if (dlg.exec() == QDialog::Accepted) {
      efx->desc.text_desc = dlg.result();
      efx->description = dlg.descText().toStdString();
      efx->id = dlg.id();
      efx->text = dlg.textBuffer();
    }
  } break;
  case BE_BMP: {
    BriefBitmapDialog dlg(&efx->desc.bmp_desc, this);
    if (dlg.exec() == QDialog::Accepted) {
      efx->desc.bmp_desc = dlg.result();
    }
  } break;
  case BE_MOVIE: {
    BriefMovieDialog dlg(&efx->desc.movie_desc, this);
    if (dlg.exec() == QDialog::Accepted) {
      efx->desc.movie_desc = dlg.result();
    }
  } break;
  case BE_SND: {
    BriefSoundDialog dlg(&efx->desc.snd_desc, this);
    if (dlg.exec() == QDialog::Accepted) {
      efx->desc.snd_desc = dlg.result();
    }
  } break;
  case BE_BUTTON: {
    BriefButtonDialog dlg(&efx->desc.button_desc, this);
    if (dlg.exec() == QDialog::Accepted) {
      efx->desc.button_desc = dlg.result();
    }
  } break;
  default:
    break;
  }
  refreshEffectList();
}

void BriefMainDialog::onEffectText() { openEffectAdd(BE_TEXT); }
void BriefMainDialog::onEffectBitmap() { openEffectAdd(BE_BMP); }
void BriefMainDialog::onEffectMovie() { openEffectAdd(BE_MOVIE); }
void BriefMainDialog::onEffectSound() { openEffectAdd(BE_SND); }
void BriefMainDialog::onEffectButton() { openEffectAdd(BE_BUTTON); }

void BriefMainDialog::onEffectEdit() {
  int scr = currentScreenIndex();
  if (scr == -1) {
    QMessageBox::information(this, "Error", "No screens.");
    return;
  }
  // m_effect is the list order; walk the chain to the array slot.
  int node = Briefing_screens[scr].root_effect;
  int ord = 0;
  while (node != -1) {
    if (ord == m_effect) {
      openEffectEdit(node);
      return;
    }
    node = Briefing_screens[scr].effects[node].next;
    ord++;
  }
}

void BriefMainDialog::onEffectDelete() {
  int scr = currentScreenIndex();
  if (scr == -1)
    return;
  if (QMessageBox::question(this, "Confirmation", "Are you sure you want to delete this effect?") != QMessageBox::Yes)
    return;
  int node = Briefing_screens[scr].root_effect;
  int ord = 0;
  while (node != -1) {
    if (ord == m_effect) {
      int prev = Briefing_screens[scr].effects[node].prev;
      int next = Briefing_screens[scr].effects[node].next;
      if (prev == -1)
        Briefing_screens[scr].root_effect = next;
      else
        Briefing_screens[scr].effects[prev].next = next;
      if (next != -1)
        Briefing_screens[scr].effects[next].prev = prev;
      BriefEditFreeEffect(&Briefing_screens[scr].effects[node]);
      break;
    }
    node = Briefing_screens[scr].effects[node].next;
    ord++;
  }
  m_effect = 0;
  refreshEffectList();
}

void BriefMainDialog::onSave() {
  QString name = QFileDialog::getSaveFileName(this, "Save Briefing", QString(), "Briefing (*.brf)");
  if (name.isEmpty())
    return;
  applyGlobals();
  if (!BriefEditSaveScreens(std::filesystem::path(name.toStdString()), &Briefing_globals))
    QMessageBox::information(this, "Error", "Could not save briefing.");
}

void BriefMainDialog::onLoad() {
  QString name = QFileDialog::getOpenFileName(this, "Load Briefing", QString(), "Briefing (*.brf)");
  if (name.isEmpty())
    return;
  if (!BriefEditLoadScreens(std::filesystem::path(name.toStdString()), &Briefing_globals)) {
    QMessageBox::information(this, "Error", "Could not load briefing.");
    return;
  }
  m_screen = 0;
  m_effect = 0;
  refreshScreenList();
  refreshGlobals();
}

void BriefMainDialog::onManage() {
  BriefManageDialog dlg(this);
  dlg.exec();
}
