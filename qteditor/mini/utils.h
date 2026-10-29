#pragma once
#include <optional>
#include <functional>
#include <cstdint>
#include "posix_stream.h"

constexpr uint32_t operator "" _ID(const char* const str, std::size_t len) {
  if (len != 4)
    throw "ID strings requires exactly 4 characters";
  return (static_cast<uint32_t>(str[3]) << 24) |
         (static_cast<uint32_t>(str[2]) << 16) |
         (static_cast<uint32_t>(str[1]) << 8)  |
         static_cast<uint32_t>(str[0]);
}

using index_t = std::optional<uint32_t>;

inline byte_istream& operator >>(byte_istream& input, index_t& data)
{
  uint32_t val;
  input >> val;
  if(val == UINT32_MAX)
    data.reset();
  else
    data = val;
  return input;
}

inline byte_ostream& operator <<(byte_ostream& output, const index_t& data)
{
  return output << data.value_or(UINT32_MAX);
}

inline index_t u16_to_index(const uint16_t data)
{
  if(data == UINT16_MAX)
    return std::nullopt;
  return static_cast<uint32_t>(data);
}

inline index_t u8_to_index(const uint8_t data)
{
  if(data == UINT8_MAX)
    return std::nullopt;
  return static_cast<uint32_t>(data);
}

inline uint16_t index_to_u16(const index_t& data)
  { return data ? static_cast<uint16_t>(*data) : UINT16_MAX; }

inline uint8_t index_to_u8(const index_t& data)
  { return data ? static_cast<uint8_t>(*data) : UINT8_MAX; }

// Convert an index_t back to the legacy integer sentinel: nullopt -> -1.
inline int index_to_int(const index_t& data)
{
  return data ? static_cast<int>(*data) : -1;
}

// Convert a legacy integer index (-1 meaning "unset") to index_t.
inline index_t to_index(int index)
{
  return index == -1 ? index_t{} : index_t{static_cast<uint32_t>(index)};
}

template <typename T>
class optref {
private:
  std::optional<std::reference_wrapper<T>> m_opt;

public:
  // 1. Constructors
  constexpr optref() noexcept = default;
  constexpr optref(std::nullopt_t) noexcept : m_opt(std::nullopt) {}

  // Allow binding directly to an lvalue reference
  optref(T& ref) noexcept : m_opt(std::ref(ref)) {}

  // 2. Overload operator* for value access
  constexpr T& operator*() const {
    return m_opt->get();
  }

  // 3. Overload operator-> to return a raw pointer to the object
  constexpr T* operator->() const {
    return std::addressof(m_opt->get());
  }

  // 4. Boolean conversion to check if it has a value
  constexpr explicit operator bool() const noexcept {
    return m_opt.has_value();
  }

  // Optional: Expose underlying optional if needed
  constexpr bool has_value() const noexcept {
    return m_opt.has_value();
  }
};
