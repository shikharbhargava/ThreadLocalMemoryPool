#ifndef _ARG_TYPES_H_
#define _ARG_TYPES_H_

#include "utils.h"

namespace Arguments
{
class ArgTypes;
}

template <>
const Arguments::ArgTypes &utils::from_string<Arguments::ArgTypes>(const std::string &);

template <>
const Arguments::ArgTypes &utils::from_string<Arguments::ArgTypes>(const char*);

namespace Arguments
{

class ArgTypes
{
public:
  friend class std::hash<ArgTypes>;
  enum type
  {
    STRING,
    INT,
    DOUBLE,
    FILE,
    BOOL,

    NUM_ARG_TYPES
  };
  ArgTypes() = default;
  ArgTypes(const ArgTypes::type& type) : m_type(type) {}
  operator type() const noexcept { return m_type; }
  operator std::string() const noexcept;
  ArgTypes& operator =(const ArgTypes& other);
  bool operator ==(const ArgTypes& other) const noexcept { return m_type == other.m_type; }
  bool operator ==(const ArgTypes::type& other) const noexcept { return m_type == other; }
  bool operator ==(const char* other) const noexcept { return m_type == utils::from_string<ArgTypes>(other); }
  bool operator !=(const ArgTypes& other) const noexcept { return m_type != other.m_type; }
private:
  type m_type = NUM_ARG_TYPES;
};

} // namespace Arguments

namespace std
{
  template <>
  struct hash<Arguments::ArgTypes>
  {
    size_t operator()(const Arguments::ArgTypes &a) const
    {
      return a.m_type;
    }
  };
}
#endif // _ARGTYPES_
