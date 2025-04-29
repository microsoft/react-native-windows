// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/core/PropsParserContext.h>
#include <react/renderer/core/propsConversions.h>
#include <optional>
#include <unordered_map>

namespace facebook::react {
struct AccessibilityAnnotation {
  std::optional<std::string> typeID{};
  std::optional<std::string> typeName{};
  std::optional<std::string> author{};
  std::optional<std::string> dateTime{};
  std::optional<std::string> target{};
};

constexpr bool operator==(const AccessibilityAnnotation &lhs, const AccessibilityAnnotation &rhs) {
  return lhs.typeID == rhs.typeID && lhs.typeName == rhs.typeName && lhs.author == rhs.author &&
      lhs.dateTime == rhs.dateTime && lhs.target == rhs.target;
}

constexpr bool operator!=(const AccessibilityAnnotation &lhs, const AccessibilityAnnotation &rhs) {
  return !(rhs == lhs);
}

inline void fromRawValue(const PropsParserContext &context, const RawValue &value, AccessibilityAnnotation &result) {
  auto map = (std::unordered_map<std::string, RawValue>)value;

  auto typeID = map.find("typeID");
  if (typeID != map.end()) {
    if (typeID->second.hasType<std::string>()) {
      result.typeID = (std::string)typeID->second;
    }
  }

  auto typeName = map.find("typeName");
  if (typeName != map.end()) {
    if (typeName->second.hasType<std::string>()) {
      result.typeName = (std::string)typeName->second;
    }
  }

  auto author = map.find("author");
  if (author != map.end()) {
    if (author->second.hasType<std::string>()) {
      result.author = (std::string)author->second;
    }
  }

  auto dateTime = map.find("dateTime");
  if (dateTime != map.end()) {
    if (dateTime->second.hasType<std::string>()) {
      result.dateTime = (std::string)dateTime->second;
    }
  }

  auto target = map.find("target");
  if (target != map.end()) {
    if (target->second.hasType<std::string>()) {
      result.target = (std::string)target->second;
    }
  }
}
} // namespace facebook::react
