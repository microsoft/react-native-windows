// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/debug/react_native_expect.h>
#include <react/renderer/core/PropsParserContext.h>
#include <react/renderer/core/propsConversions.h>
#include "primitives.h"

namespace facebook::react {

inline void
fromRawValue(const PropsParserContext &context, const RawValue &value, HandledKeyEvent::EventPhase &result) {
  if (value.hasType<int>()) {
    result = (HandledKeyEvent::EventPhase)(int)value;
    return;
  }

  LOG(ERROR) << "Unsupported HandledKeyEvent::EventPhase type";
}

inline void fromRawValue(const PropsParserContext &context, const RawValue &value, HandledKeyEvent &result) {
  if (value.hasType<butter::map<std::string, RawValue>>()) {
    auto map = (butter::map<std::string, RawValue>)value;

    auto attrIterator = map.find("eventPhase");
    if (attrIterator != map.end())
      fromRawValue(context, attrIterator->second, result.eventPhase);
    attrIterator = map.find("altKey");
    if (attrIterator != map.end())
      fromRawValue(context, attrIterator->second, result.altKey);
    attrIterator = map.find("ctrlKey");
    if (attrIterator != map.end())
      fromRawValue(context, attrIterator->second, result.ctrlKey);
    attrIterator = map.find("metaKey");
    if (attrIterator != map.end())
      fromRawValue(context, attrIterator->second, result.metaKey);
    attrIterator = map.find("shiftKey");
    if (attrIterator != map.end())
      fromRawValue(context, attrIterator->second, result.shiftKey);
    attrIterator = map.find("code");
    if (attrIterator != map.end())
      fromRawValue(context, attrIterator->second, result.code);
    return;
  }

  LOG(ERROR) << "Unsupported HandledKeyEvent type";
}

} // namespace facebook::react