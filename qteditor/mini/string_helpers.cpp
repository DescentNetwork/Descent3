#include "string_helpers.h"

#include <algorithm>
#include <cctype>

// Case-insensitive ASCII comparison (replaces the original stricmp).
bool match(const std::string &a, const std::string &b)
{
  return a.size() == b.size() &&
         std::equal(a.begin(), a.end(), b.begin(), [](unsigned char x, unsigned char y) {
           return std::tolower(x) == std::tolower(y);
         });
}
