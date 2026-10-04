#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <iomanip>
#include <stdexcept>
#include <algorithm>

#include "ArgParser.h"

using namespace Arguments;

void ArgParser::addOption(const std::string &longName, const std::string &shortName, ArgTypes type,
                          const std::string &description, bool requiresValue, bool required, std::string defaultValue)
{
  if (!required && defaultValue.empty())
  {
    throw std::invalid_argument("Required options must have a default value.");
  }
  if (longOptions.find(longName) != longOptions.end())
  {
    throw std::invalid_argument("Option already exists: --" + longName);
  }
  if (!shortName.empty() && shortToLong.find(shortName) != shortToLong.end())
  {
    throw std::invalid_argument("Short option already exists: -" + shortName);
  }
  if (longName.empty())
  {
    throw std::invalid_argument("Long option name cannot be empty.");
  }
  if (type == ArgTypes::BOOL && requiresValue)
  {
    throw std::invalid_argument("Boolean options cannot require a value.");
  }
  Option opt{longName, shortName, type, description, requiresValue, required, defaultValue};
  longOptions[longName] = opt;
  if (!shortName.empty())
    shortToLong[shortName] = longName;
}

void ArgParser::parse(const int argc, const char *argv[])
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];

    if ((arg == "--help") || (arg == "-h"))
    {
      printHelp();
      exit(0);
    }

    if (arg.rfind("--", 0) == 0)
    {
      std::string optName = arg.substr(2);
      auto it = longOptions.find(optName);
      if (it == longOptions.end())
      {
        throw std::runtime_error("Unknown option: --" + optName);
      }
      handleOption(it->second, argv, argc, i);
    }
    else if (arg.rfind("-", 0) == 0 && arg.length() > 1)
    {
      std::string shortName = arg.substr(1);
      if (shortToLong.find(shortName) == shortToLong.end())
      {
        throw std::runtime_error("Unknown option: -" + shortName);
      }
      Option &opt = longOptions[shortToLong[shortName]];
      handleOption(opt, argv, argc, i);
    }
    else
    {
      throw std::invalid_argument("Invalid argument: " + arg);
    }
  }

  // Check if all required options are set
  for (const auto& pair : longOptions)
  {
    const auto& name = pair.first;
    const auto& opt = pair.second;
    if (opt.required && !opt.isSet)
    {
      throw std::runtime_error("Missing required option: --" + name);
    }
  }
}

bool ArgParser::isSet(const std::string &longName) const
{
  auto it = longOptions.find(longName);
  if (it == longOptions.end())
    throw std::invalid_argument("Unknown option: --" + longName);
  return it->second.isSet;
}

void ArgParser::printHelp() const
{
  std::cout << "Options:\n";
  constexpr int menuCategorySize = 5;
  std::vector<std::array<std::string, menuCategorySize>> menu;
  menu.resize(longOptions.size());
  std::array<size_t, menuCategorySize> menuCategoryMaxSize = { 0 };
  int i = 0;
  for (const auto& pair : longOptions)
  {
    const auto& name = pair.first;
    const auto& opt = pair.second;
    std::stringstream ss;
    ss << "  --" << name;
    if (!opt.shortName.empty())
      ss << ", -" << opt.shortName;

    std::string type = "[" + (std::string)opt.type + "] ";
    std::string fullDesc = opt.description;
    std::string reqiured;
    std::string defaultValue;
    if (opt.required)
    {
      reqiured = "[required]";
    }
    else
    {
      reqiured = "[optional]";
      defaultValue = "[default: " + opt.value +"]";
    }

    menu[i][0] = ss.str();
    menuCategoryMaxSize[0] = std::max(menuCategoryMaxSize[0], menu[i][0].length() + 1);
    menu[i][1] = type;
    menuCategoryMaxSize[1] = std::max(menuCategoryMaxSize[1], menu[i][1].length() + 1);
    menu[i][2] = fullDesc;
    menuCategoryMaxSize[2] = std::max(menuCategoryMaxSize[2], menu[i][2].length() + 1);
    menu[i][3] = reqiured;
    menuCategoryMaxSize[3] = std::max(menuCategoryMaxSize[3], menu[i][3].length() + 1);
    menu[i][4] = defaultValue;
    menuCategoryMaxSize[4] = std::max(menuCategoryMaxSize[4], menu[i][4].length() + 1);
    i++;
  }

  for (i = 0; i < menu.size(); i++)
  {
    std::cout << std::left << std::setw(menuCategoryMaxSize[0]) << menu[i][0];
    std::cout << std::left << std::setw(menuCategoryMaxSize[1]) << menu[i][1];
    std::cout << std::left << std::setw(menuCategoryMaxSize[2]) << menu[i][2];
    std::cout << std::left << std::setw(menuCategoryMaxSize[3]) << menu[i][3];
    std::cout << std::left << std::setw(menuCategoryMaxSize[4]) << menu[i][4];
    std::cout << std::endl;
  }
  std::cout << std::left << std::setw(menuCategoryMaxSize[0] + menuCategoryMaxSize[1]) << "  --help, -h";
  std::cout << std::left << std::setw(menuCategoryMaxSize[2]) << "Show this help message";
  std::cout << std::endl;
}

void ArgParser::handleOption(Option &opt, const char *argv[], int argc, int &i)
{
  opt.isSet = true;
  if (!opt.requiresValue)
  {
    opt.value = "true";
    return;
  }

  if (i + 1 >= argc)
  {
    throw std::runtime_error("Missing value for option: --" + opt.longName);
  }

  std::string val = argv[++i];
  std::stringstream ss(val);

  switch (opt.type)
  {
  case ArgTypes::STRING:
  case ArgTypes::FILE:
    opt.value = val;
    break;
  case ArgTypes::INT:
  {
    int intVal;
    if (!(ss >> intVal))
      errorType(opt.longName, "int");
    opt.value = val;
    break;
  }
  case ArgTypes::DOUBLE:
  {
    double dblVal;
    if (!(ss >> dblVal))
      errorType(opt.longName, "double");
    opt.value = val;
    break;
  }
  case ArgTypes::BOOL:
    opt.value = (val == "true" || val == "1") ? "true" : "false";
    break;
  }
}

void ArgParser::errorType(const std::string &opt, const std::string &expected)
{
  throw std::runtime_error("Invalid value for option: --" + opt + ", expected " + expected);
}
