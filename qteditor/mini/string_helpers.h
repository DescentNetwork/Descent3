#pragma once
#include <string>
#include <posix_stream.h>

bool match(const std::string &a, const std::string &b);
bool less(const std::string &a, const std::string &b);


struct p_string_t
{
  uint16_t length = 0;
  std::string& target;
  explicit p_string_t(std::string& str) : target(str) {}
  p_string_t(p_string_t&&) = default;
};

inline p_string_t pascal_string(std::string& target)
{
  return p_string_t(target);
}

inline byte_istream& operator >>(byte_istream& input, p_string_t&& data)
{
  input >> data.length;
  while(data.length--)
    data.target.push_back(static_cast<char>(input.get()));
  return input;
}


inline byte_ostream& operator <<(byte_ostream& output, const p_string_t& data)
{
  output << uint16_t(data.target.size());
  return output.write(data.target.data(), data.target.size());
}
