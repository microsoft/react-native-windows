
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>

#include <Microsoft.ReactNative.Cxx/ReactContext.h>
#include <Views/Image/ReactImage.h>
#include <react/renderer/components/image/ImageShadowNode.h>
#include <react/renderer/imagemanager/ImageResponseObserver.h>
#include <wincodec.h>
#include <winrt/Windows.UI.Composition.h>
#include "CompositionHelpers.h"
#include "CompositionViewComponentView.h"
#include "ImageResponseImage.h"

#pragma warning(push)
#pragma warning(disable : 4244 4305)
#include <react/renderer/components/view/ViewProps.h>
#pragma warning(pop)
#include "Composition.ImageComponentView.g.h"
#include <Windows.Graphics.DirectX.Direct3D11.interop.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.Storage.Streams.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct WindowsImageResponseObserver;

struct ImageComponentView : ImageComponentViewT<ImageComponentView, ComponentView> {
  using Super = ImageComponentViewT<ImageComponentView, ComponentView>;

  [[nodiscard]] static winrt::Microsoft::ReactNative::ComponentView Create(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  void MountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void UnmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void prepareForRecycle() noexcept override;
  facebook::react::SharedViewProps viewProps() noexcept override;
  void OnRenderingDeviceLost() noexcept override;
  void onThemeChanged() noexcept override;

  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt, bool ignorePointerEvents)
      const noexcept override;
  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual Visual() const noexcept override;
  bool focusable() const noexcept override;
  virtual std::string DefaultControlType() const noexcept;

  ImageComponentView(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

 private:
  struct WindowsImageResponseObserver : facebook::react::ImageResponseObserver {
   public:
    WindowsImageResponseObserver(ImageComponentView &image);
    void didReceiveProgress(float progress) const override;
    void didReceiveImage(facebook::react::ImageResponse const &imageResponse) const override;
    void didReceiveFailure() const override;

   private:
    winrt::com_ptr<ImageComponentView> m_image;
  };

  void ensureVisual() noexcept;
  void ensureDrawingSurface() noexcept;
  void DrawImage() noexcept;

  void ImageLoadStart() noexcept;
  void ImageLoaded() noexcept;
  void didReceiveImage(const std::shared_ptr<ImageResponseImage> &wicbmp) noexcept;
  void didReceiveFailureFromObserver() noexcept;
  void setStateAndResubscribeImageResponseObserver(
      facebook::react::ImageShadowNode::ConcreteState::Shared const &state) noexcept;
  bool themeEffectsImage() const noexcept;

  std::shared_ptr<const facebook::react::ImageProps> m_props;

  winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual m_visual{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::IDrawingSurfaceBrush m_drawingSurface;
  std::shared_ptr<ImageResponseImage> m_imageResponseImage;
  std::shared_ptr<WindowsImageResponseObserver> m_imageResponseObserver;
  facebook::react::ImageShadowNode::ConcreteState::Shared m_state;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
