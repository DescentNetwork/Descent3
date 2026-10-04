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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/main/aistruct_external.h $
 * $Revision: 29 $
 * $Date: 10/23/99 2:43a $
 * $Author: Chris $
 *
 * Description goes here
 *
 * $Log: /DescentIII/main/aistruct_external.h $
 *
 * 29    10/23/99 2:43a Chris
 * Added the PutObjectOnObject AI Goal
 *
 * 28    10/17/99 11:58p Chris
 * Improved the hearing/seeing code, Added AIN_FIRED_WEAPON notify, and
 * added a *hack* so that the final boss could fire his weapons during a
 * cutscene...  (usually this is a no-no).
 *
 * 27    5/05/99 4:56p Chris
 * Added the RType powerup for the GB
 *
 * 26    5/01/99 2:21a Chris
 * Use the GF_SPEED_XXX  stuff
 *
 * 25    4/30/99 2:47p Chris
 * Added the GF_SPEED stuff
 *
 * 24    4/28/99 5:33a Chris
 * Further improved the BNode system.  (It actually works inside and
 * outside now)
 *
 * 23    4/24/99 2:20a Chris
 * Added the Neutral_till_hit flag
 *
 * 22    4/18/99 7:27a Chris
 * Added AIAF_IMMEDIATE to force non-tiling and instant anim loop changes
 *
 * 21    4/09/99 10:33a Chris
 * Improvements to Freud
 *
 * 20    3/30/99 4:32p Chris
 * Moved subtype to the main goal sturct (from goal_info).  Made move
 * relative object vec finishable.  (Like get behind player)
 *
 * 19    3/27/99 3:25p Chris
 * Added comment headers
 *
 * $NoKeywords: $
 */

#ifndef _AISTRUCT_EXTERNAL_H_
#define _AISTRUCT_EXTERNAL_H_

#include <bit>
#include <cstdint>

// AI Type -- make sure to update MAX_AI_TYPES if you add a new class

#define AIT_FLYLANDER 0
#define AIT_STALKER 1
#define AIT_EVADER1 2
#define AIT_EVADER2 3
#define AIT_STATIONARY_TURRET 4
#define AIT_AIS 5
#define AIT_MELEE1 6
#define AIT_BIRD_FLOCK1 7
#define AIT_HERD1 8

// AI Movement flying types

#define AIMF_NORMAL 0
#define AIMF_PATH 1
#define AIMF_HELICOPTER 2
#define AIMF_HOVERCRAFT 3
#define AIMF_JET 4
#define AIMF_PLAYERLIKE 5 // For NPC ships
#define AIMF_BUDDYLIKE 6  // For theif/buddy bots

//-------------------------------------------------
// AI Movement Information
//-------------------------------------------------

// AI Movement walking types

#define AIMW_RESTRICTED_FLAT                                                                                           \
  0 // Specify a min/max angle of incline (so we can restrict movement -- i.e we can even do
    // ceiling only robots)
#define AIMW_RESTRICTED_LOW_ANGLE 1
#define AIMW_RESTRICTED_HIGH_ANGLE 2
#define AIMW_NON_RESTRICTED 3
#define AIMW_UNDERWATERONLY 4 // Stays in water
#define AIMW_WATERSURFACE 5

//-------------------------------------------------
// AI Animation Info
//-------------------------------------------------

// animation flags

#define AIAF_LOOPING 1
#define AIAF_NOTIFY 2
#define AIAF_UPDATE_WBS 4
#define AIAF_USE_SPEED 8
#define AIAF_IMMEDIATE 16 // Force to not tile and set immediately

#define AIPM_ORIENT 0x00000018       // Masks for Orientation
#define AIPF_NEXT_NODE 0x00000000    // Orientate to velocity
#define AIPF_VELOCITY 0x00000008     // No automatic updating of orientation
#define AIPF_NODE_ORIENT 0x00000010  // Node orientate
#define AIPF_USER_DEFINED 0x00000018 // Always face the next node

//-------------------------------------------------
// AI Goal Enders
//-------------------------------------------------

#define MAX_ENABLERS_PER_GOAL 5 // We might malloc this depending of space and varitions between AIs

// Ender types

/*
#define AIE_NOT_USED					0
#define AIE_PLAYER_DEAD				1
#define AIE_PLAYER_AWAY				2
#define AIE_SEE_OBJ					3
#define AIE_TOUCH_OBJ				5
#define AIE_HIDE_FROM_OBJ			6
#define AIE_NOT_COLLIDE_VECTOR	7  // OBJ and sig.  make sure the dead/new objects are not avoided.
#define AIE_TIME_TILL_GOAL_KILL	8
#define AIE_MOVEMENT_TYPE			9
#define AIE_AFTER_TASK				10 // Endable goals (like touch obj, fire on obj)
#define AIE_POSITION					11
#define AIE_VELOCITY					12
#define AIE_NOT_MOVEMENT_TYPE		13
#define AIE_ADD_GOAL_TYPE			14
#define AIE_FUZZY_TIME				15
#define AIE_DIFF_ROOM				16  // On the terrain it means a distance of more than ???
#define AIE_SAME_ROOM				18
*/

