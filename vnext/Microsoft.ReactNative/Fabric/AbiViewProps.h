// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Color.g.h"
#include "ImageProps.g.h"
#include "ImageSource.g.h"
#include "ViewProps.g.h"

#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/imagemanager/primitives.h>
#include "winrt/Microsoft.ReactNative.Composition.Experimental.h"
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
      winrt::Microsoft::ReactNative::IComponentProps userProps,
      winrt::Microsoft::ReactNative::ViewProps innerProps) noexcept;
  winrt::Microsoft::ReactNative::IComponentProps UserProps() const noexcept;
  winrt::Microsoft::ReactNative::ViewProps ViewProps() const noexcept;

 private:
  winrt::Microsoft::ReactNative::IComponentProps m_userProps{nullptr};
  winrt::Microsoft::ReactNative::ViewProps m_innerProps{nullptr};
};

} // namespace Microsoft::ReactNative

namespace winrt::Microsoft::ReactNative::implementation {

struct Color : ColorT<Color, Composition::Experimental::IInternalColor> {
  Color(facebook::react::SharedColor color);

  winrt::Windows::UI::Color AsWindowsColor(const winrt::Microsoft::ReactNative::Composition::Theme &theme) noexcept;
  winrt::Microsoft::UI::Composition::CompositionBrush AsBrush(
      const winrt::Microsoft::ReactNative::Composition::Theme theme) noexcept;
  winrt::Microsoft::ReactNative::Composition::Experimental::IBrush AsInternalBrush(
      const winrt::Microsoft::ReactNative::Composition::Theme theme) noexcept;

  bool Equals(const winrt::Microsoft::ReactNative::Color &color) const noexcept;

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

struct ImageSource : ImageSourceT<ImageSource> {
  ImageSource(const facebook::react::ImageSource &imageSource);

  ImageSourceType Type() noexcept;
  winrt::hstring Uri() noexcept;
  winrt::hstring Bundle() noexcept;
  float Scale() noexcept;
  winrt::Windows::Foundation::Size Size() noexcept;

  static winrt::Microsoft::ReactNative::ImageSource ReadValue(
      const winrt::Microsoft::ReactNative::IJSValueReader &reader) noexcept;

  static void WriteValue(
      const winrt::Microsoft::ReactNative::IJSValueWriter &writer,
      const winrt::Microsoft::ReactNative::ImageSource &value) noexcept;

 private:
  const facebook::react::ImageSource m_imageSource;
};

/**
 * When providing a external ViewProps object on a normal ComponentView, ViewProps will hold a reference to the internal
 * facebook::react::ViewProps object since the ComponentView could release the object at any time. However, when
 * providing a ViewProps object on a custom ComponentView that has a custom app provided props, the AbiViewProps object
 * will already be holding a reference to this object, so we only hold onto a weak reference.
 */
struct ViewProps : ViewPropsT<ViewProps> {
  ViewProps(facebook::react::SharedViewProps props, bool holdRef = true) noexcept;

  // Notification when the owning AbiViewProps is going away.
  void Disconnect() noexcept;

  float Opacity() noexcept;
  winrt::Microsoft::ReactNative::Color BackgroundColor() noexcept;
  winrt::hstring TestId() noexcept;
  winrt::hstring AccessibilityLabel() noexcept;

 protected:
  facebook::react::SharedViewProps m_props;

  // Use a raw pointer here to avoid a ref cycle with AbiViewProps.  ~AbiViewProps will clear this pointer
  facebook::react::ViewProps const *m_viewProps{nullptr};
};

struct ImageProps : ImagePropsT<ImageProps, ViewProps> {
  using Super = ImagePropsT<ImageProps, ViewProps>;
  ImageProps(facebook::react::SharedViewProps props) noexcept;

  winrt::Windows::Foundation::Collections::IVectorView<winrt::Microsoft::ReactNative::ImageSource> Sources() noexcept;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct Color : ColorT<Color, implementation::Color> {};
struct ImageSource : ImageSourceT<ImageSource, implementation::ImageSource> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
