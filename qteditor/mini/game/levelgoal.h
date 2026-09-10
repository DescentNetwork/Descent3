/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
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

#ifndef _LEVELGOAL_H_
#define _LEVELGOAL_H_

#include <cstdint>
#include <array>
#include <string>
#include "object.h"
#include "cfile.h"
#include <cstdlib>
#include "mem/mem.h"
#include "levelgoal_external.h"

class litem {
public:
  litem() {
    m_type = LIT_OBJECT;
    m_handle = OBJECT_HANDLE_NONE;
    m_f_done = false;
  };

  char m_type;
  int m_handle;
  bool m_f_done;
};

class lgoal {
private:
  std::string m_name;
  std::string m_item_name;
  std::string m_desc;
  std::string m_completion_message;

  int m_num_items;
  std::array<litem, MAX_GOAL_ITEMS> m_item;

  char m_g_list;
  char m_modified;
  bool m_goal_completed; // since flags in a multiplayer game (client) get set to 0, then
                         // reset, we need to use this to store whether the goal has
                         // been completed, else we *possibly* can complete the goal more than
                         // once.
  int m_priority;
  uint32_t m_flags;

  void GoalComplete(int handle, bool announce);
  void SendMultiUpdate(int handle);

public:
  lgoal() {
    m_num_items = 0;
    m_priority = 0;
    m_g_list = 0;
    m_flags = LGF_ENABLED | LGF_TELCOM_LISTS;
    m_modified = 0;
    m_goal_completed = false;
  };

  int AddItem();
  bool DeleteItem(int index);
  bool ItemInfo(int index, char operation, char *type, int *handle, bool *done);

  void Reset(bool f_from_editor);

  bool SetName(int handle, const std::string &name);
  bool SetItemName(const std::string &iname);
  bool SetDesc(const std::string &desc);
  bool SetCompletionMessage(const std::string &message);

  std::string GetName() const;
  std::string GetItemName() const;
  std::string GetDesc() const;
  std::string GetCompletionMessage() const;

  bool Priority(int handle, char operation, int *value);
  bool GoalList(char operation, int8_t *value);
  bool GetStatus(int handle, char operation, int *value, bool f_save_load = false, bool announce = true);

  void SendStateToPlayer(int index, int pnum);
  void ResetModified(void);

  int GetNumItems();
};

// Goals are sorted by priority and completion
class levelgoals {
private:
  int m_num_goals;
  std::array<lgoal, MAX_LEVEL_GOALS> m_goal;
  int m_flags;

  int m_num_active_primaries;
  std::array<int, MAX_LEVEL_GOALS> m_active_primaries;
  int m_num_active_secondaries;
  std::array<int, MAX_LEVEL_GOALS> m_active_secondaries;

public:
  levelgoals() {
    m_num_goals = 0;
    m_num_active_primaries = 0;
    m_num_active_secondaries = 0;
    m_flags = 0;
  };

  int AddGoal(bool f_from_editor);
  bool DeleteGoal(int index);

  int GoalAddItem(int goal_index);
  bool GoalDeleteItem(int goal_index, int item_index);
  bool GoalItemInfo(int goal_index, int index, char operation, char *type, int *handle, bool *done);

  bool GoalSetName(int goal_index, const std::string &name);
  bool GoalSetItemName(int goal_index, const std::string &iname);
  bool GoalSetDesc(int goal_index, const std::string &desc);
  bool GoalSetCompletionMessage(int goal_index, const std::string &message);

  std::string GoalGetName(int goal_index) const;
  std::string GoalGetItemName(int goal_index) const;
  std::string GoalGetDesc(int goal_index) const;
  std::string GoalGetCompletionMessage(int goal_index) const;

  bool GoalPriority(int goal_index, char operation, int *value);
  bool GoalGoalList(int goal_index, char operation, int8_t *value);
  bool GoalStatus(int goal_index, char operation, int *value, bool announce = true);

  int GoalFindId(const std::string &goal_name);

  int GoalGetNumItems(int goal_index);

  int GetNumGoals();

  bool SaveLevelGoalInfo(struct CFILE* fptr);
  bool LoadLevelGoalInfo(struct CFILE* fptr);

  bool LGStatus(char operation, int *value);

  int GetNumActivePrimaryGoals();
  int GetActivePrimaryGoal(int p_index);

  int GetNumActiveSecondaryGoals();
  int GetActiveSecondaryGoal(int s_index);

  void DoFrame();
  void CleanupAfterLevel();
  void InitLevel();
  void Inform(char type, int comp_type, int handle);

  void ResetModifiedFlags(void);
  void MultiSendChangedGoals(int pnum);
};

extern levelgoals Level_goals;

#endif