#define AIE_NEAR 1
#define AIE_FAR 2
#define AIE_LTE_AWARENESS 3
#define AIE_GT_AWARENESS 4
#define AIE_LTE_LAST_SEE_TARGET_INTERVAL 5
#define AIE_GT_LAST_SEE_TARGET_INTERVAL 6
#define AIE_AI_STATUS_FLAG 7
#define AIE_SCRIPTED 8
#define AIE_FEAR 9
#define AIE_ANGRY 10
#define AIE_CURIOUS 11
#define AIE_FRUSTRATED 12
#define AIE_DELAY_TIME 13
#define AIE_CLEAR_TIME 14

// How are the ending conditions related (BOOLEAN INFO)

#define ENABLER_AND_NEXT 0
#define ENABLER_OR_NEXT 1
#define ENABLER_XOR_NEXT 3
#define ENABLER_NEW_BOOL_NEXT 4

// Maximum goals a robot can have at any given time
#define MAX_GOALS 10

// AI Goal Types
#define AIG_GET_AWAY_FROM_OBJ 0x00000001 // Distance
#define AIG_HIDE_FROM_OBJ 0x00000002     // Line of sight
#define AIG_GET_TO_OBJ 0x00000004        // Butt up to an object
#define AIG_ATTACH_TO_OBJ 0x00000008     // Within a distance from obj
#define AIG_FIRE_AT_OBJ 0x00000010
#define AIG_MELEE_TARGET 0x00000020
#define AIG_WANDER_AROUND 0x00000040
#define AIG_USE_MOVEMENT_TYPE 0x00000080
#define AIG_PLACE_OBJ_ON_OBJ 0x00000100
#define AIG_GUARD_OBJ 0x00000200
#define AIG_GUARD_AREA 0x00000400
#define AIG_BLANK3 0x00000800
#define AIG_FACE_DIR 0x00001000
#define AIG_DODGE_OBJ 0x00002000
#define AIG_FOLLOW_PATH 0x00004000
#define AIG_SET_ANIM 0x00008000
#define AIG_SCRIPTED 0x00010000
#define AIG_MOVE_AROUND_OBJ 0x00020000
#define AIG_MOVE_RELATIVE_OBJ_VEC 0x00040000
#define AIG_MOVE_RELATIVE_OBJ 0x00080000
#define AIG_GET_TO_POS 0x00100000
#define AIG_DO_MELEE_ANIM 0x00200000
#define AIG_GET_AROUND_OBJ 0x00400000
#define AIG_GET_AROUND_POS 0x00800000
#define AIG_ALIGNMENT 0x01000000
#define AIG_AVOID_OBJ 0x02000000
#define AIG_COHESION_OBJ 0x04000000
#define AIG_ALIGN_OBJ 0x08000000

// Goal end types  (Not used yet)
#define AIGE_HANDLE_INVALID 0
#define AIGE_SUCESSFUL 1
#define AIGE_OVERLOAD 2

// Goal Flags
#define GF_NONFLUSHABLE 0x00000001       // A perminent goal
#define GF_HAS_PATH 0x00000002           // This goal has a path (not used yet)
#define GF_KEEP_AT_COMPLETION 0x00000004 // Keeps the goal as long at it is valid
#define GF_NOTIFIES 0x00000008           // By default, goals do not notify (unless an error occurs)
#define GF_OBJ_IS_TARGET 0x00000010
#define GF_CIRCLE_OBJ 0x00000020
#define GF_CIRCLE_POS 0x00000040
#define GF_USE_BLINE_IF_SEES_GOAL 0x00000080
#define GF_FORCE_AWARENESS 0x00000100
#define GF_OBJS_ARE_FRIENDS 0x00000200
#define GF_OBJS_ARE_SPECIES 0x00000400
#define GF_OBJS_ARE_ENEMIES 0x00000800
#define GF_ORIENT_VELOCITY                                                                                             \
  0x00001000 // Defaults to target if there is one (otherwise, to velocity)  This flag forces velocity.
