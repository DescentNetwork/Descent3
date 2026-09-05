#pragma once

#include <QDialog>

#include "brief_model.h"

QT_BEGIN_NAMESPACE
namespace Ui { class BriefMainDialog; }
QT_END_NAMESPACE

// Port of CBriefEdit (IDD_BRIEF_MAIN): the briefing editor's main screen list.
class BriefMainDialog : public QDialog {
  Q_OBJECT
public:
  explicit BriefMainDialog(QWidget *parent = nullptr);
  ~BriefMainDialog();

private slots:
  void onScreenSelected(int index);
  void onScreenAdd();
  void onScreenEdit();
  void onScreenDelete();
  void onEffectText();
  void onEffectBitmap();
  void onEffectMovie();
  void onEffectSound();
  void onEffectButton();
  void onEffectEdit();
  void onEffectDelete();
  void onSave();
  void onLoad();
  void onManage();

private:
  Ui::BriefMainDialog *ui;

  // Index into the used-screen chain for the "current" screen (list order).
  int m_screen;
  // List order of the current screen's effects (root_effect is list head).
  int m_effect;

  void refreshScreenList();
  void refreshEffectList();
  void refreshGlobals();
  void applyGlobals();
  // The screen array index for the currently selected screen (or -1).
  int currentScreenIndex() const;
  // Allocates a new effect on the current screen; returns the array slot or -1.
  int allocateEffectOnCurrentScreen();
  void openEffectAdd(int effectType);
  void openEffectEdit(int arraySlot);
};
