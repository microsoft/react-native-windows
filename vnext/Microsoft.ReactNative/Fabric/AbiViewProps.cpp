// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "AbiViewProps.h"

namespace Microsoft::ReactNative {

AbiViewProps::AbiViewProps(
    const facebook::react::PropsParserContext &context,
    const AbiViewProps &sourceProps,
    const facebook::react::RawProps &rawProps)
    : facebook::react::ViewProps(context, sourceProps, rawProps) {}

void AbiViewProps::SetUserProps(winrt::Microsoft::ReactNative::IComponentProps userProps) noexcept {
  m_userProps = userProps;
}

winrt::Microsoft::ReactNative::IComponentProps AbiViewProps::UserProps() const noexcept {
  return m_userProps;
}

} // namespace Microsoft::ReactNative

namespace winrt::Microsoft::ReactNative::implementation {

UserViewProps::UserViewProps(std::shared_ptr<::Microsoft::ReactNative::AbiViewProps const> viewProps) noexcept: m_viewProps(viewProps) {
}

float UserViewProps::Opacity() noexcept {
  return m_viewProps->opacity;
}

} // namespace winrt::Microsoft::ReactNative::implementation