#define GF_RAMPED_INFLUENCE 0x00002000 // By default, distance based goals are (less dist = more influence)
#define GF_MIN_MAX_INFLUENCE 0x00004000
#define GF_SCRIPTED_INFLUENCE 0x00008000
#define GF_ORIENT_TARGET 0x00010000
#define GF_ORIENT_SCRIPTED 0x00020000
#define GF_ORIENT_GOAL_OBJ 0x00040000
#define GF_ORIENT_FOR_ATTACH 0x00080000
#define GF_ORIENT_PATH_NODE 0x00100000
#define GF_PATH_FOLLOW_EXACTLY 0x00200000
#define GF_PATH_REVERSE_AT_END 0x00400000
#define GF_PATH_CIRCLE_AT_END 0x00800000
#define GF_PATH_MOVE_REVERSE_DIR 0x01000000
#define GF_IN_CLEAR 0x02000000 // (Internal flag) Accounts for new goals overriding old goals within a GoalClearGoal
#define GF_IS_ATTACH_CHILD 0x04000000           // The moving object (for the attach goal) is actually the child
#define GF_CLEAR_IF_NOT_CURRENT_GOAL 0x08000000 // Removes the goal if it isn't the current one
#define GF_ORIENT_SET_FVEC 0x10000000           // Face the direction of the fvec
#define GF_ORIENT_SET_FVEC_UVEC 0x20000000      // Face the fvec and uvec

