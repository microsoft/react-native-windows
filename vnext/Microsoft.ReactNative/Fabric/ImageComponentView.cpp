
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ImageComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

#include <IReactContext.h>

#pragma warning(push)
#pragma warning(disable : 4244 4305)
#include <react/renderer/components/image/ImageProps.h>
#pragma warning(pop)

namespace Microsoft::ReactNative {

ImageComponentView::ImageComponentView(winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : m_context(reactContext), m_element(ReactImage::Create()) {
  static auto const defaultProps = std::make_shared<facebook::react::ImageProps const>();
  m_props = defaultProps;
}

std::vector<facebook::react::ComponentDescriptorProvider>
ImageComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void ImageComponentView::mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  assert(false);
  // m_element->Children().InsertAt(index, static_cast<const BaseComponentView &>(childComponentView).Element());
}

void ImageComponentView::unmountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  assert(false);
  // m_element->Children().RemoveAt(index);
}

void ImageComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldImageProps = *std::static_pointer_cast<const facebook::react::ImageProps>(m_props);
  const auto &newImageProps = *std::static_pointer_cast<const facebook::react::ImageProps>(props);

  if (oldImageProps.sources != newImageProps.sources) {
    if (newImageProps.sources.empty()) {
      // TODO clear image
    } else {
      ReactImageSource ris;
      ris.uri = newImageProps.sources[0].uri;
      ris.width = newImageProps.sources[0].size.width;
      ris.height = newImageProps.sources[0].size.height;
      ris.scale = newImageProps.sources[0].scale;

      auto contextSelf =
          winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactContext>(m_context.Handle());
      ris.bundleRootPath = contextSelf->GetInner().SettingsSnapshot().BundleRootPath();

      // TODO headers, method, __packager_asset?
      // Assume packager_asset for now...
      if (/*packager_assert && */ ris.uri.find("file://") == 0) {
        ris.uri.replace(0, 7, ris.bundleRootPath);
      }
      // EmitImageEvent(grid, "topLoadStart", sources[0]);
      m_element->Source(ris);
    }
  }

  if (oldImageProps.resizeMode != newImageProps.resizeMode) {
    m_element->ResizeMode(newImageProps.resizeMode);
  }

  m_props = std::static_pointer_cast<facebook::react::ImageProps const>(props);
}

void ImageComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}
void ImageComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties

  m_layoutMetrics = layoutMetrics;

  winrt::Microsoft::ReactNative::ViewPanel::SetLeft(*m_element, layoutMetrics.frame.origin.x);
  winrt::Microsoft::ReactNative::ViewPanel::SetTop(*m_element, layoutMetrics.frame.origin.y);

  m_element->Width(layoutMetrics.frame.size.width);
  m_element->Height(layoutMetrics.frame.size.height);
}
void ImageComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {}
void ImageComponentView::prepareForRecycle() noexcept {}
facebook::react::SharedProps ImageComponentView::props() noexcept {
  assert(false);
  return {};
}

const xaml::FrameworkElement ImageComponentView::Element() const noexcept {
  return *m_element;
}

} // namespace Microsoft::ReactNative
