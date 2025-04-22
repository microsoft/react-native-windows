// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "AbiViewProps.h"

#include <Fabric/Composition/Theme.h>
#include <JSValueReader.h>
#include <react/renderer/components/image/ImageProps.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.h>

#include "Color.g.cpp"
#include "ImageSource.g.cpp"

namespace Microsoft::ReactNative {

AbiViewProps::AbiViewProps(
    const facebook::react::PropsParserContext &context,
    const AbiViewProps &sourceProps,
    const facebook::react::RawProps &rawProps)
    : facebook::react::ViewProps(context, sourceProps, rawProps) {}

AbiViewProps::~AbiViewProps() {
  if (m_innerProps) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ViewProps>(m_innerProps)->Disconnect();
  }
}

void AbiViewProps::SetUserProps(
    winrt::Microsoft::ReactNative::IComponentProps userProps,
    winrt::Microsoft::ReactNative::ViewProps innerProps) noexcept {
  m_userProps = userProps;
  m_innerProps = innerProps;
}

winrt::Microsoft::ReactNative::IComponentProps AbiViewProps::UserProps() const noexcept {
  return m_userProps;
}

winrt::Microsoft::ReactNative::ViewProps AbiViewProps::ViewProps() const noexcept {
  return m_innerProps;
}

} // namespace Microsoft::ReactNative

