// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/debug/react_native_expect.h>
#include <react/renderer/core/RawValue.h>
#include <react/renderer/graphics/Color.h>
#include <react/renderer/graphics/fromRawValueShared.h>
#include <react/utils/ContextContainer.h>
#include <unordered_map>
#include <vector>

namespace facebook::react {

inline SharedColor
parsePlatformColor(const ContextContainer &contextContainer, int32_t surfaceId, const RawValue &value) {
  if (value.hasType<std::unordered_map<std::string, std::vector<std::string>>>()) {
    auto map = (std::unordered_map<std::string, std::vector<std::string>>)value;
    if (map.find("windowsbrush") != map.end()) {
      facebook::react::Color color = {
          /* .m_color = */ {},
          /* .m_platformColor = */ std::move(map["windowsbrush"]),
      };
      return {color};
    }
  }

  return {HostPlatformColor::UndefinedColor};
}

inline void
fromRawValue(const ContextContainer &contextContainer, int32_t surfaceId, const RawValue &value, SharedColor &result) {
  fromRawValueShared(contextContainer, surfaceId, value, result, parsePlatformColor);
}

} // namespace facebook::react
