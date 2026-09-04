#if 0
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

 * $Logfile: /DescentIII/Main/physics/FindIntersection.cpp $
 * $Revision: 231 $
 * $Date: 10/25/99 1:17a $
 * $Author: Chris $
 *
 * Find intersection code
 *
 * $Log: /DescentIII/Main/physics/FindIntersection.cpp $
 *
 * 231   10/25/99 1:17a Chris
 *
 * 230   10/21/99 3:12p Kevin
 * Mac merge & memory savings
 *
 * 229   6/10/99 6:31p Chris
 * Added support for the new editor
 *
 * 228   5/23/99 2:53a Chris
 * Removed annoying asserts
 *
 * 227   5/23/99 12:32a Chris
 * Fixed problems with ignoring render though portals
 *
 * 226   5/20/99 1:16a Chris
 * Fixed the last wall stuck cases
 *
 * 225   5/18/99 11:10a Matt
 * Added variable ceiling height.
 *
 * 224   5/17/99 6:05p Chris
 * Added support for hit faces on a submodel
 *
 * 223   5/17/99 5:01p Matt
 * Changed the code to check for objects in visited rooms whether the
 * vector started on the terrain or inside.  This fixes the problem where
 * the mass driver, when fired from outside, could not hit objects inside.
 *
 * 222   5/12/99 6:16a Chris
 * fvi_QuickDistObjectList if stop at closed doors, it also stops at
 * blocked/forcefielded portals
 *
 * 221   5/09/99 3:07p Jason
 * added a way to track FVI rays
 *
 * 220   5/07/99 6:21p Jeff
 * hooked up fvi to rtperformance
 *
 * 219   5/03/99 3:14p Matt
 * Fixed bug in Chris's hack.
 *
 * 218   5/03/99 1:52p Chris
 * Hacked FVI to make relinking consistant even with level cracks
 *
 * 217   4/20/99 8:14p Chris
 * Added support for object's that hit the ceiling and for making the
 * level always check for the ceiling (inside and outside the mine)
 *
 * 216   4/18/99 5:42a Chris
 * Added the FQ_IGNORE_RENDER_THROUGH_PORTALS flag
 *
 * 215   4/14/99 1:42a Jeff
 * fixed case mismatched #includes
 *
 * 214   4/12/99 10:21a Chris
 * Added another divide by zero assert
 *
 * 213   4/12/99 10:20a Chris
 * Added an assert to catch bad hit obj sizes
 *
 * 212   4/08/99 4:53p Chris
 *
 * 211   4/05/99 3:56p Chris
 * We should be able to pickup clutter now
 *
 * 210   4/02/99 10:18a Chris
 * SPHERE-SPHERE for player rock collisions
 *
 * 209   3/31/99 5:16p Matt
 * Don't look at non-shell faces when determining what room a point is in
 *
 * 208   3/09/99 10:52a Chris
 * Fix a bug introduced when trying to to fix the trigger problems
 *
 * 207   3/05/99 1:02p Jason
 * sped up raycasting for light coronas
 *
 * 206   3/04/99 8:15p Matt
 * Hack that might help the exit level trigger problem on OEM level 0.
 *
 * 205   3/04/99 5:54p Chris
 * Patched the occassional not found portal trigger
 *
 * 204   2/25/99 1:54p Chris
 * Ignoring weapons now ignores shockwaves, shards, and fireballs
 *
 * 203   2/16/99 3:09p Chris
 *
 * 202   2/16/99 1:51p Chris
 * FIxed multiple trigger bugs
 *
 * 201   2/15/99 4:38p Chris
 * Damn...  Lava was hitting triggers!!!!!!
 *
 * 200   2/15/99 11:47a Chris
 *
 * 199   2/11/99 6:25p Chris
 * Added PF_NO_COLLIDE_DOORS
 *
 * 198   2/02/99 8:44a Chris
 * I made buildings with AI work correctly (ie really big robots should be
 * buildings)
 * anim to and from states are now shorts instead of bytes
 *
 * 197   1/29/99 5:09p Chris
 * Made changes for ROCKS
 *
 * 196   1/29/99 12:48p Matt
 * Rewrote the doorway system
 *
 * 195   1/18/99 8:07p Chris
 * Added the no-collide same flag (for flocks and nests)
 *
 * 194   1/18/99 2:46p Matt
 * Combined flags & flags2 fields in object struct
 *
 * 193   1/15/99 5:59p Chris
 *
 * 192   1/13/99 6:54a Jeff
 * fixed #includes for linux
 *
 * 191   1/13/99 2:29a Chris
 * Massive AI, OSIRIS update
 *
 * 190   1/01/99 4:53p Chris
 * Fixed a problem with long-rays causing unnecessary asserts
 *
 * 189   1/01/99 4:10p Chris
 * Added some const parameters, improved ray cast object collide/rejection
 * code
 *
 * 188   11/23/98 11:06a Chris
 * Added support for finding just AIs and Players with
 * fvi_QuickDistObjectList
 *
 * 187   11/02/98 6:15p Chris
 * Room AABBs get saved with the level and the sort_face and dec_sort_face
 * list s have been removed
 *
 * 186   10/29/98 5:20p Chris
 * Player ships collide smaller now
 *
 * 185   10/22/98 10:25p Kevin
 * took out volatile
 *
 * 184   10/22/98 10:25p Chris
 * Fixed a major bug with fvi_QuickXList()  -- preincrement f@cked me.
 *
 * 183   10/22/98 9:46p Chris
 *
 * 182   10/22/98 9:45p Chris
 * Added some asserts
 *
 * 181   10/21/98 9:41p Chris
 * Improved walking code!
 *
 * 180   10/21/98 7:44a Chris
 * Tweaked player wall collisions
 *
 * 179   10/20/98 8:05p Sean
 * Fixes callers with a start room of -1
 *
 * 178   10/20/98 6:58p Chris
 * My god - improved the cylinder code.  :)
 *
 * 177   10/18/98 5:36p Chris
 *
 * 176   10/17/98 12:25p Chris
 * Fixed attached flares
 *
 * 175   10/16/98 8:33p Chris
 * Fixed attached flare problem
 *
 * 174   10/16/98 6:19p Chris
 * More linked object fixes
 *
 * 173   10/16/98 12:27a Chris
 * Improved collision code.  Finish it tomarrow.
 *
 * 172   10/15/98 12:22a Matt
 * Fixed compile warnings
 *
 * 171   9/22/98 6:42p Dan
 * Temp fix
 *
 * 170   9/22/98 12:02p Matt
 * Fixed compile warnings
 *
 * 169   9/01/98 12:04p Matt
 * Ripped out multi-face portal code
 *
 * 168   8/28/98 4:33p Chris
 *
 * 167   8/17/98 3:14p Chris
 * Fixed a bug with the FQ_IGNORE_WALLS flag that caused precomputed
 * weapons not to relink into the mine from the terrain.
 *
 * 166   8/03/98 3:59p Chris
 * Added support for FQ_IGNORE_WEAPONS, added .000001 attach code, fix a
 * bug in polymodel collision detection
 *
 * 165   7/31/98 11:52a Chris
 * Weapons can be persistent.  Added ability for objects to be manually
 * set for no object collisions.
 *
 * 164   7/23/98 4:08p Chris
 *
 * 163   7/23/98 12:46p Chris
 * Added a flag to ignore external rooms
 *
 * 162   7/23/98 12:31p Chris
 *
 * 161   7/22/98 6:48p Chris
 *
 * 160   7/22/98 6:42p Chris
 * A relink fix for points on a wall
 *
 * 159   7/22/98 2:31p Chris
 * Fixed an odd bug with objects moving farther than a long ray externally
 * (never happened until I did the weapon pre-compute stuff)
 *
 * 158   7/21/98 2:14p Chris
 * Some FVI speedups - not done
 *
 * 157   7/17/98 9:56a Chris
 * Intermediate checkin
 *
 * 156   7/16/98 8:29p Chris
 * Partial implementation of the new collide code
 *
 * 155   7/02/98 2:51p Chris
 * Added a fast line to bbox function
 *
 * 154   6/15/98 6:08p Chris
 * Hit_type to hit_type[0]
 *
 * 153   6/15/98 6:58a Chris
 * Comment'ed out most bbox related additions
 *
 * 152   6/03/98 6:50p Chris
 * Fixed some infinity bugs
 *
 * 151   6/03/98 6:42p Chris
 * Added multipoint collision detection an Assert on invalid (infinite
 * endpoint).
 *
 * 150   6/03/98 12:34p Chris
 * Fixed a potential bug in subdivided rays
 *
 * 149   6/01/98 9:23p Chris
 * Removed a mprintf
 *
 * 148   6/01/98 9:23p Chris
 * Fixed a bunch of collision problems
 *
 * 147   6/01/98 11:38a Chris
 * Made FQ_SOLID_PORTALS more effective
 *
 * 146   5/26/98 10:45a Keneta
 * Shockwaves should not collide with anything
 *
 * 145   5/22/98 2:42p Sean
 * FROM JASON:Upped max next portals to 50
 *
 * 144   5/20/98 1:46p Chris
 * Fixed a bug in QuickDistFaceList
 *
 * 143   5/18/98 11:59a Chris
 *
 * 142   5/05/98 5:23p Chris
 * Faster external room collisions with FQ_EXTERNAL_ROOMS_AS_SPHERE
 *
 * 141   5/04/98 10:17a Chris
 * Improved checking
 *
 * 140   5/04/98 9:40a Chris
 * Improved Physics_player_verbose stuff
 *
 * 139   5/04/98 12:16a Chris
 * More debugging
 *
 * 138   5/03/98 10:59p Chris
 * Fixed problems with external rooms
 *
 * 137   5/03/98 8:36p Chris
 * Additional debug info
 *
 * 136   4/23/98 5:58p Chris
 * OOps.
 *
 * 135   4/23/98 5:44p Chris
 * Working on Door collisions
 *
 * 134   4/20/98 6:56p Chris
 * Removed sphere check for poly2sphere (just AABB)
 *
 * 133   4/20/98 5:48p Chris
 * Patching a collision bug
 *
 * 132   4/13/98 5:34p Chris
 * Sped up fvi.  check_hit_objs is not called at all for fvi calls with
 * FQ_CHECK_OBJS set
 *
 * 131   4/13/98 4:41p Chris
 *
 * 130   4/13/98 3:51p Chris
 * Added sphere bubbles to weapons (when wanted)  and fixed a small FVI
 * bug
 *
 * 129   4/13/98 2:21p Chris
 * Fixed some collision problems dealing with AABBs and Polymodel paging
 * in.
 *
 * 128   4/09/98 12:05p Chris
 * Added parenting for all object types.  :)
 *
 * 127   4/08/98 7:38p Chris
 *
 * 126   4/08/98 2:38p Chris
 * Fixed powerup pickup bug
 *
 * 125   4/07/98 8:14p Chris
 * Made powerups easier to hit
 *
 * 124   4/06/98 4:09p Chris
 * Made f_correcting a bool -- NOTE: chrishack -- physics need to handle
 * powerups in a good manner (flying through them).
 *
 * 123   4/06/98 2:54p Chris
 * TEMPORARY FIX -- Object collisions are of less priority than wall
 * collisions -- keep this until we have multipoint collision detection --
 * chrishack
 *
 * 122   4/03/98 3:35p Chris
 * Fixed problems with FQ_LIGHTMAP_ONLY (has to include external rooms)
 *
 * 121   4/03/98 11:27a Chris
 * Doubled the size of powerups
 *
 * 120   4/01/98 3:51p Chris
 *
 * 119   4/01/98 3:46p Chris
 * Added hack for powerups and players
 *
 * 118   3/25/98 1:17p Chris
 * Removed a bug.
 *
 * 117   3/24/98 2:57p Chris
 * Fixed a bug in the QuickDistObjectList function
 *
 * 116   3/23/98 11:37a Chris
 * Added the f_lightmap_only parameter to fvi_QuickDistObjectList
 *
 * 115   3/23/98 11:18a Chris
 * Added int fvi_QuickDistObjectList(vector *pos, int init_room_index,
 * float rad, int16_t *object_index_list, int max_elements)
 *
 *
 * 114   3/18/98 10:12p Chris
 * Added some more bug checking.  If 2 objects radii add up to zero, then
 * let me know.  :)
 *
 * 113   3/18/98 5:01p Chris
 * Fixed a bug with the zero rad wall collision code
 *
 * 112   3/18/98 4:50p Chris
 * Added a speed opt. for zero radius wall collision objects
 *
 * 111   3/18/98 4:31p Chris
 * Speed up fvi and fixed some bugs
 *
 * 110   3/17/98 3:36p Chris
 * Fixed bugs in the relink code and the QuickDistFaceList code.  (They
 * where using fvi_max_xyz and fvi_min_xyz)
 *
 * 109   3/17/98 11:33a Chris
 * Improved performance (AABB updates on poly-sphere collide)
 *
 * 108   3/16/98 9:55p Chris
 * Working on optimizing fvi
 *
 * 107   3/16/98 8:25p Chris
 * Fixed bugs with the new fvi code
 *
 * 106   3/16/98 6:55p Chris
 * Temporary fix.  There is a problem somewhere in the new optimazations.
 *
 * 105   3/16/98 5:50p Chris
 * Added sorted face lists for fvi
 *
 * 104   3/16/98 2:52p Chris
 * Moved some asserts for FQ_NO_RELINK
 *
 * 103   3/16/98 1:13p Chris
 * Speed up
 *
 * 102   3/16/98 12:50p Chris
 * Speed up?
 *
 * 101   3/16/98 9:46a Chris
 * Improved the speed
 *
 * 100   3/16/98 9:43a Chris
 * Added FQ_NO_RELINK
 *
 * 99    3/16/98 9:25a Chris
 * Sped up fvi
 *
 * 98    3/13/98 5:55p Chris
 * Added the new collision spheres
 *
 * 97    2/23/98 6:50p Jason
 * changes to help facilitate fast lighting with shadow volumes
 *
 *
 * 96    2/20/98 1:45p Chris
 * Milestone fixes
 *
 * 95    2/19/98 6:17p Chris
 * Added some debug info
 *
 * 94    2/15/98 2:08p Chris
 *
 * 93    2/12/98 2:07p Chris
 * Fixed the trigger face list
 *
 * 92    2/11/98 10:04p Chris
 * Added FQ_ONLY_DOOR_OBJ and fixed a bug with FQ_ONLY_PLAYER_OBJ
 *
 * 91    2/10/98 2:45p Chris
 * Made objects not get checked if FQ_CHECK_OBJECTS is not set.
 * (OBJ_ROOMS are still checked)
 *
 * 90    2/10/98 2:41p Chris
 * Fixed a problem with large objects beening hit when they shouldn't
 *
 * 89    2/06/98 3:47p Brent
 * CHRIS -- I made the MAX_CELLS_VISITED equal to the number of cells on
 * the terrain.  This is insane, but it allows for the placement of HUGE
 * objects.  :)  -- Thanx Brent.
 *
 * 88    2/06/98 2:57a Chris
 * Added point_collide_with_walls and ignore_robot_collisions
 *
 * 87    2/05/98 1:58p Chris
 * Made the cellnum be a roomnum for hitting terrain
 *
 * 86    2/04/98 6:23p Matt
 * Changed object room number to indicate a terrain cell via a flag.  Got
 * rid of the object flag which used to indicate terrain.
 *
 * 85    2/02/98 8:19p Chris
 * Fixed the hung on wall problem and the powerup problem.
 *
 * 84    1/29/98 5:50p Matt
 * Changed old camera object type to be viewer object (for editor), and
 * now camera objects will just be for game cameras.
 *
 * 83    1/20/98 12:25p Luke
 * CHRIS-- Added a check to make sure that the startseg is not an external
 * room
 *
 * 82    1/20/98 12:22p Chris
 * Removed the attempting to patch mprintf
 *
 * 81    1/19/98 5:05p Chris
 * Added ceiling collisions for players and powerups
 *
 * 80    1/14/98 7:56p Jason
 * don't do obj relink if you're in the same room
 *
 * 79    1/07/98 4:11p Jason
 * CHRIS -- Fixed stupid mistake with edge fix -- .y values where getting
 * misassigned.
 *
 * 78    1/06/98 7:14p Chris
 * Made edge of world less dangerous
 *
 * 77    12/22/97 6:19p Chris
 * Moved weapon battery firing sound off the projectile (weapon) and into
 * the weapon battery.
 *
 * 76    12/18/97 12:25p Chris
 * Sub-divide long rays and use a better flat-line algorithm for the
 * terrain fvi.
 *
 * 75    12/16/97 6:11p Chris
 * Attempt to improve FVI for outside
 *
 * 74    12/03/97 2:36p Sean
 * Added another code fix for external room problems
 *
 * 73    11/24/97 11:41a Chris
 * Fixed a small glitch in the fvi_check_node stuff
 *
 * 72    11/24/97 11:01a Chris
 * Removed a debugging assert
 *
 * 71    11/24/97 10:15a Chris
 * Improved fvi
 *
 * 70    11/21/97 5:26p Jason
 * checked in so chris can find a bug
 *
 * 68    11/20/97 6:02p Chris
 * Fixed the quick cell list code
 *
 * 67    11/17/97 5:46p Chris
 * Fixed a problem in line_to_sphere1 it -- see history for diff.
 *
 * 66    11/11/97 8:39p Chris
 * Passed the wrong object to PolyCollide when doing the relative motion
 * mapping of polygon moving and hitting a sphere.
 *
 * 65    11/11/97 11:26a Sean
 * CHRIS -- increased MAX_NEXT_PORTALS.  It seems that radiosity rays can
 * cross a lot of portals in a single room (in this case it was over 5)...
 * Beefed it up to 20.  It that causes a problem, it can be beefed up more
 * without any disadvantage.
 *
 * 64    11/03/97 5:10p Jason
 * Fixed by Chris, forgot to assume fvi_terrain_obj_visit_list to the max
 * cells visited limit.
 *
 * 63    10/29/97 3:39p Matt
 * Got rid of compiler warning
 *
 * 62    10/28/97 5:46p Chris
 * Added a quick cell list function
 *
 * 61    10/28/97 12:20p Chris
 * Added support to ignore wall collisions
 *
 * 60    10/22/97 4:33p Chris
 * Improved default hit_obj_type collision types (i.e. when the ray is
 * cast with a -1 as its index)
 *
 * 59    10/22/97 4:30p Chris
 * We can now slew between the mine and the terrain
 *
 * 58    10/22/97 12:42p Chris
 * Incremental BBOX stuff and added support for subobject hit returned
 * from fvi
 *
 * 57    10/21/97 4:14p Chris
 * Incremental improvements to the fvi/physics/collide integration
 *
 * 56    10/20/97 3:51p Chris
 * Fixed a problem with the POLY_BBOX collide code
 *
 * 55    10/20/97 3:43p Chris
 * Added more collide code
 *
 * 54    10/20/97 11:55a Chris
 * Added some support for the new collide system.
 *
 * 53    10/15/97 5:08p Chris
 * Fixed the collision_side_pnt and added a relative position/velocity
 * check
 *
 * 52    10/14/97 6:56p Chris
 * In middle of some incremental improvements
 *
 * 51    10/07/97 3:51p Chris
 * Added support for object backface checking -- external rooms are
 * classified as objects
 *
 * 50    10/06/97 2:36p Chris
 * Improved and fixed fvi.
 *
 * 49    10/06/97 12:36p Chris
 * Intermediate fix for terrain rendering problems
 *
 * 48    10/06/97 10:52a Jason
 * added "FQ_IGNORE_NON_LIGHTMAP_OBJECTS" flag
 *
 * 47    10/05/97 6:14a Chris
 * Added terrain speedup.  Allowed poly-collision for weapons.
 *
 * 46    10/03/97 6:24p Chris
 * Corrected a mistype to disallow all camera collisions
 *
 * 45    10/03/97 3:34p Chris
 * No camera collisions
 *
 * 44    10/02/97 7:02p Chris
 * Removed LOD optimazation
 *
 * 43    10/02/97 6:44p Chris
 * Added support for quick fvi_checks
 *
 * 42    10/02/97 5:38p Chris
 * Added support for invisible terrain tiles
 *
 * 41    10/02/97 11:34a Chris
 * Added support for external room collisions
 *
 * 40    9/19/97 6:58p Chris
 * Fixed some bugs with the big object system and removed some annoying
 * mprintf's
 *
 * 39    9/19/97 4:56p Jason
 * FROM CHRIS: Took out clutter assert in check_hit_obj
 *
 * 38    9/19/97 1:01p Chris
 * Added better large object support
 *
 * 37    9/17/97 3:24p Chris
 *
 * 36    9/17/97 11:01a Chris
 * Working on poly collision code (not done) and removed dependance on
 * segment.h
 *
 * 35    9/15/97 6:24p Chris
 * Added a second chance check if no rooms are initialy found
 *
 * 34    9/15/97 4:43p Craig
 *
 * 33    9/15/97 5:20a Chris
 * Added sphere2poly support.  Rearranged object collisions.
 *
 * 32    9/12/97 6:36p Chris
 * Added collision terrain segment support
 * Added some LARGE OBJECT collision support
 *
 * 31    9/12/97 10:37a Chris
 * Added a zero radius speed up for fvi terrain.
 *
 * 30    9/11/97 7:08p Matt
 * Fixed minor bug
 *
 * 29    9/11/97 3:10p Chris
 *
 * 28    9/11/97 3:08p Chris
 * Added support for weapons flying through holes in transparent textures.
 *
 * 27    9/11/97 1:58p Chris
 * Added ability to ignore moving objects
 *
 * 26    9/11/97 12:43p Chris
 * Added new support for face_physics_info from room.h.  Also, changed how
 * we detect portals.  Also added recorded faces.
 *
 * 25    9/10/97 11:58a Matt
 * Ripped out some unused commented-out code
 *
 * 24    9/02/97 6:32p Jason
 * Fixed fvi_rooms_visited array in the quick face list code.
 *
 * 23    9/02/97 4:16p Chris
 * Fixed the 'clockwise' vertices problem with backface checking.  I.e.
 * vertices should be oppositely ordered when negating the face normal.
 *
 * 22    9/02/97 3:06p Chris
 * Fixed stupid bug
 *
 * 21    9/02/97 3:04p Chris
 * Fixed quick face list bug
 *
 * 20    9/02/97 11:57a Chris
 * Fix a bug in the quick_dist stuff
 *
 * 19    9/02/97 11:41a Chris
 * Added support the the quick face/room from distance function
 *
 * 18    8/26/97 1:44p Chris
 * Fixed a bug in the clipping of a vector to the terrain boundaries.
 *
 * 17    8/25/97 6:21p Chris
 * Portals only crossed at centerpoint
 *
 * 16    8/20/97 3:16p Chris
 * Working on the edge of terrain problems
 *
 * 15    8/18/97 1:48a Chris
 * Added AABB speedups.  Added a new way to do inter-room and mine-terrain
 * movements.  Added a fvi_relink that correctly uses the hit_seg
 * FVIF_OUTSIDE flag
 *
 * 14    8/12/97 3:47p Chris
 * Updated the AABB stuff.
 *
 * 13    8/12/97 1:13p Chris
 * Added AABBs.
 *
 * 12    8/11/97 5:49p Chris
 * Allow for some terrain to mine flights
 *
 * 11    8/05/97 4:43p Chris
 * Added support for fly by textures.  Added support (fly by) support for
 * fly thru textures.
 *
 * 10    8/04/97 5:35p Chris
 * Added support for back face collisions and new fvi code
 *
 * 9     8/04/97 12:37p Chris
 * We now support fvi in Rooms  :)
 *
 * 8     7/31/97 3:12p Chris
 * Fixed a problem with the new iterator based radius checker
 *
 * 7     7/28/97 1:16p Chris
 * Allow for player and robots to be hit by weapons and correct parenting
 * of weapons.
 *
 * 6     7/23/97 6:27p Jason
 * added code to support terrain simplification
 *
 * 5     7/23/97 5:14p Chris
 * Used a faster iterator for visiting terrain segments
 *
 * 4     7/15/97 7:29p Chris
 * Added a sound for helicopter blades.
 *
 * 3     7/15/97 5:35p Chris
 * New AI code
 *
 * 39    6/24/97 4:01p Chris
 * Fixed another problem with the .y dependancy.
 *
 * 38    6/24/97 3:53p Chris
 * Fixed a bug with the new .y dependancy
 *
 * 37    6/24/97 3:29p Chris
 * Removed the .pos dependancy for terrain and am using the .y instead
 *
 * 36    6/18/97 3:52p Chris
 *
 * 35    6/12/97 2:40p Chris
 *
 * 34    6/10/97 6:23p Chris
 *
 * 33    6/10/97 5:32p Chris
 *
 * 32    6/10/97 5:32p Chris
 *
 * 31    6/05/97 7:22p Matt
 * When get list of faces for mesh side, return the normals for each face.
 * Also, added a function to return the four floating verts in a side.
 *
 * 30    5/27/97 5:36p Chris
 *
 * 29    5/27/97 5:36p Chris
 * Incremental.
 *
 * 28    5/27/97 5:05p Chris
 * Added physics for floating segments.  We need more
 * code if all mesh faces get their own normals or if mesh sides are used
 * for non-floating other sides.
 *
 * 27    5/13/97 5:52p Chris
 * Added ability to exit and enter mine.  Also did some
 * incremental improvements.
 *
 * 26    5/06/97 11:18a Chris
 *
 * 25    5/05/97 5:41a Chris
 * Added some better polygon collision code.  It is still rough though.
 *
 * 24    4/17/97 3:10p Chris
 * Added edge of world object delete.  Also did some incremental
 * improvements.
 *
 * 23    4/16/97 3:25p Chris
 * Commented the code
 *
 * 22    4/16/97 1:39p Chris
 *
 * 21    4/15/97 6:31p Chris
 * Added support for self-correcting and non-correcting
 * sphere collisions.  This allows objects to ignore sphere
 * collisions that happen at initial positions, but allows the walls to
 * self-correct edges.
 *
 * 20    4/15/97 4:13p Chris
 *
 * 19    4/11/97 3:05a Chris
 * Added some terrain collision code.  Need ground.
 *
 * 18    3/24/97 6:29p Chris
 *
 * 17    3/24/97 3:50p Chris
 * Removed some annoying, but informative physics info.
 *
 * 16    3/24/97 3:27p Chris
 * Fixed some corner problems
 *
 * 15    3/24/97 11:43a Chris
 *
 * 14    3/21/97 7:08p Chris
 * Working on rewritting FVI and Physics
 *
 * 13    3/20/97 6:26p Chris
 *
 * 12    3/20/97 6:18p Chris
 *
 * 11    3/20/97 6:13p Chris
 *
 * 10    3/20/97 6:06p Chris
 *
 * 9     3/17/97 7:36p Chris
 * Made it easier to hit objects...  :)
 *
 * 8     3/17/97 11:55a Chris
 * Added more debug info.
 *
 * 7     3/14/97 12:18p Chris
 * Tweak physics and remove braking for now.
 *
 * 6     3/13/97 1:17p Chris
 * Incremental improvements...
 *
 * 5     3/12/97 6:31p Chris
 * Alpha 1.0 of player "flying" in the mine
 *
 * 4     3/07/97 7:01p Chris
 * Segments now have perfect edges and corners  :)
 *
 * 3     3/03/97 1:03p Chris
 *
 * 2     3/03/97 5:52a Chris
 * Pre-alpha .01
 *
 * 1     3/03/97 3:25a Chris
 *
 * $NoKeywords: $
 */

