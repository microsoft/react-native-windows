
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "../ComponentView.h"
#include "../ViewComponentView.h"
#include "WindowsTextInputProps.h"
#include "WindowsTextInputShadowNode.h"

namespace Microsoft::ReactNative {

struct WindowsTextInputComponentView : BaseComponentView {
  using Super = BaseComponentView;
  WindowsTextInputComponentView();

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
  void handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept override;

  virtual const xaml::FrameworkElement Element() const noexcept override;

 private:
  void SetText(winrt::hstring text) noexcept;
  facebook::react::AttributedString getAttributedString() const;

  facebook::react::LayoutMetrics m_layoutMetrics;
  xaml::Controls::TextBox m_element;
  xaml::Controls::TextBox::TextChanged_revoker m_textChangedRevoker;
  xaml::Controls::TextBox::SelectionChanged_revoker m_SelectionChangedRevoker;
  std::shared_ptr<facebook::react::WindowsTextInputProps const> m_props;
  std::shared_ptr<facebook::react::WindowsTextInputShadowNode::ConcreteState const> m_state;
  int64_t m_mostRecentEventCount{0};
  int m_nativeEventCount{0};
  bool m_comingFromJS{false};
  bool m_comingFromState{false};
};

} // namespace Microsoft::ReactNative
