// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/core/PropsParserContext.h>
#include <react/renderer/core/propsConversions.h>
#include <react/renderer/graphics/Float.h>
#include <string>

namespace facebook::react {

/*
 * Describes an individual key event.
 */
struct BaseKeyEvent {
  /**
   * The code for the event aligned to https://www.w3.org/TR/uievents-code/.
   */
  std::string code{};

  /*
   * A flag indicating if the alt key is pressed.
   */
  bool altKey{false};

  /*
   * A flag indicating if the control key is pressed.
   */
  bool ctrlKey{false};

  /*
   * A flag indicating if the shift key is pressed.
   */
  bool shiftKey{false};

  /*
   * A flag indicating if the meta key is pressed.
   */
  bool metaKey{false};
};

inline static bool operator==(const BaseKeyEvent &lhs, const BaseKeyEvent &rhs) {
  return lhs.code == rhs.code && lhs.altKey == rhs.altKey && lhs.ctrlKey == rhs.ctrlKey &&
      lhs.shiftKey == rhs.shiftKey && lhs.metaKey == rhs.metaKey;
}

/**
 * Event phase indicator matching the EventPhase in React.
 * EventPhase includes None, Capturing, AtTarget, Bubbling.
 */
enum class HandledEventPhase { Capturing = 1, Bubbling = 3 };

inline void fromRawValue(const PropsParserContext &context, const RawValue &value, HandledEventPhase &result) {
  if (value.hasType<int>()) {
    result = static_cast<HandledEventPhase>((int)value);
    return;
  }

  LOG(ERROR) << "Unsupported HandledKeyEvent::EventPhase type";
}

/**
 * Describes a handled key event declaration.
 */
struct HandledKeyEvent : BaseKeyEvent {
  /**
   * The phase at which the event should be marked as handled.
   */
  HandledEventPhase handledEventPhase{HandledEventPhase::Bubbling};
};

inline void fromRawValue(const PropsParserContext &context, const RawValue &value, HandledKeyEvent &result) {
  if (value.hasType<std::unordered_map<std::string, RawValue>>()) {
    auto map = (std::unordered_map<std::string, RawValue>)value;

    auto attrIterator = map.find("handledEventPhase");
    if (attrIterator != map.end())
      fromRawValue(context, attrIterator->second, result.handledEventPhase);
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

inline static bool operator==(const HandledKeyEvent &lhs, const HandledKeyEvent &rhs) {
  return lhs.handledEventPhase == rhs.handledEventPhase &&
      static_cast<BaseKeyEvent>(lhs) == static_cast<BaseKeyEvent>(rhs);
}

struct KeyEvent : BaseKeyEvent {
  /**
   * The key for the event aligned to https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/key/Key_Values.
   */
  std::string key{};
};

} // namespace facebook::react
