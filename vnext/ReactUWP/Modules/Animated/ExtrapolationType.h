#pragma once
#include <string>
#include <cassert>

enum ExtrapolationType
{
  Identity,
  Clamp,
  Extend,
};

static ExtrapolationType ExtrapolationTypeFromString(const std::string& string)
{
  if (string == "identity") return ExtrapolationType::Identity;
  else if (string == "clamp") return ExtrapolationType::Clamp;
  else
  {
    assert(string == "extend");
    return ExtrapolationType::Extend;
  }
}