// Mini implementation of find_vector_intersection (FVI).
//
// Replaces the win32 Descent3Core FVI.  Traces a segment p0 -> p1 through the
// mine, crossing portals and stopping at the first wall.  Unlike the full
// engine version this is a segment (radius 0) tracer: object/terrain
// collisions and sphere extents are not modelled, which is adequate for the
// editor (object dragging, point casts, LOS checks, slew room tracking).

#include "findintersection.h"

#include <cmath>
#include <cstring>

#include "room.h"
#include "vecmat.h"

namespace {

// True when pnt (which lies on the plane of face f) is inside the face polygon.
bool pointInsideFace(const room *rp, int faceIndex, const vector3 &pnt) {
  const face *fp = &rp->faces[faceIndex];
  const int n = fp->num_verts;
  if (n < 3)
    return false;

  // Project onto the 2D plane of the face (drop the dominant normal axis).
  const float ax = std::fabs(fp->normal.x());
  const float ay = std::fabs(fp->normal.y());
  const float az = std::fabs(fp->normal.z());
  const bool dropX = (ax >= ay && ax >= az);

  float sx[64] = {};
  float sy[64] = {};
  for (int v = 0; v < n; ++v) {
    const vector3 rel = rp->verts[fp->face_verts[v]] - pnt;
    if (dropX) {
      sx[v] = rel.y();
      sy[v] = rel.z();
    } else if (ay >= az) {
      sx[v] = rel.x();
      sy[v] = rel.z();
    } else {
      sx[v] = rel.x();
      sy[v] = rel.y();
    }
  }

  bool inside = false;
  for (int i = 0, j = n - 1; i < n; j = i++) {
    if ((sy[i] > 0.0f) != (sy[j] > 0.0f) &&
        (0.0f < (sx[j] - sx[i]) * (0.0f - sy[i]) / (sy[j] - sy[i]) + sx[i]))
      inside = !inside;
  }
  return inside;
}

// Parametric t in [0,1] where the segment crosses the face plane.
bool facePlaneIntersect(const vector3 &p0, const vector3 &seg, const vector3 &v0, const vector3 &normal, float &tOut) {
  const float denom = vm_DotProduct(&normal, &seg);
  if (std::fabs(denom) < 1e-6f)
    return false;
  const float t = (vm_DotProduct(&normal, &v0) - vm_DotProduct(&normal, &p0)) / denom;
  if (!(t > 0.0f) || !(t < 1.0f))
    return false;
  tOut = t;
  return true;
}

// Strictly-inside test: a point is inside a room when it is on the interior
// side of every face plane.
bool pointInsideRoom(const room *rp, const vector3 &pnt) {
  for (int f = 0; f < rp->num_faces; ++f) {
    const face *fp = &rp->faces[f];
    if (fp->num_verts < 3)
      continue;
    const vector3 rel = pnt - rp->verts[fp->face_verts[0]];
    if (vm_DotProduct(&fp->normal, &rel) > 0.0f)
      return false;
  }
  return true;
}

} // namespace

