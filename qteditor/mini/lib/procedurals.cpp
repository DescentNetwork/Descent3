#include "procedurals.h"

byte_istream& operator >>(byte_istream& input, procedural_t& data)
{
  return input
         >> data.type
         >> data.frequency
         >> data.speed
         >> data.size
         >> data.x1
         >> data.y1
         >> data.x2
         >> data.y2;
}

byte_ostream& operator <<(byte_ostream& output, const procedural_t& data)
{
  return output
         << data.type
         << data.frequency
         << data.speed
         << data.size
         << data.x1
         << data.y1
         << data.x2
         << data.y2;
}

byte_istream& operator >>(byte_istream& input, procedurals_t& data)
{
  uint16_t num_proc_elements = 0;
  input
      >> data.proc_palette
      >> data.heat
      >> data.light
      >> data.thickness
      >> data.evaluation_time
      >> data.osc_time
      >> data.osc_value
      >> num_proc_elements;
  data.data.resize(num_proc_elements);

  if (num_proc_elements)
    input >> data.data;
  return input;
}

byte_ostream& operator <<(byte_ostream& output, const procedurals_t& data)
{
  uint16_t num_proc_elements = data.data.size();
  output
      << data.proc_palette
      << data.heat
      << data.light
      << data.thickness
      << data.evaluation_time
      << data.osc_time
      << data.osc_value
      << num_proc_elements;
  if (num_proc_elements)
    output << data.data;
  return output;
}
