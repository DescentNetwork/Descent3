#ifndef FIXED_STRING_H
#define FIXED_STRING_H

#include <algorithm>
#include <array>
#include <istream>
#include <iterator>
#include <ostream>
#include <posix_stream.h>
#include <string>
#include <string_view>

template <std::size_t length>
struct fixed_string_t : std::array<char, length>
{
  constexpr fixed_string_t(std::string_view data) noexcept
  {
    std::array<char, length>::fill('\0');
    std::copy(std::begin(data), std::end(data), std::begin(*this));
  }

  constexpr fixed_string_t(char fill_with) noexcept
  {
    std::array<char, length>::fill(fill_with);
  }

  inline std::string string(void) const
  {
    return std::string(std::begin(*this),
                       std::find(std::begin(*this), std::end(*this), '\0'));
  }

  constexpr void operator=(const fixed_string_t<length>& other)
  {
    std::copy(std::begin(other), std::end(other), std::begin(*this));
  }

  inline void operator=(const std::string& other)
  {
    std::array<char, length>::fill('\0');
    std::copy_n(std::begin(other), std::min(length, other.size()),
                std::begin(*this));
  }
};

template <std::size_t length>
constexpr byte_ostream& operator<<(byte_ostream& output,
                                   const fixed_string_t<length>& fs)
{
  output.write(fs.data(), length);
  return output;
}

template <std::size_t length>
constexpr byte_istream& operator>>(byte_istream& input,
                                   fixed_string_t<length>& fs)
{
  input.read(fs.data(), length);
  return input;
}

template <std::size_t length>
constexpr std::ostream& operator<<(std::ostream& output,
                                   const fixed_string_t<length>& fs)
{
  std::copy_n(std::begin(fs), length, std::ostream_iterator<char>(output));
  return output;
}

template <std::size_t length>
constexpr std::istream& operator>>(std::istream& input,
                                   fixed_string_t<length>& fs)
{
  std::copy_n(std::istream_iterator<char>(input), length, std::begin(fs));
  return input;
}

#endif // FIXED_STRING_H
