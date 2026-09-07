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

 * $Logfile: /DescentIII/main/TelComEfxStructs.h $
 * $Revision: 19 $
 * $Date: 4/20/99 12:46p $
 * $Author: Jeff $
 *
 * Structures used for TelCom effects
 *
 * $Log: /DescentIII/main/TelComEfxStructs.h $
 *
 * 19    4/20/99 12:46p Jeff
 * telcom main menu, mouse over button sets focus.  if you go into telcom
 * main menu, when you leave a system it will return you to main menu.
 *
 * 18    4/17/99 6:15p Samir
 * replaced gr.h with grdefs.h and fixed resulting compile bugs.
 *
 * 17    2/17/99 6:56p Jeff
 * added jump button type.  Added no early render flag for bitmaps.  Fixed
 * color bug for type text
 *
 * 16    2/03/99 12:14a Jeff
 * updated single player ship selection to full functional...needs effects
 * added in.  Had to add flag to buttons for gain focus to register as a
 * click
 *
 * 15    1/04/99 12:33p Jeff
 * added support for mission flag parsing
 *
 * 14    10/12/98 8:32p Jeff
 * changed the way focus is handled
 *
 * 13    9/17/98 2:29p Jeff
 * added focus filenames to button effect
 *
 * 12    9/03/98 2:20p Jeff
 * creation of dialogs in Brief editor
 *
 * 11    8/27/98 2:51p Jeff
 * New TelCom finally checked in
 *
 * 10    5/19/98 3:40p Jeff
 * poly models functional
 *
 * 9     5/18/98 2:29p Jeff
 * added new type of on screen button, a glow type (if mouse is over it,
 * it will do the glow)
 *
 * 8     5/05/98 6:50p Jeff
 * Telcom doesn't use rend_DrawLFBitmap anymore...more speed!
 *
 * 7     5/04/98 1:35p Jeff
 * Changes made for mouse handling
 *
 * 6     5/04/98 11:01a Jeff
 * added scroll parameter to text
 *
 * 5     5/01/98 2:17p Jeff
 * Added sound effect support
 *
 * 4     4/28/98 6:58p Jeff
 * Added new poly model effect driver
 *
 * 3     4/26/98 7:21p Jeff
 * added stretch bitmap effect define
 *
 * 2     4/26/98 2:54a Jeff
 * Initial Creation
 *
 * $NoKeywords: $
 */

#ifndef __TELCOMEFXSTRUCTS_H_
#define __TELCOMEFXSTRUCTS_H_

#include <string>

#include "grdefs.h"

#define MAX_FILELEN 32
//===================
// tc_text
//  - Contains information on the text coordinates of an effect
//===================
struct tc_text {
  int left, right, top, bottom;
};

// TCTEXTDESC caps flags
struct [[gnu::packed]] text_caps_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 24;
  uint32_t tabstop : 1;
  uint32_t scroll : 1;
  uint32_t textbox : 1;
  uint32_t waittime : 1;
  uint32_t looping : 1;
  uint32_t speed : 1;
  uint32_t color : 1;
  uint32_t font : 1;
#else
  uint32_t font : 1;
  uint32_t color : 1;
  uint32_t speed : 1;
  uint32_t looping : 1;
  uint32_t waittime : 1;
  uint32_t textbox : 1;
  uint32_t scroll : 1;
  uint32_t tabstop : 1;
  uint32_t padding : 24;
#endif
};
static_assert(sizeof(text_caps_flags_t) == sizeof(uint32_t));

// TCBMPDESC caps flags
struct [[gnu::packed]] bmp_caps_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 28;
  uint32_t speed : 1;
  uint32_t waittime : 1;
  uint32_t looping : 1;
  uint32_t xy : 1;
#else
  uint32_t xy : 1;
  uint32_t looping : 1;
  uint32_t waittime : 1;
  uint32_t speed : 1;
  uint32_t padding : 28;
#endif
};
static_assert(sizeof(bmp_caps_flags_t) == sizeof(uint32_t));

// TCMOVIEDESC caps flags
struct [[gnu::packed]] movie_caps_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 28;
  uint32_t fps : 1;
  uint32_t waittime : 1;
  uint32_t looping : 1;
  uint32_t xy : 1;
