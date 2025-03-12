
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>

#include "CompositionViewComponentView.h"
#include "FocusManager.h"
#include "Theme.h"

#include "Composition.RootComponentView.g.h"
#include <react/components/rnwcore/ShadowNodes.h>

struct IRawElementProviderFragmentRoot;

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct RootComponentView : RootComponentViewT<RootComponentView, ViewComponentView> {
  using Super = RootComponentViewT<RootComponentView, ViewComponentView>;

  [[nodiscard]] static winrt::Microsoft::ReactNative::ComponentView Create(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  winrt::Microsoft::ReactNative::ComponentView GetFocusedComponent() noexcept;
  void SetFocusedComponent(
      const winrt::Microsoft::ReactNative::ComponentView &value,
      winrt::Microsoft::ReactNative::FocusNavigationDirection direction) noexcept;
  bool TrySetFocusedComponent(
      const winrt::Microsoft::ReactNative::ComponentView &view,
      winrt::Microsoft::ReactNative::FocusNavigationDirection direction,
      bool forceNoSelectionIfCannotMove = false) noexcept;

  bool NavigateFocus(const winrt::Microsoft::ReactNative::FocusNavigationRequest &request) noexcept;

  bool TryMoveFocus(bool next) noexcept;

  RootComponentView *rootComponentView() const noexcept override;

  winrt::Windows::Foundation::Point ConvertScreenToLocal(winrt::Windows::Foundation::Point pt) noexcept;
  winrt::Windows::Foundation::Point ConvertLocalToScreen(winrt::Windows::Foundation::Point pt) noexcept;

  winrt::Microsoft::UI::Content::ContentIsland parentContentIsland() noexcept;

  // Index that visuals can be inserted into OuterVisual for debugging UI
  uint32_t overlayIndex() noexcept;
  void start(const winrt::Microsoft::ReactNative::ReactNativeIsland &rootView) noexcept;
  void stop() noexcept;

  void ReactNativeIsland(const winrt::Microsoft::ReactNative::ReactNativeIsland &rootView) noexcept;
  winrt::Microsoft::ReactNative::ReactNativeIsland ReactNativeIsland() noexcept;

  facebook::react::Point getClientOffset() const noexcept override;

  HRESULT GetFragmentRoot(IRawElementProviderFragmentRoot **pRetVal) noexcept;
  winrt::Microsoft::ReactNative::implementation::ClipState getClipState() noexcept override;
  float FontSizeMultiplier() const noexcept;

  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;

  winrt::IInspectable UiaProviderFromPoint(const POINT &ptPixels) noexcept;

  RootComponentView(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext,
      ReactCompositionViewComponentBuilder *builder);

  RootComponentView(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      const winrt::Microsoft::ReactNative::Composition::PortalComponentView &portal,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

  winrt::Microsoft::ReactNative::Composition::PortalComponentView Portal() const noexcept;

  virtual ~RootComponentView();

  winrt::Microsoft::ReactNative::ComponentView FindFirstFocusableElement() noexcept;
  winrt::Microsoft::ReactNative::ComponentView FindLastFocusableElement() noexcept;

  HWND GetHwndForParenting() noexcept override;

 private:
  // should this be a ReactTaggedView? - It shouldn't actually matter since if the view is going away it should always
  // be clearing its focus But being a reactTaggedView might make it easier to identify cases where that isn't
  // happening.
  winrt::Microsoft::ReactNative::ComponentView m_focusedComponent{nullptr};
  winrt::weak_ref<winrt::Microsoft::ReactNative::ReactNativeIsland> m_wkRootView{nullptr};
  winrt::weak_ref<winrt::Microsoft::ReactNative::Composition::PortalComponentView> m_wkPortal{nullptr};
  bool m_visualAddedToIsland{false};
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