// Goal Flags (goal::flags)
struct [[gnu::packed]] goal_flags_t {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 2;
  uint32_t orient_set_fvec_uvec : 1; // GF_ORIENT_SET_FVEC_UVEC (0x20000000)
  uint32_t orient_set_fvec : 1;     // GF_ORIENT_SET_FVEC (0x10000000)
  uint32_t clear_if_not_current_goal : 1; // GF_CLEAR_IF_NOT_CURRENT_GOAL (0x08000000)
  uint32_t is_attach_child : 1;     // GF_IS_ATTACH_CHILD (0x04000000)
  uint32_t in_clear : 1;            // GF_IN_CLEAR (0x02000000) (Internal flag) Accounts for new goals overriding
                                    // old goals within a GoalClearGoal
  uint32_t path_move_reverse_dir : 1; // GF_PATH_MOVE_REVERSE_DIR (0x01000000)
  uint32_t path_circle_at_end : 1;  // GF_PATH_CIRCLE_AT_END (0x00800000)
  uint32_t path_reverse_at_end : 1; // GF_PATH_REVERSE_AT_END (0x00400000)
  uint32_t path_follow_exactly : 1; // GF_PATH_FOLLOW_EXACTLY (0x00200000)
  uint32_t orient_path_node : 1;    // GF_ORIENT_PATH_NODE (0x00100000)
  uint32_t orient_for_attach : 1;   // GF_ORIENT_FOR_ATTACH (0x00080000)
  uint32_t orient_goal_obj : 1;     // GF_ORIENT_GOAL_OBJ (0x00040000)
  uint32_t orient_scripted : 1;     // GF_ORIENT_SCRIPTED (0x00020000)
  uint32_t orient_target : 1;       // GF_ORIENT_TARGET (0x00010000)
  uint32_t scripted_influence : 1;  // GF_SCRIPTED_INFLUENCE (0x00008000)
  uint32_t min_max_influence : 1;   // GF_MIN_MAX_INFLUENCE (0x00004000)
  uint32_t ramped_influence : 1;    // GF_RAMPED_INFLUENCE (0x00002000)
  uint32_t orient_velocity : 1;     // GF_ORIENT_VELOCITY (0x00001000) Defaults to target if there is one
                                    // (otherwise, to velocity)  This flag forces velocity.
  uint32_t objs_are_enemies : 1;    // GF_OBJS_ARE_ENEMIES (0x00000800)
  uint32_t objs_are_species : 1;    // GF_OBJS_ARE_SPECIES (0x00000400)
  uint32_t objs_are_friends : 1;    // GF_OBJS_ARE_FRIENDS (0x00000200)
  uint32_t force_awareness : 1;     // GF_FORCE_AWARENESS (0x00000100)
  uint32_t use_bline_if_sees_goal : 1; // GF_USE_BLINE_IF_SEES_GOAL (0x00000080)
  uint32_t circle_pos : 1;          // GF_CIRCLE_POS (0x00000040)
  uint32_t circle_obj : 1;          // GF_CIRCLE_OBJ (0x00000020)
  uint32_t obj_is_target : 1;       // GF_OBJ_IS_TARGET (0x00000010)
  uint32_t notifies : 1;            // GF_NOTIFIES (0x00000008) By default, goals do not notify (unless an
                                    // error occurs)
  uint32_t keep_at_completion : 1;  // GF_KEEP_AT_COMPLETION (0x00000004) Keeps the goal as long at it is valid
  uint32_t has_path : 1;            // GF_HAS_PATH (0x00000002) This goal has a path (not used yet)
  uint32_t nonflushable : 1;        // GF_NONFLUSHABLE (0x00000001) A perminent goal
#else
  uint32_t nonflushable : 1;        // GF_NONFLUSHABLE (0x00000001) A perminent goal
  uint32_t has_path : 1;            // GF_HAS_PATH (0x00000002) This goal has a path (not used yet)
  uint32_t keep_at_completion : 1;  // GF_KEEP_AT_COMPLETION (0x00000004) Keeps the goal as long at it is valid
  uint32_t notifies : 1;            // GF_NOTIFIES (0x00000008) By default, goals do not notify (unless an
                                    // error occurs)
  uint32_t obj_is_target : 1;       // GF_OBJ_IS_TARGET (0x00000010)
  uint32_t circle_obj : 1;          // GF_CIRCLE_OBJ (0x00000020)
  uint32_t circle_pos : 1;          // GF_CIRCLE_POS (0x00000040)
  uint32_t use_bline_if_sees_goal : 1; // GF_USE_BLINE_IF_SEES_GOAL (0x00000080)
  uint32_t force_awareness : 1;     // GF_FORCE_AWARENESS (0x00000100)
  uint32_t objs_are_friends : 1;    // GF_OBJS_ARE_FRIENDS (0x00000200)
  uint32_t objs_are_species : 1;    // GF_OBJS_ARE_SPECIES (0x00000400)
  uint32_t objs_are_enemies : 1;    // GF_OBJS_ARE_ENEMIES (0x00000800)
  uint32_t orient_velocity : 1;     // GF_ORIENT_VELOCITY (0x00001000) Defaults to target if there is one
                                    // (otherwise, to velocity)  This flag forces velocity.
  uint32_t ramped_influence : 1;    // GF_RAMPED_INFLUENCE (0x00002000)
  uint32_t min_max_influence : 1;   // GF_MIN_MAX_INFLUENCE (0x00004000)
  uint32_t scripted_influence : 1;  // GF_SCRIPTED_INFLUENCE (0x00008000)
  uint32_t orient_target : 1;       // GF_ORIENT_TARGET (0x00010000)
  uint32_t orient_scripted : 1;     // GF_ORIENT_SCRIPTED (0x00020000)
  uint32_t orient_goal_obj : 1;     // GF_ORIENT_GOAL_OBJ (0x00040000)
  uint32_t orient_for_attach : 1;   // GF_ORIENT_FOR_ATTACH (0x00080000)
  uint32_t orient_path_node : 1;    // GF_ORIENT_PATH_NODE (0x00100000)
  uint32_t path_follow_exactly : 1; // GF_PATH_FOLLOW_EXACTLY (0x00200000)
  uint32_t path_reverse_at_end : 1; // GF_PATH_REVERSE_AT_END (0x00400000)
  uint32_t path_circle_at_end : 1;  // GF_PATH_CIRCLE_AT_END (0x00800000)
  uint32_t path_move_reverse_dir : 1; // GF_PATH_MOVE_REVERSE_DIR (0x01000000)
  uint32_t in_clear : 1;            // GF_IN_CLEAR (0x02000000) (Internal flag) Accounts for new goals overriding
                                    // old goals within a GoalClearGoal
  uint32_t is_attach_child : 1;     // GF_IS_ATTACH_CHILD (0x04000000)
  uint32_t clear_if_not_current_goal : 1; // GF_CLEAR_IF_NOT_CURRENT_GOAL (0x08000000)
  uint32_t orient_set_fvec : 1;     // GF_ORIENT_SET_FVEC (0x10000000)
  uint32_t orient_set_fvec_uvec : 1; // GF_ORIENT_SET_FVEC_UVEC (0x20000000)
  uint32_t padding : 2;
#endif
};
static_assert(sizeof(goal_flags_t) == sizeof(uint32_t));
#define GF_SPEED_MASK (0x40000000 | 0x80000000)
#define GF_SPEED_NORMAL 0x00000000
#define GF_SPEED_DODGE 0x40000000
#define GF_SPEED_FLEE 0x80000000
#define GF_SPEED_ATTACK (0x40000000 | 0x80000000)

#define ISORIENTGOAL(x)                                                                                                \
  ((x)->flags & (GF_ORIENT_VELOCITY | GF_ORIENT_TARGET | GF_ORIENT_SCRIPTED | GF_ORIENT_GOAL_OBJ |                     \
                 GF_ORIENT_FOR_ATTACH | GF_ORIENT_PATH_NODE | GF_ORIENT_SET_FVEC | GF_ORIENT_SET_FVEC_UVEC))