#else
  uint32_t xy : 1;
  uint32_t looping : 1;
  uint32_t waittime : 1;
  uint32_t fps : 1;
  uint32_t padding : 28;
#endif
};
static_assert(sizeof(movie_caps_flags_t) == sizeof(uint32_t));

// TCBKGDESC caps flags
struct [[gnu::packed]] bkg_caps_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 29;
  uint32_t color : 1;
  uint32_t waittime : 1;
  uint32_t id : 1;
#else
  uint32_t id : 1;
  uint32_t waittime : 1;
  uint32_t color : 1;
  uint32_t padding : 29;
#endif
};
static_assert(sizeof(bkg_caps_flags_t) == sizeof(uint32_t));

// TCPOLYDESC caps flags
struct [[gnu::packed]] poly_caps_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 31;
  uint32_t speed : 1;
#else
  uint32_t speed : 1;
  uint32_t padding : 31;
#endif
};
static_assert(sizeof(poly_caps_flags_t) == sizeof(uint32_t));

// TCSNDDESC caps flags
struct [[gnu::packed]] snd_caps_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 30;
  uint32_t once : 1;
  uint32_t waittime : 1;
#else
  uint32_t waittime : 1;
  uint32_t once : 1;
  uint32_t padding : 30;
#endif
};
static_assert(sizeof(snd_caps_flags_t) == sizeof(uint32_t));

// On-screen button flags (TCBUTTONDESC)
struct [[gnu::packed]] button_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 28;
  uint32_t mouseover_focus : 1;
  uint32_t change_focus_is_click : 1;
  uint32_t glow : 1;
  uint32_t flash : 1;
#else
  uint32_t flash : 1;
  uint32_t glow : 1;
  uint32_t change_focus_is_click : 1;
  uint32_t mouseover_focus : 1;
  uint32_t padding : 28;
#endif
};
static_assert(sizeof(button_flags_t) == sizeof(uint32_t));

// Button type / click type enumeration values (mirror TelComEffects.h).  The
// underlying values are stored in TCBUTTONDESC.
enum class tc_button_type : uint8_t {
  up_arrow = 0,
  down_arrow = 1,
  next_page = 2,
  prev_page = 3,
  quit = 4,
  jump = 6,
};

enum class tc_click_type : uint8_t {
  click_down = 0,
  click_up = 1,
  hold = 2,
};

// Type values
#define TC_TEXT_STATIC 0
#define TC_TEXT_SCROLL 1
#define TC_TEXT_FADE 2
#define TC_TEXT_FLASH 3
#define TC_BMP_STATIC 0
#define TC_BMP_BLUR 1
#define TC_BMP_SCANLINE 2
#define TC_BMP_INVERT 3
#define TC_BMP_STRETCH 4
#define TC_BACK_STATIC 0
#define TC_MOVIE_STATIC 0
#define TC_POLY_STATIC 0
#define TC_SND_STATIC 0

// text effect mode (scroll direction / fade)
enum class tc_text_mode : uint8_t {
  scroll_l2r = 0,
  scroll_r2l = 1,
  scroll_t2b = 2,
  scroll_b2t = 3,
  fade_in = 4,
  fade_out = 5,
  fade_pingpong = 6,
};

// bitmap effect mode (blur/scanline/invert in or out)
enum class tc_bmp_mode : uint8_t { in = 0, out = 1 };

//===================
// TCTEXTDESC
//  - Contains all the info for a text effect passed to the monitor call
//===================
//	The following structure should be filled in, and passed to the Monitor::Text() function
// caps must be set with the above flags.  If a member isn't set to be defined by the caps, then
// if it's needed default values will be used (defaults listed after each member in structure)
struct TCTEXTDESC {
  // what values are set/defined
  text_caps_flags_t caps; //(MUST BE DEFINED!)
  // the text box of the effect
  tc_text textbox; //(Default:entire monitor screen)
  // what kind of font to use
  int font; //(Default: BRIEF_FONT_INDEX)
  // default color of the text
  ddgr_color color; //(Default: GR_GREEN)
  // effect mode (scroll direction / fade)
  tc_text_mode mode;
  // speed
  float speed;
  // does the text effect loop
  bool looping; //(Default: false)
  // how much time to wait before effect starts
  float waittime; //(Default: 0.0f)
  // type
  int type;
  // mission flags mask
  uint32_t mission_mask_set, mission_mask_unset;
};

