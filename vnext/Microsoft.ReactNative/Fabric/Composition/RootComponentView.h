
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>

#include "CompositionViewComponentView.h"
#include "Theme.h"

#include "Composition.RootComponentView.g.h"
#include <react/components/rnwcore/ShadowNodes.h>

struct IRawElementProviderFragmentRoot;

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct RootComponentView : RootComponentViewT<RootComponentView, CompositionViewComponentView> {
  using Super = RootComponentViewT<RootComponentView, CompositionViewComponentView>;

  [[nodiscard]] static winrt::Microsoft::ReactNative::ComponentView Create(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  winrt::Microsoft::ReactNative::ComponentView &GetFocusedComponent() noexcept;
  void SetFocusedComponent(const winrt::Microsoft::ReactNative::ComponentView &value) noexcept;
  bool TrySetFocusedComponent(const winrt::Microsoft::ReactNative::ComponentView &view) noexcept;

  bool NavigateFocus(const winrt::Microsoft::ReactNative::FocusNavigationRequest &request) noexcept;

  bool TryMoveFocus(bool next) noexcept;

  RootComponentView *rootComponentView() noexcept override;

  HRESULT GetFragmentRoot(IRawElementProviderFragmentRoot **pRetVal) noexcept;
  winrt::Microsoft::ReactNative::implementation::ClipState getClipState() noexcept override;

  winrt::IInspectable UiaProviderFromPoint(const POINT &ptPixels) noexcept;

  RootComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

 private:
  // should this be a ReactTaggedView? - It shouldn't actually matter since if the view is going away it should always
  // be clearing its focus But being a reactTaggedView might make it easier to identify cases where that isn't
  // happening.
  winrt::Microsoft::ReactNative::ComponentView m_focusedComponent{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
