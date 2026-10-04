#ifndef _ARGPARSER_
#define _ARGPARSER_

#include <string>
#include <map>
#include <sstream>

#include "ArgTypes.h"

namespace Arguments
{

class ArgParser
{
private:
  struct Option
  {
    std::string longName;
    std::string shortName;
    ArgTypes type;
    std::string description;
    bool requiresValue;
    bool required; // NEW: whether this is a required argument
    std::string value;
    bool isSet = false;
  };

  std::map<std::string, Option> longOptions;
  std::map<std::string, std::string> shortToLong;

public:
  void addOption(const std::string &longName, const std::string &shortName, ArgTypes type,
                 const std::string &description, bool requiresValue, bool required = false, std::string defaultValue = "");

  void parse(const int argc, const char *argv[]);

  template <typename T>
  T get(const std::string &longName) const;

  bool isSet(const std::string &longName) const;

  void printHelp() const;

private:
  void handleOption(Option &opt, const char *argv[], int argc, int &i);
  void errorType(const std::string &opt, const std::string &expected);
};

template <typename T>
inline T ArgParser::get(const std::string &longName) const
{
  const auto& it = longOptions.find(longName);
  if (it == longOptions.end())
  {
    throw std::invalid_argument("Unknown option: --" + longName);
  }
  const std::string& value = it->second.value;
  return static_cast<T>(value);
}

template <>
inline int ArgParser::get<int>(const std::string &longName) const
{
  auto str = get<std::string>(longName);
  int val;
  std::stringstream ss(str);
  if (!(ss >> val))
  {
    throw std::runtime_error("Invalid int value for: " + longName);
  }
  return val;
}

template <>
inline double ArgParser::get<double>(const std::string &longName) const
{
  auto str = get<std::string>(longName);
  double val;
  std::stringstream ss(str);
  if (!(ss >> val))
  {
    throw std::runtime_error("Invalid double value for: " + longName);
  }
  return val;
}

template <>
inline bool ArgParser::get<bool>(const std::string &longName) const
{
  auto str = get<std::string>(longName);
  return str == "1" || str == "true";
}

} // namespace ArgParser

#endif // _ARGPARSER_
