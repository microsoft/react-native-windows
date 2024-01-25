
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/AbiViewProps.h>
#include <Fabric/ComponentView.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>
#include "CompositionViewComponentView.h"

#include <Fabric/Composition/ReactCompositionViewComponentBuilder.h>

#include "Composition.AbiCompositionViewComponentView.g.h"
#include <react/components/rnwcore/ShadowNodes.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct AbiCompositionViewComponentView
    : public AbiCompositionViewComponentViewT<AbiCompositionViewComponentView, CompositionBaseComponentView> {
  using Super = AbiCompositionViewComponentViewT<AbiCompositionViewComponentView, CompositionBaseComponentView>;

  [[nodiscard]] static winrt::Microsoft::ReactNative::ComponentView Create(
      winrt::Microsoft::ReactNative::ReactContext const &reactContext,
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::IReactViewComponentBuilder builder) noexcept;

  winrt::IInspectable EnsureUiaProvider() noexcept override;

  void mountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void unmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;

  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;

  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void finalizeUpdates(
      winrt::Microsoft::ReactNative::implementation::RNComponentViewUpdateMask updateMask) noexcept override;
  void prepareForRecycle() noexcept override;
  bool focusable() const noexcept override;
  void onKeyDown(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;
  void onKeyUp(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;
  void onCharacterReceived(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs &args) noexcept
      override;
  void onPointerEntered(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void onPointerExited(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void onPointerPressed(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void onPointerReleased(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void onPointerMoved(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void onPointerWheelChanged(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  std::vector<facebook::react::ComponentDescriptorProvider> supplementalComponentDescriptorProviders() noexcept
      override;
  facebook::react::SharedViewProps viewProps() noexcept override;
  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt, bool ignorePointerEvents)
      const noexcept override;
  winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept override;

  AbiCompositionViewComponentView(
      winrt::Microsoft::ReactNative::ReactContext const &reactContext,
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::IReactViewComponentBuilder builder);

 private:
  winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder &Builder() noexcept;

  winrt::IInspectable m_handle;
  winrt::Microsoft::ReactNative::IReactViewComponentBuilder m_builder;
  winrt::Microsoft::ReactNative::IComponentState m_state;
  std::shared_ptr<::Microsoft::ReactNative::AbiViewProps const> m_props;
  winrt::Microsoft::ReactNative::Composition::IVisual m_visual{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
