// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/components/view/windows/primitives.h>
#include <react/renderer/components/view/BaseViewProps.h>
#include <react/renderer/core/PropsParserContext.h>

namespace facebook::react {
class WindowsViewProps : public BaseViewProps {
 public:
  WindowsViewProps() = default;
  WindowsViewProps(
      const PropsParserContext &context,
      const WindowsViewProps &sourceProps,
      const RawProps &rawProps,
      bool shouldSetRawProps = true);

  void
  setProp(const PropsParserContext &context, RawPropsPropNameHash hash, const char *propName, RawValue const &value);

#ifdef ANDROID
  void propsDiffMapBuffer(Props const *oldProps, MapBufferBuilder &builder) const override;
#endif

  static bool requiresFormsStackingContext(const WindowsViewProps &props) noexcept;
  static bool requiresFormsView(const WindowsViewProps &props) noexcept;

  Float elevation{};
  WindowsViewEvents windowsEvents{};
  bool enableFocusRing{true};
  bool focusable{false};

  // std::optional<std::string> overflowAnchor{};
  // std::optional<std::string> tooltip{};
  // std::vector<HandledKeyEvent> keyDownEvents{};
  // std::vector<HandledKeyEvent> keyUpEvents{};

#ifdef ANDROID

  std::optional<NativeDrawable> nativeBackground{};
  std::optional<NativeDrawable> nativeForeground{};

  bool hasTVPreferredFocus{false};
  bool needsOffscreenAlphaCompositing{false};
  bool renderToHardwareTextureAndroid{false};

#endif

#ifdef ANDROID
  bool getProbablyMoreHorizontalThanVertical_DEPRECATED() const override;
#endif

#if RN_DEBUG_STRING_CONVERTIBLE
  SharedDebugStringConvertibleList getDebugProps() const override;
#endif
};
} // namespace facebook::react
