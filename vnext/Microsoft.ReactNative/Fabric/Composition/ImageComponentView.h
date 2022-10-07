
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

#pragma warning(push)
#pragma warning(disable : 4244 4305)
#include <react/renderer/components/view/ViewProps.h>
#pragma warning(pop)
#include <Windows.Graphics.DirectX.Direct3D11.interop.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.Storage.Streams.h>

namespace Microsoft::ReactNative {

struct WindowsImageResponseObserver;

struct ImageComponentView : CompositionBaseComponentView {
  using Super = CompositionBaseComponentView;
  ImageComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

  std::vector<facebook::react::ComponentDescriptorProvider> supplementalComponentDescriptorProviders() noexcept
      override;
  void mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept override;
  void unmountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept override;
  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept override;
  void prepareForRecycle() noexcept override;
  facebook::react::Props::Shared props() noexcept override;
  void OnRenderingDeviceLost() noexcept override;
  facebook::react::SharedTouchEventEmitter touchEventEmitter() noexcept override;

  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt) const noexcept override;
  winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept override;

 private:
  struct WindowsImageResponseObserver : facebook::react::ImageResponseObserver {
   public:
    WindowsImageResponseObserver(std::shared_ptr<ImageComponentView> image);
    void didReceiveProgress(float progress) const override;
    void didReceiveImage(facebook::react::ImageResponse const &imageResponse) const override;
    void didReceiveFailure() const override;

   private:
    std::shared_ptr<ImageComponentView> m_image;
  };

  void ensureVisual() noexcept;
  void ensureDrawingSurface() noexcept;
  void DrawImage() noexcept;

  void ImageLoadStart() noexcept;
  void ImageLoaded() noexcept;
  void didReceiveImage(const winrt::com_ptr<IWICBitmap> &wicbmp) noexcept;
  void didReceiveFailureFromObserver() noexcept;
  void setStateAndResubscribeImageResponseObserver(
      facebook::react::ImageShadowNode::ConcreteState::Shared const &state) noexcept;

  facebook::react::SharedViewProps m_props;

  winrt::Microsoft::ReactNative::Composition::SpriteVisual m_visual{nullptr};
  winrt::Microsoft::ReactNative::ReactContext m_context;
  winrt::Microsoft::ReactNative::Composition::ICompositionDrawingSurface m_drawingSurface;
  winrt::com_ptr<IWICBitmap> m_wicbmp;
  std::shared_ptr<WindowsImageResponseObserver> m_imageResponseObserver;
  facebook::react::ImageShadowNode::ConcreteState::Shared m_state;
};

} // namespace Microsoft::ReactNative