#define ISTRACKGOAL(x) ((x)->flags & (GF_OBJS_ARE_FRIENDS | GF_OBJS_ARE_SPECIES | GF_OBJS_ARE_ENEMIES))

// Goal Sub-Types  -- game.consts
#define GST_FVEC 0
#define GST_NEG_FVEC 1
#define GST_RVEC 2
#define GST_NEG_RVEC 3
#define GST_UVEC 4
#define GST_NEG_UVEC 5

// Influence constants
#define MAX_INFLUENCE 10000.0f
#define HIGH_INFLUENCE 10.0f
#define NORMAL_INFLUENCE 1.0f
#define LOW_INFLUENCE 0.5f

#define MAX_INFLUENCE_DELTA_CONSIDER 100.0f

#define NUM_ACTIVATION_LEVELS 4
#define ACTIVATION_BLEND_LEVEL 100

//-------------------------------------------------
// AI notification of events
//-------------------------------------------------

#define AIN_NEW_MOVEMENT 1
#define AIN_OBJ_KILLED 2
#define AIN_WHIT_BY_OBJ 3
#define AIN_SEE_TARGET 4
#define AIN_PLAYER_SEES_YOU 5
#define AIN_WHIT_OBJECT 6
#define AIN_TARGET_DIED 7 // In code, it only notifies if the target is gone
#define AIN_OBJ_FIRED 8
#define AIN_GOAL_COMPLETE 9
#define AIN_GOAL_FAIL 10
#define AIN_GOAL_ERROR 11
#define AIN_HEAR_NOISE 12
#define AIN_NEAR_TARGET 13
#define AIN_HIT_BY_WEAPON 14
#define AIN_NEAR_WALL 15
#define AIN_USER_DEFINED 16   // Processed in script
#define AIN_TARGET_INVALID 17 // Goal is killed
#define AIN_GOAL_INVALID 18
#define AIN_SCRIPTED_GOAL 19
#define AIN_SCRIPTED_ENABLER 20
#define AIN_ANIM_COMPLETE 21
#define AIN_BUMPED_OBJ 22
#define AIN_MELEE_HIT 23
#define AIN_MELEE_ATTACK_FRAME 24
#define AIN_SCRIPTED_INFLUENCE 25
#define AIN_SCRIPTED_ORIENT 26
#define AIN_MOVIE_START 27
#define AIN_MOVIE_END 28
#define AIN_FIRED_WEAPON 29

#define AI_NOTIFIES_ALWAYS_ON                                                                                          \
  ((0x00000001 << AIN_ANIM_COMPLETE) | (0x00000001 << AIN_NEW_MOVEMENT) | (0x00000001 << AIN_PLAYER_SEES_YOU) |        \
   (0x00000001 << AIN_GOAL_COMPLETE) | (0x00000001 << AIN_GOAL_FAIL) | (0x00000001 << AIN_GOAL_ERROR) |                \
   (0x00000001 << AIN_USER_DEFINED) | (0x00000001 << AIN_TARGET_DIED) | (0x00000001 << AIN_TARGET_INVALID) |           \
   (0x00000001 << AIN_BUMPED_OBJ) | (0x00000001 << AIN_MELEE_HIT) | (0x00000001 << AIN_MELEE_ATTACK_FRAME) |           \
   (0x00000001 << AIN_TARGET_INVALID))

#define AWARE_FULLY 100.0f
#define AWARE_MOSTLY 60.0f
#define AWARE_PARTIALLY 30.0f
#define AWARE_BARELY 15.0f
#define AWARE_NONE 0.0f

#define MAX_AWARE_TIME 5.0f
#define MAX_RENDER_RECENTLY_TIME 120.0f

#define AWARENESS_RENDER_RECENTLY_INTERVAL 20.0f
#define AWARENESS_SEE_TARGET_RECENTLY_INTERVAL 25.0f

#define AWARE_FALLOFF (AWARE_FULLY / MAX_AWARE_TIME)
#define AWARE_RENDER_RECENTLY_FALLOFF (AWARE_FULLY / MAX_RENDER_RECENTLY_TIME)

#define MIN_NEXT_PATH_INTERVAL 2.0f

