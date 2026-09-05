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

#include <cstring>

// Returns the goal's name into the supplied buffer.
int lgoal::GetName(char *name, int buffer_size) {
  if (!name)
    return 0;

  if (buffer_size <= 0)
    return m_name ? static_cast<int>(strlen(m_name)) : 0;

  if (m_name == NULL) {
    name[0] = '\0';
    return 0;
  }

  strncpy(name, m_name, buffer_size);
  name[buffer_size - 1] = '\0';

  return strlen(m_name);
}

// Returns the name of goal goal_index, or -1 on bad index.
int levelgoals::GoalGetName(int goal_index, char *name, int buffer_size) {
  if (goal_index < 0 || goal_index >= m_num_goals)
    return -1;

  return m_goal[goal_index].GetName(name, buffer_size);
}

// Returns the index of the goal with the given name, or -1 if not found.
int levelgoals::GoalFindId(const char *goal_name) {
  char cur_name[200];

  for (int i = 0; i < m_num_goals; i++) {
    GoalGetName(i, cur_name, sizeof(cur_name));
    if (strcasecmp(cur_name, goal_name) == 0)
      return i;
  }

  return -1;
}
