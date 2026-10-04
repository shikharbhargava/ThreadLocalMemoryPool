#ifndef _UTILS_
#define _UTILS_

#include <string>

namespace utils
{
  template <class T>
  std::string to_string(const T &);

  template <class T>
  const T &from_string(const std::string &);

  template <class T>
  const T &from_string(const char *);

} // namespace utils

#endif // _UTILS_
