
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>

#include "CompositionViewComponentView.h"

#include <react/components/rnwcore/ShadowNodes.h>

namespace Microsoft::ReactNative {

struct RootComponentView : CompositionViewComponentView {
  using Super = CompositionViewComponentView;

  [[nodiscard]] static std::shared_ptr<RootComponentView> Create(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag) noexcept;

  std::shared_ptr<RootComponentView> getPtr();

  ::Microsoft::ReactNative::IComponentView *GetFocusedComponent() noexcept;
  void SetFocusedComponent(::Microsoft::ReactNative::IComponentView *value) noexcept;

  bool NavigateFocus(const winrt::Microsoft::ReactNative::FocusNavigationRequest &request) noexcept;

  bool TryMoveFocus(bool next) noexcept;

  RootComponentView *rootComponentView() noexcept override;

  winrt::IInspectable EnsureUiaProvider() noexcept override;

 private:
  RootComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag);
  ::Microsoft::ReactNative::IComponentView *m_focusedComponent = nullptr;
};

} // namespace Microsoft::ReactNative