namespace winrt::Microsoft::ReactNative::implementation {

Color::Color(facebook::react::SharedColor color) : m_color(color) {}

winrt::Windows::UI::Color Color::AsWindowsColor(
    const winrt::Microsoft::ReactNative::Composition::Theme &theme) noexcept {
  return winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(theme)->Color(*m_color);
}
#ifdef USE_WINUI3
winrt::Microsoft::UI::Composition::CompositionBrush Color::AsBrush(
    const winrt::Microsoft::ReactNative::Composition::Theme theme) noexcept {
  return winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::InnerBrush(
      winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(theme)->Brush(*m_color));
}
#endif
winrt::Microsoft::ReactNative::Composition::Experimental::IBrush Color::AsInternalBrush(
    const winrt::Microsoft::ReactNative::Composition::Theme theme) noexcept {
  return winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(theme)->Brush(*m_color);
}

bool Color::Equals(const winrt::Microsoft::ReactNative::Color &color) const noexcept {
  if (!color) {
    return false;
  }
  return m_color == winrt::get_self<Color>(color)->m_color;
}

winrt::Microsoft::ReactNative::Color Color::ReadValue(
    const winrt::Microsoft::ReactNative::IJSValueReader &reader) noexcept {
  switch (reader.ValueType()) {
    case JSValueType::Int64: {
      auto argb = reader.GetInt64();
      return winrt::make<Color>(facebook::react::Color{/*color*/
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
      return winrt::make<Color>(facebook::react::Color{/*color*/ {}, std::move(platformColors)});
    }
    default:
      return winrt::make<Color>(facebook::react::Color{/*color*/ {0, 0, 0, 0}, {}});
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

ImageSource::ImageSource(const facebook::react::ImageSource &imageSource) : m_imageSource(imageSource) {}

ImageSourceType ImageSource::Type() noexcept {
  static_assert(
      winrt::Microsoft::ReactNative::ImageSourceType::Invalid ==
      static_cast<winrt::Microsoft::ReactNative::ImageSourceType>(facebook::react::ImageSource::Type::Invalid));
  static_assert(
      winrt::Microsoft::ReactNative::ImageSourceType::Remote ==
      static_cast<winrt::Microsoft::ReactNative::ImageSourceType>(facebook::react::ImageSource::Type::Remote));
  static_assert(
      winrt::Microsoft::ReactNative::ImageSourceType::Local ==
      static_cast<winrt::Microsoft::ReactNative::ImageSourceType>(facebook::react::ImageSource::Type::Local));
  return static_cast<winrt::Microsoft::ReactNative::ImageSourceType>(m_imageSource.type);
}

winrt::hstring ImageSource::Uri() noexcept {
  return winrt::to_hstring(m_imageSource.uri);
}

winrt::hstring ImageSource::Bundle() noexcept {
  return winrt::to_hstring(m_imageSource.bundle);
}

float ImageSource::Scale() noexcept {
  return m_imageSource.scale;
}

winrt::Windows::Foundation::Size ImageSource::Size() noexcept {
  return {m_imageSource.size.width, m_imageSource.size.height};
}

winrt::Microsoft::ReactNative::ImageSource ImageSource::ReadValue(
    const winrt::Microsoft::ReactNative::IJSValueReader &reader) noexcept {
  facebook::react::ImageSource imageSource;
  switch (reader.ValueType()) {
    case JSValueType::String: {
      imageSource.type = facebook::react::ImageSource::Type::Remote;
      imageSource.uri = winrt::to_string(reader.GetString());
      break;
    }
    case JSValueType::Object: {
      imageSource.type = facebook::react::ImageSource::Type::Remote;

      winrt::hstring propertyName;
      while (reader.GetNextObjectProperty(/*out*/ propertyName)) {
        if (propertyName == L"__packager_asset") {
          imageSource.type = facebook::react::ImageSource::Type::Local;
        } else if (propertyName == L"width") {
          imageSource.size.width = static_cast<float>(reader.GetDouble());
        } else if (propertyName == L"height") {
          imageSource.size.height = static_cast<float>(reader.GetDouble());
        } else if (propertyName == L"scale") {
          imageSource.scale = static_cast<float>(reader.GetDouble());
        } else if (propertyName == L"uri") {
          imageSource.uri = winrt::to_string(reader.GetString());
        } else if (propertyName == L"url") {
          imageSource.uri = winrt::to_string(reader.GetString());
        } else if (propertyName == L"bundle") {
          imageSource.bundle = winrt::to_string(reader.GetString());
          imageSource.type = facebook::react::ImageSource::Type::Local;
        }
      }
      break;
    }
    default: {
      imageSource.type = facebook::react::ImageSource::Type::Invalid;
      break;
    }
  }
  return winrt::make<ImageSource>(imageSource);
}

ViewProps::ViewProps(facebook::react::SharedViewProps props, bool holdRef) noexcept
    : m_props(holdRef ? props : nullptr), m_viewProps(static_cast<const facebook::react::ViewProps *>(props.get())) {}

void ViewProps::Disconnect() noexcept {
  m_viewProps = nullptr;
}

float ViewProps::Opacity() noexcept {
  return m_viewProps ? m_viewProps->opacity : 1.0f;
}

winrt::Microsoft::ReactNative::Color ViewProps::BackgroundColor() noexcept {
  return winrt::make<Color>(m_viewProps ? m_viewProps->backgroundColor : facebook::react::clearColor());
}

winrt::hstring ViewProps::TestId() noexcept {
  return m_viewProps ? winrt::to_hstring(m_viewProps->testId) : winrt::hstring{};
}

winrt::hstring ViewProps::AccessibilityLabel() noexcept {
  return m_viewProps ? winrt::to_hstring(m_viewProps->accessibilityLabel) : winrt::hstring{};
}

ImageProps::ImageProps(facebook::react::SharedViewProps props) noexcept : Super(props) {}

winrt::Windows::Foundation::Collections::IVectorView<winrt::Microsoft::ReactNative::ImageSource>
ImageProps::Sources() noexcept {
  winrt::Windows::Foundation::Collections::IVector<winrt::Microsoft::ReactNative::ImageSource> sources{
      winrt::single_threaded_vector<winrt::Microsoft::ReactNative::ImageSource>()};
  const auto imageProps = std::static_pointer_cast<const facebook::react::ImageProps>(m_props);

  for (const auto &source : imageProps->sources) {
    sources.Append(winrt::make<ImageSource>(source));
  }

  return sources.GetView();
}

} // namespace winrt::Microsoft::ReactNative::implementation
