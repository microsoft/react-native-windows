
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "PortalComponentView.h"

#include "Composition.PortalComponentView.g.cpp"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

PortalComponentView::PortalComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext,
    winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder *builder)
    : base_type(tag, reactContext, builder) {
  m_rootComponentView = winrt::make_self<winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView>(
      compContext, *this, reactContext);
}

PortalComponentView::~PortalComponentView() {}

winrt::Microsoft::ReactNative::Composition::RootComponentView PortalComponentView::ContentRoot() const noexcept {
  return *m_rootComponentView;
}

void PortalComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  m_rootComponentView->MountChildComponentView(childComponentView, index);
  if (m_builder && m_builder->MountChildComponentViewHandler()) {
    m_builder->MountChildComponentViewHandler()(
        *this,
        winrt::make<winrt::Microsoft::ReactNative::implementation::MountChildComponentViewArgs>(
            childComponentView, index));
  }
}

void PortalComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  if (m_builder && m_builder->UnmountChildComponentViewHandler()) {
    m_builder->UnmountChildComponentViewHandler()(
        *this,
        winrt::make<winrt::Microsoft::ReactNative::implementation::UnmountChildComponentViewArgs>(
            childComponentView, index));
  }
  m_rootComponentView->UnmountChildComponentView(childComponentView, index);
}

const winrt::Microsoft::ReactNative::IComponentProps PortalComponentView::userProps(
    facebook::react::Props::Shared const &props) noexcept {
  const auto &abiViewProps = *std::static_pointer_cast<const ::Microsoft::ReactNative::AbiViewProps>(props);
  return abiViewProps.UserProps();
}

void PortalComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  m_rootComponentView->updateProps(props, oldProps);
  base_type::updateProps(props, oldProps);
}

void PortalComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  m_rootComponentView->updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
  base_type::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
}

void PortalComponentView::FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept {
  m_rootComponentView->FinalizeUpdates(updateMask);
  base_type::FinalizeUpdates(updateMask);
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
