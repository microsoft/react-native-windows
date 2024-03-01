// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "AbiViewProps.h"

#include <Fabric/Composition/Theme.h>
#include <JSValueReader.h>

#if __has_include("Color.g.cpp")
#include "Color.g.cpp"
#endif

namespace Microsoft::ReactNative {

AbiViewProps::AbiViewProps(
    const facebook::react::PropsParserContext &context,
    const AbiViewProps &sourceProps,
    const facebook::react::RawProps &rawProps)
    : facebook::react::ViewProps(context, sourceProps, rawProps) {}

AbiViewProps::~AbiViewProps() {
  if (m_userProps) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::UserViewProps>(m_userProps)->Disconnect();
  }
}

void AbiViewProps::SetUserProps(
    winrt::Microsoft::ReactNative::IComponentProps componentProps,
    winrt::Microsoft::ReactNative::ViewProps userProps) noexcept {
  m_componentProps = componentProps;
  m_userProps = userProps;
}

winrt::Microsoft::ReactNative::IComponentProps AbiViewProps::UserProps() const noexcept {
  return m_componentProps;
}

winrt::Microsoft::ReactNative::ViewProps AbiViewProps::ViewProps() const noexcept {
  return m_userProps;
}

} // namespace Microsoft::ReactNative

namespace winrt::Microsoft::ReactNative::implementation {

Color::Color(facebook::react::SharedColor color) : m_color(color) {}

winrt::Windows::UI::Color Color::AsWindowsColor(
    const winrt::Microsoft::ReactNative::Composition::Theme &theme) noexcept {
  return winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(theme)->Color(*m_color);
}
winrt::Microsoft::ReactNative::Composition::IBrush Color::AsBrush(
    const winrt::Microsoft::ReactNative::Composition::Theme theme) noexcept {
  return winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(theme)->Brush(*m_color);
}

winrt::Microsoft::ReactNative::Color Color::ReadValue(
    const winrt::Microsoft::ReactNative::IJSValueReader &reader) noexcept {
  switch (reader.ValueType()) {
    case JSValueType::Int64: {
      auto argb = reader.GetInt64();
      return winrt::make<Color>(facebook::react::Color{
          /*m_isDefined*/ true,
          /*color*/
          {static_cast<uint8_t>((argb >> 24) & 0xFF),
           static_cast<uint8_t>((argb >> 16) & 0xFF),
           static_cast<uint8_t>((argb >> 8) & 0xFF),
           static_cast<uint8_t>(argb & 0xFF)},
          {}});
    }
    case JSValueType::Object: {
      std::vector<std::string> platformColors;
      winrt::hstring propertyName;
      while (reader.GetNextObjectProperty(/*out*/ propertyName)) {
        if (propertyName == L"windowsbrush") {
          winrt::Microsoft::ReactNative::ReadValue(reader, platformColors);
        } else {
          SkipValue<JSValue>(reader); // Skip this property
        }
      }
      return winrt::make<Color>(facebook::react::Color{/*m_isDefined*/ true, /*color*/ {}, std::move(platformColors)});
    }
    default:
      return winrt::make<Color>(facebook::react::Color{/*m_isDefined*/ false, /*color*/ {0, 0, 0, 0}, {}});
  }
}

void Color::WriteValue(
    const winrt::Microsoft::ReactNative::IJSValueWriter &writer,
    const winrt::Microsoft::ReactNative::Color &color) noexcept {
  auto self = winrt::get_self<Color>(color);
  if ((*self->m_color).m_platformColor.empty()) {
    writer.WriteInt64(
        ((*self->m_color).m_color.A << 24) & ((*self->m_color).m_color.R << 16) & ((*self->m_color).m_color.G << 8) &
        (*self->m_color).m_color.B);
  } else {
    writer.WriteObjectBegin();
    writer.WritePropertyName(L"windowsbrush");
    writer.WriteArrayBegin();
    for (auto &platformColor : (*self->m_color).m_platformColor) {
      writer.WriteString(winrt::to_hstring(platformColor));
    }
    writer.WriteArrayEnd();
    writer.WriteObjectEnd();
  }
}

winrt::Microsoft::ReactNative::Color Color::Transparent() noexcept {
  return winrt::make<Color>(facebook::react::clearColor());
}
winrt::Microsoft::ReactNative::Color Color::Black() noexcept {
  return winrt::make<Color>(facebook::react::blackColor());
}
winrt::Microsoft::ReactNative::Color Color::White() noexcept {
  return winrt::make<Color>(facebook::react::whiteColor());
}

UserViewProps::UserViewProps(std::shared_ptr<::Microsoft::ReactNative::AbiViewProps const> viewProps) noexcept
    : m_viewProps(viewProps.get()) {}

void UserViewProps::Disconnect() noexcept {
  m_viewProps = nullptr;
}

float UserViewProps::Opacity() noexcept {
  return m_viewProps ? m_viewProps->opacity : 1.0f;
}

winrt::Microsoft::ReactNative::Color UserViewProps::BackgroundColor() noexcept {
  return winrt::make<Color>(m_viewProps ? m_viewProps->backgroundColor : facebook::react::clearColor());
}

ViewProps::ViewProps(facebook::react::SharedViewProps props) noexcept : m_props(props) {}

float ViewProps::Opacity() noexcept {
  return m_props->opacity;
}

winrt::Microsoft::ReactNative::Color ViewProps::BackgroundColor() noexcept {
  return winrt::make<Color>(m_props->backgroundColor);
}

} // namespace winrt::Microsoft::ReactNative::implementation
