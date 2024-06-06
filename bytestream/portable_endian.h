#ifndef PORTABLE_ENDIAN_H
#define PORTABLE_ENDIAN_H

#if __has_include(<endian.h>)
#include <endian.h>
#endif

#if __has_include(<libkern/OSByteOrder.h>)
#include <libkern/OSByteOrder.h>
#endif

#if __has_include(<sys/endian.h>)
#include <sys/endian.h>
#endif

#if __has_include(<sys/param.h>)
#include <sys/param.h>
#endif

#if __has_include(<gulliver.h>)
#include <gulliver.h>
#endif

#if defined(BYTE_ORDER) && !defined(__BYTE_ORDER)
#define __BYTE_ORDER BYTE_ORDER
#define __BIG_ENDIAN BIG_ENDIAN
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#define __PDP_ENDIAN PDP_ENDIAN
#endif

#if !defined(__LITTLE_ENDIAN)
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#define __PDP_ENDIAN 3412
#endif

#if defined(__BIGENDIAN__)
#define __BYTE_ORDER __BIG_ENDIAN
#elif defined(__LITTLEENDIAN__)
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif

#if __has_include(<bit>)
#include <bit>
#endif

#ifndef __cpp_lib_endian
namespace std
{
  enum class endian
  {
    little = __LITTLE_ENDIAN,
    big = __BIG_ENDIAN,
    native = __BYTE_ORDER
  };
}
#endif

#ifndef __cpp_lib_byteswap
#include <cstdint>

namespace std
{
  template <typename T>
  T byteswap(T);

  template <>
  constexpr uint16_t byteswap<uint16_t>(uint16_t x)
  {
    return ((x >> 8) & 0x00FF) | ((x << 8) & 0xFF00);
  }

  template <>
  constexpr uint32_t byteswap<uint32_t>(uint32_t x)
  {
    return ((x >> 24) & 0x000000FF) | ((x >> 8) & 0x0000FF00) |
           ((x << 8) & 0x00FF0000) | ((x << 24) & 0xFF000000);
  }

  template <>
  constexpr uint64_t byteswap<uint64_t>(uint64_t x)
  {
    return ((x >> 56) & 0x00000000000000FF) | ((x >> 40) & 0x000000000000FF00) |
           ((x >> 24) & 0x0000000000FF0000) | ((x >> 8) & 0x00000000FF000000) |
           ((x << 8) & 0x000000FF00000000) | ((x << 24) & 0x0000FF0000000000) |
           ((x << 40) & 0x00FF000000000000) | ((x << 56) & 0xFF00000000000000);
  }
} // namespace std
#endif

// to host
template <typename T>
constexpr T le_to_host(T x)
{
  if constexpr (std::endian::native == std::endian::little) return x;
  else
    return byteswap<T>(x);
}

template <typename T>
constexpr T be_to_host(T x)
{
  if constexpr (std::endian::native == std::endian::little) return x;
  else
    return byteswap<T>(x);
}

template <typename T>
constexpr T net_to_host(T x)
{
  return le_to_host(x);
}
template <typename T>
constexpr T host_to_net(T x)
{
  return net_to_host(x);
}
template <typename T>
constexpr T host_to_le(T x)
{
  return le_to_host(x);
}
template <typename T>
constexpr T host_to_be(T x)
{
  return be_to_host(x);
}

#endif /* PORTABLE_ENDIAN_H */
