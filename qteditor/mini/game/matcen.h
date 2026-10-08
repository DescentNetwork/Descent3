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

#ifndef _MATCEN_H_
#define _MATCEN_H_

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include "posix_stream.h"
#include "vecmat.h"
#include "matcen_external.h"
#include "utils.h"

#define MAX_MATCENS 60
#define MAX_MATCEN_NAME_LEN 32

#define MATCEN_LOADSAVE_VERSION 3

#define MAX_MATCEN_ALIVE_CHILDREN 32

extern bool Matcen_created;

#define MATCEN_OUTSIDE_NEAR_DIST 150.0f

#define MATCEN_ACTIVE_CHECK_RATE 4.0f
#define MATCEN_ACTIVE_CHECK_VARIENCE 1.0f

#define CHECK_ACTIVE_RATE 3.0f
#define CHECK_ACTIVE_VARIENCE 1.0f

#define MAX_MATCEN_EFFECT_SATURATION 2

#ifdef EDITOR
extern char *MatcenEffectStrings[static_cast<size_t>(matcen_effect::count)];
#endif

// Versions
// 1 - Initial
// 2 - Added matcen effects
// 3 - Added matcen active_sound_handle

class matcen {
private:
  // Static data -- only changes by OSIRIS
  std::string m_name;

  char m_num_prod_types;
  matcen_control_type m_control_type;
  matcen_type m_type;
  matcen_effect m_creation_effect;
  int16_t m_creation_texture;
  uint8_t m_cur_saturation_count;

  int m_num_spawn_pnts;

  union {
    int m_roomnum;
    int m_objref;
  };

  vector3 m_create_pnt;
  int m_create_room;

  std::vector<index_t> m_spawn_pnt;
  std::vector<vector3> m_spawn_vec;
  std::vector<vector3> m_spawn_normal;
  std::array<std::array<int16_t, MAX_SPAWN_PNTS>, MAX_MATCEN_EFFECT_SATURATION> m_spawn_vis_effects;

  int m_max_prod;

  std::vector<index_t> m_prod_type;
  std::vector<float> m_prod_time;
  std::vector<int> m_prod_priority;
  std::vector<int> m_max_prod_type;

  int16_t m_max_alive_children;
  int16_t m_num_alive;
  std::vector<int> m_alive_list; // list of alive children

  float m_preprod_time;
  float m_postprod_time;

  std::vector<index_t> m_sounds;

  float m_speed_multi;

  // Dynamic values that change without scripting
  matcen_prod_mode m_prod_mode;
  float m_prod_mode_time;

  matcen_status_flags_t m_status;

  int m_num_prod;
  int m_last_prod_type_index;
  float m_last_prod_finish_time;

  int m_cached_prod_index;
  float m_cached_prod_time;

  int m_sound_active_handle;

  float m_next_active_check_time;
  bool m_last_active_check_result;

  int m_last_prod_objref;

  std::vector<int> m_num_prod_type;

  // Private functions that are not available outside of the matcen internals
  bool StartObjProd();
  bool DoObjProd();
  bool FinishObjProd();

  bool ComputeNextProdInfo();
  bool ComputeCreatePnt();

  void CheckActivateStatus();
  bool DoAliveListFrame();
  bool AddToAliveList(int objref);

public:
  matcen();
  ~matcen();

  void SetCreationTexture(int16_t texnum);
  int16_t GetCreationTexture();

  char GetAttachType();
  bool SetAttachType(char type);

  char GetControlType();
  bool SetControlType(char type);

  int GetAttach();
  bool SetAttach(int attach);

  bool GetCreatePnt(vector3 *pnt);
  bool SetCreatePnt(vector3 *pnt);
  int GetCreateRoom();
  bool SetCreateRoom(int room);

  char GetNumSpawnPnts();
  bool SetNumSpawnPnts(char num_s);

  int GetSpawnPnt(int8_t s_index);
  bool SetSpawnPnt(int8_t s_index, int s_value);

  void SaveData(posix_ostream &ofile) const;
  void LoadData(posix_istream &ifile, const int *texture_xlate);

  std::string GetName(void);
  bool SetName(const std::string& name);

  int GetMaxProd();
  bool SetMaxProd(int max_p);

  char GetNumProdTypes();
  bool SetNumProdTypes(char num_prod_types);

  bool GetProdInfo(int8_t index, int *type_id, int *priority, float *time, int *max_prod);
  bool SetProdInfo(int8_t index, int *type_id, int *priority, float *time, int *max_prod);

  float GetProdMultiplier();
  bool SetProdMultiplier(float multi);

  int GetStatus();
  bool SetStatus(int status, bool f_enable); // Not all flags are settable

  void DoThinkFrame();
  void DoRenderFrame();

  char GetCreationEffect();
  bool SetCreationEffect(char effect_index);

  int GetMaxAliveChildren();
  bool SetMaxAliveChildren(int max_alive);

  float GetPreProdTime();
  bool SetPreProdTime(float time);

  float GetPostProdTime();
  bool SetPostProdTime(float time);

  int GetSound(int8_t sound_type);
  bool SetSound(int8_t sound_type, int sound_index);

  void Reset();
};

extern std::vector<matcen> Matcen;
int FindMatcenIndex(const std::string &name);
int CreateMatcen(const std::string &name, bool &f_name_changed);
void InitMatcens();

void DestroyAllMatcens();

void DestroyMatcen(int32_t id, bool f_resort);

bool MatcenValid(int32_t id);

void DoMatcensFrame();
void DoMatcensRenderFrame();
void InitMatcensForLevel();

#endif
