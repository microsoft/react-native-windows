// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>

#include "Composition.WindowsModalHostComponentView.g.h"
#include "../CompositionViewComponentView.h"

#include <Fabric/Composition/RootComponentView.h>
#include <react/components/rnwcore/ShadowNodes.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct WindowsModalHostComponentView
    : WindowsModalHostComponentViewT<WindowsModalHostComponentView, RootComponentView> {
  using Super = WindowsModalHostComponentViewT<WindowsModalHostComponentView, RootComponentView>;

  ~WindowsModalHostComponentView();

  [[nodiscard]] static winrt::Microsoft::ReactNative::ComponentView Create(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual VisualToMountChildrenInto() noexcept override;
  void MountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void UnmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void HandleCommand(const winrt::Microsoft::ReactNative::HandleCommandArgs &args) noexcept override;
  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;

  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept;
  static facebook::react::SharedViewProps defaultProps() noexcept;
  const facebook::react::ModalHostViewProps &modalHostViewProps() const noexcept;
  bool focusable() const noexcept override;
  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt, bool ignorePointerEvents)
      const noexcept override;
  virtual std::string DefaultControlType() const noexcept;

  WindowsModalHostComponentView(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

  // Used for creating new window
  void ShowOnUIThread();
  void HideOnUIThread() noexcept;
  void EnsureModalCreated();
  static void RegisterWndClass() noexcept;

 private:
  HWND m_hwnd{nullptr};
  uint64_t m_prevWindowID;
  winrt::Microsoft::ReactNative::ReactContext m_reactContext;
  winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext m_compositionContext;
  winrt::Microsoft::ReactNative::ReactNativeIsland m_reactNativeIsland;
  facebook::react::ModalHostViewProps *m_ModalProps;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