// Designer-editable AI flags (replaces AIF_* for ai_info_t::flags).
struct ai_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t aim_pnt_fov : 1;                 // AIF_AIM_PNT_FOV
  uint32_t uvec_fov : 1;                    // AIF_UVEC_FOV
  uint32_t force_awareness : 1;             // AIF_FORCE_AWARENESS
  uint32_t biased_flight_height : 1;        // AIF_BIASED_FLIGHT_HEIGHT
  uint32_t track_closest_2_enemies : 1;     // AIF_TRACK_CLOSEST_2_ENEMIES
  uint32_t track_closest_2_friends : 1;     // AIF_TRACK_CLOSEST_2_FRIENDS
  uint32_t auto_avoid_friends : 1;          // AIF_AUTO_AVOID_FRIENDS
  uint32_t disable_melee : 1;               // AIF_DISABLE_MELEE
  uint32_t disable_firing : 1;              // AIF_DISABLE_FIRING
  uint32_t target_by_dist : 1;              // AIF_TARGET_BY_DIST
  uint32_t report_new_orient : 1;           // AIF_REPORT_NEW_ORIENT
  uint32_t xz_dist : 1;                     // AIF_XZ_DIST
  uint32_t orient_to_vel : 1;               // AIF_ORIENT_TO_VEL
  uint32_t ordered_wb_firing : 1;           // AIF_ORDERED_WB_FIRING
  uint32_t team : 2;                        // AIF_TEAM_* (PTMC/REBEL/HOSTILE/NEUTRAL)
  uint32_t fluctuate_speed_properties : 1;  // AIF_FLUCTUATE_SPEED_PROPERTIES
  uint32_t disabled : 1;                    // AIF_DISABLED
  uint32_t avoid_walls : 1;                 // AIF_AVOID_WALLS
  uint32_t only_taunt_at_death : 1;         // AIF_ONLY_TAUNT_AT_DEATH
  uint32_t aim : 1;                         // AIF_AIM
  uint32_t determine_target : 1;            // AIF_DETERMINE_TARGET
  uint32_t flinch : 1;                      // AIF_FLINCH
  uint32_t fire : 1;                        // AIF_FIRE
  uint32_t dodge : 1;                       // AIF_DODGE
  uint32_t persistant : 1;                  // AIF_PERSISTANT
  uint32_t act_as_neutral_until_shot : 1;   // AIF_ACT_AS_NEUTRAL_UNTIL_SHOT
  uint32_t stays_inout : 1;                 // AIF_STAYS_INOUT (also AIF_GB_MIMIC_PLAYER_FIRING_HACK)
  uint32_t melee2 : 1;                      // AIF_MELEE2
  uint32_t melee1 : 1;                      // AIF_MELEE1
  uint32_t weapon2 : 1;                     // AIF_WEAPON2
  uint32_t weapon1 : 1;                     // AIF_WEAPON1
#else
  uint32_t weapon1 : 1;                     // AIF_WEAPON1
  uint32_t weapon2 : 1;                     // AIF_WEAPON2
  uint32_t melee1 : 1;                      // AIF_MELEE1
  uint32_t melee2 : 1;                      // AIF_MELEE2
  uint32_t stays_inout : 1;                 // AIF_STAYS_INOUT (also AIF_GB_MIMIC_PLAYER_FIRING_HACK)
  uint32_t act_as_neutral_until_shot : 1;   // AIF_ACT_AS_NEUTRAL_UNTIL_SHOT
  uint32_t persistant : 1;                  // AIF_PERSISTANT
  uint32_t dodge : 1;                       // AIF_DODGE
  uint32_t fire : 1;                        // AIF_FIRE
  uint32_t flinch : 1;                      // AIF_FLINCH
  uint32_t determine_target : 1;            // AIF_DETERMINE_TARGET
  uint32_t aim : 1;                         // AIF_AIM
  uint32_t only_taunt_at_death : 1;         // AIF_ONLY_TAUNT_AT_DEATH
  uint32_t avoid_walls : 1;                 // AIF_AVOID_WALLS
  uint32_t disabled : 1;                    // AIF_DISABLED
  uint32_t fluctuate_speed_properties : 1;  // AIF_FLUCTUATE_SPEED_PROPERTIES
  uint32_t team : 2;                        // AIF_TEAM_* (PTMC/REBEL/HOSTILE/NEUTRAL)
  uint32_t ordered_wb_firing : 1;           // AIF_ORDERED_WB_FIRING
  uint32_t orient_to_vel : 1;               // AIF_ORIENT_TO_VEL
  uint32_t xz_dist : 1;                     // AIF_XZ_DIST
  uint32_t report_new_orient : 1;           // AIF_REPORT_NEW_ORIENT
  uint32_t target_by_dist : 1;              // AIF_TARGET_BY_DIST
  uint32_t disable_firing : 1;              // AIF_DISABLE_FIRING
  uint32_t disable_melee : 1;               // AIF_DISABLE_MELEE
  uint32_t auto_avoid_friends : 1;          // AIF_AUTO_AVOID_FRIENDS
  uint32_t track_closest_2_friends : 1;     // AIF_TRACK_CLOSEST_2_FRIENDS
  uint32_t track_closest_2_enemies : 1;     // AIF_TRACK_CLOSEST_2_ENEMIES
  uint32_t biased_flight_height : 1;        // AIF_BIASED_FLIGHT_HEIGHT
  uint32_t force_awareness : 1;             // AIF_FORCE_AWARENESS
  uint32_t uvec_fov : 1;                    // AIF_UVEC_FOV
  uint32_t aim_pnt_fov : 1;                 // AIF_AIM_PNT_FOV