struct TCBMPDESC {
  // type
  int type;
  // effect mode (blur/scanline/invert in or out)
  tc_bmp_mode mode;
  // don't start rendering until time is up
  bool no_early_render;
  // what members are defined
  bmp_caps_flags_t caps; // MUST BE FILLED IN
  // upped left corner to display bitmap
  int x, y; //(Default: (0,0) )
  // does the effect loop
  bool looping; //(Default: false)
  // how long to wait until displaying the bitmap
  float waittime; //(Default: 0.0f)
  // how fast the bitmap effect should be (only useful for non-static)
  float speed; //(Default: 2.0f)
  // filename
  std::string filename; // MUST BE FILLED IN
  // mission flag mask
  uint32_t mission_mask_set, mission_mask_unset;
};

struct TCMOVIEDESC {
  // fill in with whats valid
  movie_caps_flags_t caps; // MUST BE FILLED IN
  // type
  int type;
  // upper left coordinate of the movie
  int x, y; //(Default: (0,0) )
  // filename of the movie to play
  std::string filename; // MUST BE FILLED IN
  // whether the movie is to loop
  bool looping; //(Default: false)
  // Frames per second
  float fps; //(Default: 20.0f)
  // time to wait until starting the movie
  float waittime; //(Default: 0.0f)
  // mission flag mask
  uint32_t mission_mask_set, mission_mask_unset;
};

struct TCBKGDESC {
  // whats defined for the struct
  bkg_caps_flags_t caps; // MUST BE SET!
  // what type of background effect
  int type; //(Default: MBS_DESKTOP)
  // id used by the type
  int id; //(if type is set to Desktop then it MUST BE SET to the bitmap handle, else MBI_QUICKCLEAR)
  // color if needed
  ddgr_color color; //(Default: GR_BLUE)
  // how long to wait until effect start
  float waittime; //(Default: 0.0f)
  // mission flag mask
  uint32_t mission_mask_set, mission_mask_unset;
};
struct TCPOLYDESC {
  // whats defined for the struct
  poly_caps_flags_t caps; // MUST BE SET!
  // what type of poly effect
  int type;
  // position
  float pos_x, pos_y, pos_z; // coordinates (MUST BE SET!)
  // rotation (degrees per sec)
  float rot_x, rot_y, rot_z;
  // orientation
  float ori_x, ori_y, ori_z;
  // flags to use
  int flags;
  // how long to wait until effect starts
  float waittime;
  // polymodel name
  std::string polyname;
  // mission flag mask
  uint32_t mission_mask_set, mission_mask_unset;
};

struct TCSNDDESC {
  // whats defined for the struct
  snd_caps_flags_t caps; // MUST BE SET
  // type of sound effect
  int type;
  // true if this sound should only play once
  bool once;
  // how long to wait until effect starts
  float waittime;
  // sound name
  std::string filename;
  // mission flag mask
  uint32_t mission_mask_set, mission_mask_unset;
};

struct TCBUTTONDESC {
  std::string filename;
  std::string filename_focus;
  std::string flash_filename;
  std::string flash_filename_focus;
  int sibling_id; // Sibling effect ID(for down/up arrows), -1 if none
  int parent_id;  // Parent effect ID (text for down/up arrows), -1 if it works with TelCom System
  int x, y, w, h;
  button_flags_t osflags;
  int jump_page;
  void (*internal)(int);
  float flash_time;
  tc_button_type button_type; // Up arrow, Down Arrow, TelCom System
  tc_click_type click_type;   // CLICKTYPE_DOWN or CLICKTYPE_CLICK (what the button responds to)
  bool flasher;
  bool tab_stop;
  uint32_t mission_mask_set, mission_mask_unset; // mission flag mask
};

#endif
