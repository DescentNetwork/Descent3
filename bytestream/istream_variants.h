#ifndef STREAM_VARIANTS_H
#define STREAM_VARIANTS_H

#include "posix_stream.h"

enum class stream_variant
{
  i386,
  amd64,
  powerpc,
};

template <stream_variant sv>
struct byte_istream_variant : byte_istream
{
  byte_istream_variant<sv>& read(void* s, size_t length)
  {
    byte_istream::read(s, length);
    return *this;
  }
};

using i386_istream = byte_istream_variant<stream_variant::i386>;
using amd64_istream = byte_istream_variant<stream_variant::amd64>;
using powerpc_istream = byte_istream_variant<stream_variant::powerpc>;

template <stream_variant sv>
struct posix_variant_istream : posix_istream, byte_istream_variant<sv>
{
  byte_istream_variant<sv>& operator>>(int8_t& data)
  {
    data = get();
    return *this;
  }
  byte_istream_variant<sv>& operator>>(uint8_t& data)
  {
    data = get();
    return *this;
  }
};

template <stream_variant sv, typename T,
          std::enable_if_t<(std::is_arithmetic<T>::value && sizeof(T) > 1),
                           bool> = true>
byte_istream_variant<sv>& operator>>(byte_istream_variant<sv>& input, T& data)
{
  T sdata;
  input.read(&sdata, sizeof(T));
  data = le_to_host(sdata);
  return input;
}

template <typename T,
          std::enable_if_t<(std::is_arithmetic<T>::value && sizeof(T) > 1),
                           bool> = true>
powerpc_istream& operator>>(powerpc_istream& input, T& data)
{
  T sdata;
  input.read(&sdata, sizeof(T));
  data = be_to_host(sdata);
  return input;
}

#endif