int fvi_FindIntersection(fvi_query *fq, fvi_info *ans, bool no_subdivision) {
  (void)no_subdivision;
  memset(ans, 0, sizeof(fvi_info));
  ans->hit_face[0] = -1;
  ans->hit_object[0] = -1;

  const vector3 p0 = *fq->p0;
  const vector3 p1 = *fq->p1;
  const vector3 seg = p1 - p0;
  const float segLen = vm_GetMagnitude(&seg);

  if (fq->startroom < 0 || fq->startroom > Highest_room_index) {
    // Starting outside the mine: treat as a free move, but report the room the
    // endpoint landed in, if any.
    for (int r = 0; r <= Highest_room_index; ++r) {
      if (Rooms[r].used && pointInsideRoom(&Rooms[r], p1)) {
        ans->hit_pnt = p1;
        ans->hit_room = r;
        ans->hit_dist = segLen;
        return HIT_NONE;
      }
    }
    ans->hit_pnt = p1;
    ans->hit_room = fq->startroom;
    ans->hit_dist = segLen;
    return HIT_NONE;
  }

  if (segLen < 1e-6f) {
    ans->hit_pnt = p0;
    ans->hit_room = fq->startroom;
    ans->hit_dist = 0.0f;
    return HIT_NONE;
  }

  const bool ignoreWalls = (fq->flags & FQ_IGNORE_WALLS) != 0;
  const bool solidPortals = (fq->flags & FQ_SOLID_PORTALS) != 0;

  int roomIdx = fq->startroom;
  float rayT = 0.0f;
  ans->hit_room = roomIdx;
  int roomCount = 0;
  if (roomCount < MAX_FVI_SEGS)
    ans->roomlist[roomCount++] = roomIdx;

  for (int iter = 0; iter < MAX_FVI_SEGS; ++iter) {
    if (roomIdx < 0 || roomIdx > Highest_room_index) {
      ans->hit_pnt = p1;
      ans->n_rooms = roomCount;
      return HIT_NONE;
    }
    const room *rp = &Rooms[roomIdx];
    if (!rp->used) {
      ans->hit_pnt = p1;
      ans->n_rooms = roomCount;
      return HIT_NONE;
    }

    // Nearest wall and nearest crossable portal ahead of rayT in this room.
    float bestWallT = 2.0f;
    int bestWallFace = -1;
    float bestPortalT = 2.0f;
    int bestPortalFace = -1;
    for (int f = 0; f < rp->num_faces; ++f) {
      const face *fp = &rp->faces[f];
      const int nv = fp->num_verts;
      if (nv < 3)
        continue;
      const vector3 v0 = rp->verts[fp->face_verts[0]];
      float t = 0.0f;
      if (!facePlaneIntersect(p0, seg, v0, fp->normal, t))
        continue;
      if (t <= rayT + 1e-5f)
        continue;
      const vector3 pnt = p0 + seg * t;
      if (!pointInsideFace(rp, f, pnt))
        continue;
      const bool isPortal = (fp->portal_num >= 0);
      if (isPortal && !solidPortals) {
        if (t < bestPortalT) {
          bestPortalT = t;
          bestPortalFace = f;
        }
      } else {
        if (t < bestWallT) {
          bestWallT = t;
          bestWallFace = f;
        }
      }
    }

    if (!ignoreWalls && bestWallFace >= 0 && bestWallT <= bestPortalT) {
      // Blocked by a wall (or a solid portal).
      ans->hit_pnt = p0 + seg * bestWallT;
      ans->hit_room = roomIdx;
      ans->hit_face[0] = bestWallFace;
      ans->hit_dist = bestWallT * segLen;
      ans->hit_wallnorm[0] = rp->faces[bestWallFace].normal;
      ans->num_hits = 1;
      ans->n_rooms = roomCount;
      ans->hit_type[0] = HIT_WALL;
      ans->hit_face_pnt[0] = ans->hit_pnt;
      ans->hit_face_room[0] = roomIdx;
      ans->hit_face[0] = bestWallFace;
      return HIT_WALL;
    }

    if (bestPortalFace >= 0) {
      const portal *pp = &rp->portals[rp->faces[bestPortalFace].portal_num];
      roomIdx = pp->croom;
      rayT = bestPortalT;
      if (roomCount < MAX_FVI_SEGS)
        ans->roomlist[roomCount++] = roomIdx;
      ans->hit_room = roomIdx;
      continue;
    }

    // Reached the endpoint without obstruction.
    ans->hit_pnt = p1;
    ans->hit_room = roomIdx;
    ans->hit_dist = segLen;
    ans->n_rooms = roomCount;
    return HIT_NONE;
  }

  ans->hit_pnt = p1;
  ans->n_rooms = roomCount;
  return HIT_NONE;
}
#endif