#endif
};
static_assert(sizeof(ai_flags_t) == sizeof(uint32_t));

// Designer-editable AI notification flags (replaces notify_flags for
// ai_info_t).  Bit N corresponds to AIN_N event; only events the AI is told
// about are set.
struct ai_notify_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding_h : 2;               // Unused padding to complete 32 bits
  uint32_t fired_weapon : 1;            // AIN_FIRED_WEAPON
  uint32_t movie_end : 1;               // AIN_MOVIE_END
  uint32_t movie_start : 1;             // AIN_MOVIE_START
  uint32_t scripted_orient : 1;         // AIN_SCRIPTED_ORIENT
  uint32_t scripted_influence : 1;      // AIN_SCRIPTED_INFLUENCE
  uint32_t melee_attack_frame : 1 = true;  // AIN_MELEE_ATTACK_FRAME
  uint32_t melee_hit : 1 = true;           // AIN_MELEE_HIT
  uint32_t bumped_obj : 1 = true;          // AIN_BUMPED_OBJ
  uint32_t anim_complete : 1 = true;       // AIN_ANIM_COMPLETE
  uint32_t scripted_enabler : 1;           // AIN_SCRIPTED_ENABLER
  uint32_t scripted_goal : 1;              // AIN_SCRIPTED_GOAL
  uint32_t goal_invalid : 1;               // AIN_GOAL_INVALID
  uint32_t target_invalid : 1 = true;      // AIN_TARGET_INVALID
  uint32_t user_defined : 1 = true;        // AIN_USER_DEFINED
  uint32_t near_wall : 1;                  // AIN_NEAR_WALL
  uint32_t hit_by_weapon : 1;              // AIN_HIT_BY_WEAPON
  uint32_t near_target : 1;                // AIN_NEAR_TARGET
  uint32_t hear_noise : 1;                 // AIN_HEAR_NOISE
  uint32_t goal_error : 1 = true;          // AIN_GOAL_ERROR
  uint32_t goal_fail : 1 = true;           // AIN_GOAL_FAIL
  uint32_t goal_complete : 1 = true;       // AIN_GOAL_COMPLETE
  uint32_t obj_fired : 1;                  // AIN_OBJ_FIRED
  uint32_t target_died : 1 = true;         // AIN_TARGET_DIED
  uint32_t whit_object : 1;                // AIN_WHIT_OBJECT
  uint32_t player_sees_you : 1 = true;     // AIN_PLAYER_SEES_YOU
  uint32_t see_target : 1;                 // AIN_SEE_TARGET
  uint32_t whit_by_obj : 1;                // AIN_WHIT_BY_OBJ
  uint32_t obj_killed : 1;                 // AIN_OBJ_KILLED
  uint32_t new_movement : 1 = true;        // AIN_NEW_MOVEMENT
  uint32_t padding_l : 1;                  // Unused padding (bit 0)
#else
  uint32_t padding_l : 1;                  // Unused padding (bit 0)
  uint32_t new_movement : 1 = true;        // AIN_NEW_MOVEMENT
  uint32_t obj_killed : 1;                 // AIN_OBJ_KILLED
  uint32_t whit_by_obj : 1;                // AIN_WHIT_BY_OBJ
  uint32_t see_target : 1;                 // AIN_SEE_TARGET
  uint32_t player_sees_you : 1 = true;     // AIN_PLAYER_SEES_YOU
  uint32_t whit_object : 1;                // AIN_WHIT_OBJECT
  uint32_t target_died : 1 = true;         // AIN_TARGET_DIED
  uint32_t obj_fired : 1;                  // AIN_OBJ_FIRED
  uint32_t goal_complete : 1 = true;       // AIN_GOAL_COMPLETE
  uint32_t goal_fail : 1 = true;           // AIN_GOAL_FAIL
  uint32_t goal_error : 1 = true;          // AIN_GOAL_ERROR
  uint32_t hear_noise : 1;                 // AIN_HEAR_NOISE
  uint32_t near_target : 1;                // AIN_NEAR_TARGET
  uint32_t hit_by_weapon : 1;              // AIN_HIT_BY_WEAPON
  uint32_t near_wall : 1;                  // AIN_NEAR_WALL
  uint32_t user_defined : 1 = true;        // AIN_USER_DEFINED
  uint32_t target_invalid : 1 = true;      // AIN_TARGET_INVALID
  uint32_t goal_invalid : 1;               // AIN_GOAL_INVALID
  uint32_t scripted_goal : 1;              // AIN_SCRIPTED_GOAL
  uint32_t scripted_enabler : 1;           // AIN_SCRIPTED_ENABLER
  uint32_t anim_complete : 1 = true;       // AIN_ANIM_COMPLETE
  uint32_t bumped_obj : 1 = true;          // AIN_BUMPED_OBJ
  uint32_t melee_hit : 1 = true;           // AIN_MELEE_HIT
  uint32_t melee_attack_frame : 1 = true;  // AIN_MELEE_ATTACK_FRAME
  uint32_t scripted_influence : 1;         // AIN_SCRIPTED_INFLUENCE
  uint32_t scripted_orient : 1;            // AIN_SCRIPTED_ORIENT
  uint32_t movie_start : 1;                // AIN_MOVIE_START
  uint32_t movie_end : 1;                  // AIN_MOVIE_END
  uint32_t fired_weapon : 1;               // AIN_FIRED_WEAPON
  uint32_t padding_h : 2;                  // Unused padding to complete 32 bits
