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

// Returns the index of the goal with the given name, or nullopt if not found.
std::optional<uint32_t> levelgoals::GoalFindId(const std::string &goal_name) {
  for (int i = 0; i < (int)m_num_goals; i++) {
    if (match(GoalGetName(i), goal_name))
      return static_cast<uint32_t>(i);
  }

  return std::nullopt;
}

// Returns the number of goals.
int levelgoals::GetNumGoals() { return m_num_goals; }

// Discards all level goals and their item tables (the engine's DeleteGoal
// loop; the mini's values live in std::string/std::array members).
void levelgoals::CleanupAfterLevel() {
  for (lgoal &g : m_goal)
    g = lgoal();

  m_num_goals = 0;
  m_num_active_primaries = 0;
  m_num_active_secondaries = 0;
  m_flags = 0;
}

// ---------------------------------------------------------------------------
// LVLG chunk serialization.  Ported from the engine's
// levelgoals::SaveLevelGoalInfo / LoadLevelGoalInfo (levelgoal.cpp:730 / :826).
// The engine reads/writes each goal's status (LGF_* flags), priority, goal
// list, the four C-string fields (name/item/desc/completion message) as
// short-length-prefixed byte runs (no NUL), the item table, then the level's
// own flag word.  The mini's m_name/m_item_name/m_desc/m_completion_message
// are std::strings (the engine uses malloc'd char*), which makes the len
// reads overflow-safe.
// ---------------------------------------------------------------------------

namespace {
constexpr int32_t LEVEL_GOAL_VERSION = 4;

// Reads a short-length-prefixed string (length then that many raw bytes,
// consuming exactly the declared bytes).
std::string readGoalString(posix_istream &ifile) {
  int16_t len = 0;
  ifile >> len;

  if (len <= 0)
    return std::string();

  std::string s(static_cast<size_t>(len), '\0');
  ifile.read(&s[0], len);
  return s;
}

// Writes a short-length-prefixed string matching the engine's strlen+bytes.
void writeGoalString(posix_ostream &ofile, const std::string &s) {
  ofile << static_cast<int16_t>(static_cast<int>(s.size()));
  if (!s.empty())
    ofile.write(s.data(), s.size());
}
} // namespace

bool levelgoals::SaveLevelGoalInfo(posix_ostream &ofile) const {
  ofile << static_cast<int16_t>(LEVEL_GOAL_VERSION);
  ofile << static_cast<int16_t>(m_num_goals);

  for (int i = 0; i < m_num_goals; i++) {
    const lgoal &g = m_goal[i];

    ofile << static_cast<int32_t>(g.m_flags);
    ofile << static_cast<int32_t>(g.m_priority);
    ofile << static_cast<int8_t>(g.m_g_list);

    writeGoalString(ofile, g.m_name);
    writeGoalString(ofile, g.m_item_name);
    writeGoalString(ofile, g.m_desc);
    writeGoalString(ofile, g.m_completion_message);

    ofile << static_cast<int16_t>(g.m_num_items);
    for (int j = 0; j < g.m_num_items; j++) {
      const litem &it = g.m_item[j];
      ofile << static_cast<int8_t>(it.m_type);
      ofile << static_cast<int32_t>(it.m_handle);
      ofile << static_cast<int8_t>(it.m_f_done ? 1 : 0);
    }
  }

  ofile << static_cast<int32_t>(m_flags);

  return true;
}

bool levelgoals::LoadLevelGoalInfo(posix_istream &ifile) {
  // Clean-slate reset (mirrors the mini's CleanupAfterLevel()).
  CleanupAfterLevel();

  int16_t version = 0;
  ifile >> version;
  int16_t num_goals = 0;
  ifile >> num_goals;
  if (num_goals < 0)
    num_goals = 0;
  if (num_goals > MAX_LEVEL_GOALS)
    num_goals = MAX_LEVEL_GOALS; // corrupt count; rest skipped by the framer

  for (int i = 0; i < num_goals; i++) {
    lgoal &g = m_goal[i];
    g = lgoal();

    int32_t status = 0;
    ifile >> status;
    g.m_flags = static_cast<uint32_t>(status);

    ifile >> g.m_priority;

    if (version >= 4) {
      int8_t g_list = 0;
      ifile >> g_list;
      g.m_g_list = g_list;
    } else {
      g.m_g_list = 0;
    }

    g.m_name = readGoalString(ifile);
    g.m_item_name = readGoalString(ifile);
    g.m_desc = readGoalString(ifile);

    if (version >= 3) {
      g.m_completion_message = readGoalString(ifile);
    } else {
      g.m_completion_message.clear();
    }

    int16_t num_items = 0;
    ifile >> num_items;
    if (num_items < 0)
      num_items = 0;
    if (num_items > MAX_GOAL_ITEMS)
      num_items = MAX_GOAL_ITEMS;

    g.m_item = std::array<litem, MAX_GOAL_ITEMS>{};
    for (int j = 0; j < num_items; j++) {
      int8_t type = 0;
      ifile >> type;
      litem &it = g.m_item[j];
      it.m_type = type;
      ifile >> it.m_handle;
      int8_t done = 0;
      ifile >> done;
      it.m_f_done = (done != 0);
    }
    g.m_num_items = num_items;
  }

  m_num_goals = num_goals;

  if (version >= 2) {
    int32_t lvl_flags = 0;
    ifile >> lvl_flags;
    m_flags = static_cast<uint32_t>(lvl_flags);
  } else {
    m_flags = 0;
  }

  return true;
}
