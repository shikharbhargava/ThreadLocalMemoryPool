#include <unordered_map>

#include "ArgTypes.h"
#include "bi_unordered_map.h"

using namespace Arguments;

// class ArgTypes
static const ArgTypes ARG_TYPE_INVALID = ArgTypes::NUM_ARG_TYPES;

static const bi_unordered_map<ArgTypes, std::string> logTypeToString =
    {
        {ArgTypes::STRING, "STRING"},
        {ArgTypes::INT, "INT"},
        {ArgTypes::DOUBLE, "DOUBLE"},
        {ArgTypes::FILE, "FILE"},
        {ArgTypes::BOOL, "BOOL"},
        {ArgTypes::NUM_ARG_TYPES, "INVALID"}};

template <>
const ArgTypes &utils::from_string<ArgTypes>(const std::string &logTypeStr)
{
  const auto &it = logTypeToString.vfind(logTypeStr);
  if (it != logTypeToString.vend())
  {
    return it->second;
  }
  return ARG_TYPE_INVALID;
}

template <>
const ArgTypes &utils::from_string<ArgTypes>(const char *analytics)
{
  return utils::from_string<ArgTypes>(std::string(analytics));
}

ArgTypes::operator std::string() const noexcept
{
  const auto &it = logTypeToString.find(*this);
  if (it != logTypeToString.end())
  {
    return it->second;
  }
  return (logTypeToString.find(ARG_TYPE_INVALID)->second);
}

ArgTypes &ArgTypes::operator=(const ArgTypes &other)
{
  // Guard self assignment
  if (this == &other)
    return *this;

  m_type = other.m_type;
  return *this;
}
