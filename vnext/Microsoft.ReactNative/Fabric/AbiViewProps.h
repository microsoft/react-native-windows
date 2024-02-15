// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Color.g.h"
#include "ViewProps.g.h"

#include <react/renderer/components/view/ViewProps.h>
#include "winrt/Microsoft.ReactNative.h"

namespace Microsoft::ReactNative {

class AbiViewProps final : public facebook::react::ViewProps {
 public:
  AbiViewProps() = default;
  AbiViewProps(
      const facebook::react::PropsParserContext &context,
      const AbiViewProps &sourceProps,
      const facebook::react::RawProps &rawProps);
  ~AbiViewProps();

  void SetUserProps(
      winrt::Microsoft::ReactNative::IComponentProps componentProps,
      winrt::Microsoft::ReactNative::ViewProps userProps) noexcept;
  winrt::Microsoft::ReactNative::IComponentProps UserProps() const noexcept;
  winrt::Microsoft::ReactNative::ViewProps ViewProps() const noexcept;

 private:
  winrt::Microsoft::ReactNative::IComponentProps m_componentProps{nullptr};
  winrt::Microsoft::ReactNative::ViewProps m_userProps{nullptr};
};

} // namespace Microsoft::ReactNative

namespace winrt::Microsoft::ReactNative::implementation {

struct Color : ColorT<Color> {
  Color(facebook::react::SharedColor color);

  winrt::Windows::UI::Color AsWindowsColor(const winrt::Microsoft::ReactNative::Composition::Theme &theme) noexcept;
  winrt::Microsoft::ReactNative::Composition::IBrush AsBrush(
      const winrt::Microsoft::ReactNative::Composition::Theme theme) noexcept;

  static winrt::Microsoft::ReactNative::Color ReadValue(
      const winrt::Microsoft::ReactNative::IJSValueReader &reader) noexcept;
  static void WriteValue(
      const winrt::Microsoft::ReactNative::IJSValueWriter &writer,
      const winrt::Microsoft::ReactNative::Color &value) noexcept;
  static winrt::Microsoft::ReactNative::Color Transparent() noexcept;
  static winrt::Microsoft::ReactNative::Color Black() noexcept;
  static winrt::Microsoft::ReactNative::Color White() noexcept;

 private:
  facebook::react::SharedColor m_color;
};

struct UserViewProps : ViewPropsT<UserViewProps> {
  UserViewProps(std::shared_ptr<::Microsoft::ReactNative::AbiViewProps const> viewProps) noexcept;

  float Opacity() noexcept;
  winrt::Microsoft::ReactNative::Color BackgroundColor() noexcept;

  void Disconnect() noexcept;

 private:
  // Use a raw pointer here to avoid a ref cycle with AbiViewProps.  ~AbiViewProps will clear this pointer
  ::Microsoft::ReactNative::AbiViewProps const *m_viewProps{nullptr};
};

struct ViewProps : ViewPropsT<ViewProps> {
  ViewProps(facebook::react::SharedViewProps props) noexcept;

  float Opacity() noexcept;
  winrt::Microsoft::ReactNative::Color BackgroundColor() noexcept;

 private:
  facebook::react::SharedViewProps m_props;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct Color : ColorT<Color, implementation::Color> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
