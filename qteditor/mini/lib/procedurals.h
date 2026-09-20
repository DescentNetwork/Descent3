#ifndef PROCEDURALS_H
#define PROCEDURALS_H

#include <posix_stream.h>

struct procedural_t
{
  uint8_t type;
  uint8_t frequency;
  uint8_t speed;
  uint8_t size;
  uint8_t x1;
  uint8_t y1;
  uint8_t x2;
  uint8_t y2;
};

byte_istream& operator >>(byte_istream& input, procedural_t& data);
byte_ostream& operator <<(byte_ostream& output, const procedural_t& data);


struct procedurals_t
{
  std::array<uint16_t, 255> proc_palette;
  uint8_t heat = 200;
  uint8_t light = 1;
  uint8_t thickness = 4;
  float evaluation_time = 0.0f;

  uint8_t osc_value = 8;
  float osc_time = 0.0f;

  uint16_t num_proc_elements;
  std::vector<procedural_t> data;
};

byte_istream& operator >>(byte_istream& input, procedurals_t& data);
byte_ostream& operator <<(byte_ostream& output, const procedurals_t& data);

#endif
