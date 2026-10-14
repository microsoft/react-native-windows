// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "AbiTextInputShadowNode.h"

#include "AbiTextInputProps.h"

namespace Microsoft::ReactNative {

extern const char AbiTextInputComponentName[] = "AbiTextInput";

facebook::react::Size AbiTextInputShadowNode::measureContent(
    const facebook::react::LayoutContext &layoutContext,
    const facebook::react::LayoutConstraints &layoutConstraints) const {
  return Super::measureContent(layoutContext, layoutConstraints);
}

void AbiTextInputShadowNode::layout(facebook::react::LayoutContext layoutContext) {
  Super::layout(layoutContext);
}

void AbiTextInputShadowNode::OnClone(const facebook::react::ShadowNode &sourceShadowNode) noexcept {
  auto const &source = static_cast<AbiTextInputShadowNode const &>(sourceShadowNode);
  m_proxy = source.m_proxy;
  m_builder = source.m_builder;
}

void AbiTextInputShadowNode::Builder(
    winrt::Microsoft::ReactNative::IReactViewComponentBuilder builder) noexcept {
  m_builder = builder;
}

winrt::Microsoft::ReactNative::IReactViewComponentBuilder AbiTextInputShadowNode::Builder() const noexcept {
  return m_builder;
}

void AbiTextInputShadowNode::Proxy(winrt::Microsoft::ReactNative::ShadowNode proxy) noexcept {
  m_proxy = proxy;
}

winrt::Microsoft::ReactNative::ShadowNode AbiTextInputShadowNode::Proxy() const noexcept {
  return m_proxy;
}

AbiTextInputProps::AbiTextInputProps(
    const facebook::react::PropsParserContext &context,
    const AbiTextInputProps &sourceProps,
    const facebook::react::RawProps &rawProps)
    : facebook::react::WindowsTextInputProps(context, sourceProps, rawProps) {}

AbiTextInputProps::~AbiTextInputProps() {
  if (m_innerProps) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ViewProps>(m_innerProps)->Disconnect();
  }
}

void AbiTextInputProps::SetUserProps(
    winrt::Microsoft::ReactNative::IComponentProps userProps,
    winrt::Microsoft::ReactNative::ViewProps innerProps) noexcept {
  m_userProps = userProps;
  m_innerProps = innerProps;
}

winrt::Microsoft::ReactNative::IComponentProps AbiTextInputProps::UserProps() const noexcept {
  return m_userProps;
}

winrt::Microsoft::ReactNative::ViewProps AbiTextInputProps::ViewProps() const noexcept {
  return m_innerProps;
}




} // namespace Microsoft::ReactNative


namespace winrt::Microsoft::ReactNative::implementation {
  TextInputProps::TextInputProps(facebook::react::SharedViewProps props, bool holdRef) noexcept :
    Super(props, holdRef)
  {
  }

  const facebook::react::WindowsTextInputProps & TextInputProps::windowsTextInputProps() const noexcept {
    return static_cast<const facebook::react::WindowsTextInputProps &>(*m_viewProps);
  }

  winrt::hstring TextInputProps::Placeholder() noexcept {
    return m_viewProps ? winrt::to_hstring(windowsTextInputProps().placeholder) : winrt::hstring();
  }

  bool TextInputProps::SecureTextEntry() noexcept {
    return m_viewProps ? windowsTextInputProps().secureTextEntry : false;
  }

} // namespace winrt::Microsoft::ReactNative::implementation
