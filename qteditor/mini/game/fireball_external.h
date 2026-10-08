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

#ifndef FIREBALL_EXTERNAL_H_
#define FIREBALL_EXTERNAL_H_

#include <cstdint>

// Indices into the fireball texture table
enum class fireball_texture : uint8_t {
  med_explosion_index2 = 0,
  small_explosion_index2 = 1,
  med_explosion_index = 2,
  med_explosion_index3 = 3,
  big_explosion_index = 4,
  billowing_index = 5,
  small_explosion_index = 6,
  med_smoke_index = 7,
  black_smoke_index = 8,
  blast_ring_index = 9,
  smoke_trail_index = 10,
  custom_explosion_index = 11,
  shrinking_blast_index = 12,
  smoldering_index = 13,
  shrinking_blast_index2 = 14,
  hot_spark_index = 15,
  cool_spark_index = 16,
  gradient_ball_index = 17,
  spray_index = 18,
  fading_line_index = 19,
  muzzle_flash_index = 20,
  ship_hit_index = 21,
  blue_blast_ring_index = 22,
  particle_index = 23,
  afterburner_index = 24,
  napalm_ball_index = 25,
  lightning_origin_indexa = 26,
  lightning_origin_indexb = 27,
  raindrop_index = 28,
  puddledrop_index = 29,
  gravity_field_index = 30,
  lightning_bolt_index = 31,
  invul_hit_index = 32,
  sine_wave_index = 33,
  axis_billboard_index = 34,
  default_corona_index = 35,
  headlight_corona_index = 36,
  star_corona_index = 37,
  sun_corona_index = 38,
  snowflake_index = 39,
  thick_lightning_index = 40,
  blue_fire_index = 41,
  rubble1_index = 42,
  rubble2_index = 43,
  water_splash_index = 44,
  shatter_index = 45,
  shatter_index2 = 46,
  billboard_smoketrail_index = 47,
  massdriver_effect_index = 48,
  blue_explosion_index = 49,
  gray_spark_index = 50,
  gray_lightning_bolt_index = 51,
  mercboss_massdriver_effect_index = 52,
};

// Fireball types
enum class fireball_type : uint8_t {
  explosion = 0,
  smoke = 1,
  effect = 2,
  billow = 3,
  spark = 4,
};

#endif
