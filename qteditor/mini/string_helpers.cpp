#include "string_helpers.h"

#include <algorithm>
#include <cctype>

inline bool lower_same(char a, char b) { return std::tolower(a) == std::tolower(b); }
inline bool lower_less(char a, char b) { return std::tolower(a) < std::tolower(b); }

// Case-insensitive ASCII comparison (replaces the original stricmp).
bool match(const std::string &a, const std::string &b)
{ return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), lower_same); }
/*
bool less(const std::string &a, const std::string &b)
{
  auto pos = std::ranges::mismatch(a,b);
  return ???
}
*/
