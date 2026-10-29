#ifndef COLOR_H
#define COLOR_H
#include "posix_stream.h"

struct color3f
{
  float r, g, b;

  color3f& operator =(const color3f& o)
  {
    r = o.r;
    g = o.g;
    b = o.b;
    return *this;
  }
};

inline byte_istream& operator >>(byte_istream& input, color3f& data)
  { return input >> data.r >> data.g >> data.b; }

inline byte_ostream& operator <<(byte_ostream& output, const color3f& data)
  { return output << data.r << data.g << data.b; }


struct color4f
{
  float r, g, b, a;

  color4f& operator =(const color4f& o)
  {
    r = o.r;
    g = o.g;
    b = o.b;
    a = o.a;
    return *this;
  }
};

inline byte_istream& operator >>(byte_istream& input, color4f& data)
  { return input >> data.r >> data.g >> data.b >> data.a; }

inline byte_ostream& operator <<(byte_ostream& output, const color4f& data)
  { return output << data.r << data.g << data.b << data.a; }

#endif // COLOR_H
