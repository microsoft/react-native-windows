
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>
#include "RootComponentView.h"

#include "Composition.PortalComponentView.g.h"

namespace Microsoft::ReactNative {
struct CompContext;
} // namespace Microsoft::ReactNative

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct PortalComponentView
    : public PortalComponentViewT<PortalComponentView, winrt::Microsoft::ReactNative::implementation::ComponentView> {
  PortalComponentView(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext,
      winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder *builder);
  virtual ~PortalComponentView();

  winrt::Microsoft::ReactNative::Composition::RootComponentView ContentRoot() const noexcept;

  void MountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void UnmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;

  const winrt::Microsoft::ReactNative::IComponentProps userProps(
      facebook::react::Props::Shared const &props) noexcept override;
  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;

  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;

  void FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept override;

 private:
  winrt::com_ptr<winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView> m_rootComponentView;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
