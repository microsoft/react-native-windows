// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/components/view/BaseViewProps.h>
#include <react/renderer/core/PropsParserContext.h>
#include "KeyEvent.h"
#include "WindowsViewEvents.h"

namespace facebook::react {
class HostPlatformViewProps : public BaseViewProps {
 public:
  HostPlatformViewProps() = default;
  HostPlatformViewProps(
      const PropsParserContext &context,
      const HostPlatformViewProps &sourceProps,
      const RawProps &rawProps,
      bool shouldSetRawProps = true);

  void
  setProp(const PropsParserContext &context, RawPropsPropNameHash hash, const char *propName, RawValue const &value);

  WindowsViewEvents windowsEvents{};
  bool enableFocusRing{true};
  bool focusable{false};

  // std::optional<std::string> overflowAnchor{};
  // std::optional<std::string> tooltip{};
  std::vector<HandledKeyEvent> keyDownEvents{};
  std::vector<HandledKeyEvent> keyUpEvents{};
};
} // namespace facebook::react
