/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral port of the Win32 editor's levelgoal.cpp.  Implements the subset
 * of the levelgoals/lgoal member functions needed by the level editor (name
 * lookups and introspection of the in-editor goal list).
 */

#include "levelgoal.h"

#include <string>

#include "string_helpers.h"

// Returns the goal's name.
std::string lgoal::GetName() const { return m_name; }

// Returns the name of goal goal_index, or an empty string on bad index.
std::string levelgoals::GoalGetName(int goal_index) const {
  if (goal_index < 0 || goal_index >= m_goal.size())
    return std::string();

  return m_goal[goal_index].GetName();
}

// Returns the index of the goal with the given name, or -1 if not found.
int levelgoals::GoalFindId(const std::string &goal_name) {
  for (int i = 0; i < (int)m_num_goals; i++) {
    if (match(GoalGetName(i), goal_name))
      return i;
  }

  return -1;
}
