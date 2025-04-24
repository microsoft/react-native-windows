
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

struct ImageComponentView : ImageComponentViewT<ImageComponentView, ViewComponentView> {
  using Super = ImageComponentViewT<ImageComponentView, ViewComponentView>;

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
  void prepareForRecycle() noexcept override;
  void OnRenderingDeviceLost() noexcept override;
  void onThemeChanged() noexcept override;

  virtual std::string DefaultControlType() const noexcept;
  static facebook::react::SharedViewProps defaultProps() noexcept;

  ImageComponentView(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

  const facebook::react::ImageProps &imageProps() const noexcept;
  // TODO try to change to ViewProps
  winrt::Microsoft::ReactNative::ImageProps ImageProps() noexcept;
  winrt::Microsoft::ReactNative::ImageProps ViewProps() noexcept;

 protected:
  winrt::Microsoft::ReactNative::ViewProps ViewPropsInner() noexcept override;

 private:
  struct WindowsImageResponseObserver : facebook::react::ImageResponseObserver {
   public:
    WindowsImageResponseObserver(
        winrt::Microsoft::ReactNative::ReactContext const &reactContext,
        winrt::weak_ref<winrt::Microsoft::ReactNative::Composition::implementation::ImageComponentView> wkImage);
    void didReceiveProgress(float progress, int64_t loaded, int64_t total) const override;
    void didReceiveImage(const facebook::react::ImageResponse &imageResponse) const override;
    void didReceiveFailure(const facebook::react::ImageLoadError &error) const override;

   private:
    winrt::weak_ref<winrt::Microsoft::ReactNative::Composition::implementation::ImageComponentView> m_wkImage;
    winrt::Microsoft::ReactNative::ReactContext m_reactContext;
  };

  void ensureDrawingSurface() noexcept;
  void DrawImage() noexcept;

  void ImageLoadStart() noexcept;
  void ImageLoaded() noexcept;
  void didReceiveProgress(float progress, int loaded, int total) noexcept;
  void didReceiveImage(const std::shared_ptr<ImageResponseImage> &wicbmp) noexcept;
  void didReceiveFailureFromObserver(const facebook::react::ImageLoadError &error) noexcept;
  void setStateAndResubscribeImageResponseObserver(
      facebook::react::ImageShadowNode::ConcreteState::Shared const &state) noexcept;
  bool themeEffectsImage() const noexcept;

  winrt::Microsoft::ReactNative::Composition::Experimental::IDrawingSurfaceBrush m_drawingSurface;
  std::shared_ptr<ImageResponseImage> m_imageResponseImage;
  std::shared_ptr<WindowsImageResponseObserver> m_imageResponseObserver;
  facebook::react::ImageShadowNode::ConcreteState::Shared m_state;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