#endif
};
static_assert(sizeof(ai_notify_flags_t) == sizeof(uint32_t));

#define AI_ATTACK_SOUND 0
#define AI_SEE_SOUND 1
#define AI_FLEE_SOUND 2
#define AI_ANGRY_SOUND 3
#define AI_TURRET_SOUND 4

//-------------------------------------------------
// AI Status Registers
//-------------------------------------------------

// Transient per-agent/goal AI state (replaces AISR_* for goal::status_reg and
// ai_frame::status_reg).
struct ai_status_reg_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 24;                 // Unused bits to complete 32
  uint32_t ok_to_fire_during_cinematics : 1;  // AISR_OK_TO_FIRE_DURING_CINEMATICS
  uint32_t sees_goal : 1;                // AISR_SEES_GOAL
  uint32_t ranged_attack : 1;            // AISR_RANGED_ATTACK
  uint32_t melee : 1;                    // AISR_MELEE
  uint32_t path : 1;                     // AISR_PATH
  uint32_t circle_dist : 1;              // AISR_CIRCLE_DIST
  uint32_t attacking : 1;                // AISR_ATTACKING
  uint32_t fleeing : 1;                  // AISR_FLEE
#else
  uint32_t fleeing : 1;                  // AISR_FLEE
  uint32_t attacking : 1;                // AISR_ATTACKING
  uint32_t circle_dist : 1;              // AISR_CIRCLE_DIST
  uint32_t path : 1;                     // AISR_PATH
  uint32_t melee : 1;                    // AISR_MELEE
  uint32_t ranged_attack : 1;            // AISR_RANGED_ATTACK
  uint32_t sees_goal : 1;                // AISR_SEES_GOAL
  uint32_t ok_to_fire_during_cinematics : 1;  // AISR_OK_TO_FIRE_DURING_CINEMATICS
  uint32_t padding : 24;                 // Unused bits to complete 32
#endif
};
static_assert(sizeof(ai_status_reg_t) == sizeof(uint32_t));

//-------------------------------------------------
// AI Anim states
//-------------------------------------------------

#define AS_ALERT 0
#define AS_DEATH 1
#define AS_BIRTH 2
#define AS_MISSILE1_RECOIL 3
#define AS_MISSILE2 4
#define AS_MISSILE2_RECOIL 5
#define AS_MELEE1 6
#define AS_MELEE1_RECOIL 7
#define AS_MELEE2 8
#define AS_MELEE2_RECOIL 9
#define AS_IDLE 10
#define AS_QUIRK 11
#define AS_FLINCH 12
#define AS_TAUNT 13
#define AS_GOTO_IDLE_STANDING 14
#define AS_GOTO_IDLE_FLYING 15
#define AS_GOTO_IDLE_ROLLING 16
#define AS_GOTO_IDLE_WALKING 17
#define AS_GOTO_IDLE_JUMPING 18
#define AS_GOTO_ALERT_STANDING 19
#define AS_GOTO_ALERT_FLYING 20
#define AS_GOTO_ALERT_ROLLING 21
#define AS_GOTO_ALERT_WALKING 22
#define AS_GOTO_ALERT_JUMPING 23
#define AS_TO_SIDE_STEP 24
#define AS_SIDE_STEP 25
#define AS_FROM_SIDE_STEP 26

// These are for ease of coding
#define AS_GOTO_IDLE_OFFSET 14
#define AS_GOTO_ALERT_OFFSET 19

// Special -- not on Animation dialog
#define AS_RANGED_ATTACK 27
#define AS_RANGED_RECOIL 28

#define AS_CUSTOM 29

#endif
