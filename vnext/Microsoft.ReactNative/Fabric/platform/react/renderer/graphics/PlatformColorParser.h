// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/core/PropsParserContext.h>
#include <react/renderer/core/RawProps.h>
#include <react/renderer/graphics/Color.h>
#include <unordered_map>
#include <vector>

namespace facebook::react {

inline SharedColor parsePlatformColor(const PropsParserContext &context, const RawValue &value) {
  if (value.hasType<std::unordered_map<std::string, std::string>>()) {
    auto map = (std::unordered_map<std::string, std::vector<std::string>>)value;
    if (map.find("windowsbrush") != map.end()) {
      facebook::react::Color color = {
          /* .m_isDefined = */ true,
          /* .m_color = */ {},
          /* .m_platformColor = */ std::move(map["windowsbrush"]),
      };
      return {color};
    }
  }

  return {HostPlatformColor::UndefinedColor};
}

} // namespace facebook::react
