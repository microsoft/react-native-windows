// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "AbiShadowNode.h"

#include <Fabric/Composition/ReactCompositionViewComponentBuilder.h>
#include <react/debug/react_native_assert.h>
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
