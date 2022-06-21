
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>

#include <Microsoft.ReactNative.Cxx/ReactContext.h>
#include <Views/Image/ReactImage.h>
#include <wincodec.h>
#include <winrt/Windows.UI.Composition.h>
#include "CompHelpers.h"
#include "CompViewComponentView.h"

#pragma warning(push)
#pragma warning(disable : 4244 4305)
#include <react/renderer/components/view/ViewProps.h>
#pragma warning(pop)
#include <Windows.Graphics.DirectX.Direct3D11.interop.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.Storage.Streams.h>

namespace Microsoft::ReactNative {

struct CompImageComponentView : CompBaseComponentView {
  using Super = CompBaseComponentView;
  CompImageComponentView(
      const winrt::com_ptr<Composition::ICompositionContext> &compContext,
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
  facebook::react::SharedProps props() noexcept override;
  void OnRenderingDeviceLost() noexcept override;

  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt) const noexcept override;
  const winrt::com_ptr<Composition::ISpriteVisual> Visual() const noexcept override;

 private:
  void ensureVisual() noexcept;
  void beginDownloadImage() noexcept;
  void generateBitmap(const winrt::Windows::Storage::Streams::InMemoryRandomAccessStream &stream) noexcept;
  void ensureDrawingSurface() noexcept;
  void DrawImage() noexcept;

  facebook::react::SharedViewProps m_props;

  winrt::com_ptr<Microsoft::ReactNative::Composition::ISpriteVisual> m_visual;
  winrt::Microsoft::ReactNative::ReactContext m_context;
  winrt::com_ptr<Composition::ICompositionDrawingSurface> m_drawingSurfaceInterop;
  winrt::com_ptr<IWICBitmap> m_wicbmp;
  unsigned int m_imgWidth{0}, m_imgHeight{0};
  bool m_reloadImage{false};

  enum class ImageState {
    None,
    Loading,
    Loaded,
    Error,
  };

  ImageState m_state;
  std::string m_url;
};

} // namespace Microsoft::ReactNative
