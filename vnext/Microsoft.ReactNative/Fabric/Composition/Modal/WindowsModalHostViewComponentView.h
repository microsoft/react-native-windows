
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>

#include "Composition.WindowsModalHostComponentView.g.h"
#include "../CompositionViewComponentView.h"

#include <react/components/rnwcore/ShadowNodes.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct WindowsModalHostComponentView
    : public winrt::Microsoft::ReactNative::Composition::implementation::
          WindowsModalHostComponentViewT<WindowsModalHostComponentView, CompositionBaseComponentView> {
  using Super = winrt::Microsoft::ReactNative::Composition::implementation::
      WindowsModalHostComponentViewT<WindowsModalHostComponentView, CompositionBaseComponentView>;

  [[nodiscard]] static winrt::Microsoft::ReactNative::ComponentView Create(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  void mountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void unmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept override;
  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void prepareForRecycle() noexcept override;
  facebook::react::SharedViewProps viewProps() noexcept override;
  bool focusable() const noexcept override;

  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt, bool ignorePointerEvents)
      const noexcept override;
  winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept override;
  virtual std::string DefaultControlType() const noexcept;

  WindowsModalHostComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

  // Testing
  void ShowOnUIThread() noexcept;
  void HideOnUIThread() noexcept;
  static void RegisterWndClass() noexcept;

  // Testing

 private:
  std::shared_ptr<facebook::react::ModalHostViewProps const> m_props;
  winrt::Microsoft::ReactNative::Composition::ISpriteVisual m_visual{nullptr};

  // Testing
  HWND m_hwnd{nullptr};
  winrt::Microsoft::ReactNative::ReactContext m_context;
  // Testing
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
