// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "AbiShadowNode.h"

#include <Fabric/Composition/ReactCompositionViewComponentBuilder.h>
#include <react/debug/react_native_assert.h>
#include <react/renderer/core/TraitCast.h>
#include <react/renderer/core/conversions.h>

#include <utility>

namespace winrt::Microsoft::ReactNative::implementation {

AbiProps::AbiProps(
    const facebook::react::PropsParserContext &context,
    const AbiProps &sourceProps,
    const facebook::react::RawProps &rawProps)
    : facebook::react::Props(context, sourceProps, rawProps) {}

AbiProps::~AbiProps() {}

void AbiProps::SetUserProps(winrt::Microsoft::ReactNative::IComponentProps componentProps) noexcept {
  m_componentProps = componentProps;
}

winrt::Microsoft::ReactNative::IComponentProps AbiProps::UserProps() const noexcept {
  return m_componentProps;
}

ShadowNode::ShadowNode(facebook::react::ShadowNode::Shared shadowNode) noexcept : m_shadowNode(shadowNode) {}

void ShadowNode::EnsureUnsealed() noexcept {
  m_shadowNode->ensureUnsealed();
}

winrt::IInspectable ShadowNode::Tag() const noexcept {
  return m_tag;
}

void ShadowNode::Tag(winrt::IInspectable tag) noexcept {
  m_tag = tag;
}

winrt::IInspectable ShadowNode::StateData() const noexcept {
  auto state = m_shadowNode->getState();
  react_native_assert(state && "State must not be `nullptr`.");
  auto abiStateData =
      static_cast<const facebook::react::ConcreteState<::Microsoft::ReactNative::AbiStateData> *>(state.get())
          ->getData();
  return abiStateData.userdata;
}

void ShadowNode::StateData(winrt::IInspectable tag) noexcept {
  m_shadowNode->ensureUnsealed();

  auto &state = const_cast<facebook::react::State::Shared &>(m_shadowNode->getState());
  state = std::make_shared<const facebook::react::ConcreteState<::Microsoft::ReactNative::AbiStateData>>(
      std::make_shared<const ::Microsoft::ReactNative::AbiStateData>(tag), *state);
}

} // namespace winrt::Microsoft::ReactNative::implementation

namespace Microsoft::ReactNative {

extern const char AbiComponentName[] = "Abi";

void AbiShadowNode::Builder(winrt::Microsoft::ReactNative::IReactViewComponentBuilder builder) noexcept {
  m_builder = builder;
}

winrt::Microsoft::ReactNative::IReactViewComponentBuilder AbiShadowNode::Builder() const noexcept {
  return m_builder;
}

void AbiShadowNode::Proxy(winrt::Microsoft::ReactNative::ShadowNode proxy) noexcept {
  m_proxy = proxy;
}

winrt::Microsoft::ReactNative::ShadowNode AbiShadowNode::Proxy() const noexcept {
  return m_proxy;
}

} // namespace Microsoft::ReactNative
