
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ComponentView.h"

#include <Microsoft.ReactNative.Cxx/ReactContext.h>
#include <UI.Xaml.Controls.h>
#include "ViewComponentView.h"

#pragma warning(push)
#pragma warning(disable : 4244 4305)
#include <react/renderer/components/view/ViewProps.h>
#pragma warning(pop)

namespace Microsoft::ReactNative {

struct ActivityIndicatorComponentView : BaseComponentView {
  ActivityIndicatorComponentView();

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

  const xaml::FrameworkElement Element() const noexcept override;

 private:
  facebook::react::SharedViewProps m_props;
  facebook::react::LayoutMetrics m_layoutMetrics;
  xaml::Controls::ProgressRing m_element;
};

} // namespace Microsoft::ReactNative
