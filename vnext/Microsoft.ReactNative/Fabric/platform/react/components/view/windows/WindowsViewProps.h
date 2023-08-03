// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/components/view/windows/primitives.h>
#include <react/renderer/core/Props.h>
#include <react/renderer/core/PropsParserContext.h>

namespace facebook::react {
class WindowsViewProps {
 public:
  WindowsViewProps() = default;
  WindowsViewProps(
      const PropsParserContext &context,
      const WindowsViewProps &sourceProps,
      const RawProps &rawProps,
      bool shouldSetRawProps = true);

  void
  setProp(const PropsParserContext &context, RawPropsPropNameHash hash, const char *propName, RawValue const &value);

  static bool requiresFormsStackingContext(const WindowsViewProps &props) noexcept;
  static bool requiresFormsView(const WindowsViewProps &props) noexcept;

  WindowsViewEvents windowsEvents{};
  bool enableFocusRing{true};
  bool focusable{false};
  // std::optional<std::string> overflowAnchor{};
  // std::optional<std::string> tooltip{};
  // std::vector<HandledKeyEvent> keyDownEvents{};
  // std::vector<HandledKeyEvent> keyUpEvents{};
};
} // namespace facebook::react
