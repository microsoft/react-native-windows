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

AbiViewProps::~AbiViewProps() {
  if (m_userProps) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::UserViewProps>(m_userProps)->Disconnect();
  }
}

void AbiViewProps::SetUserProps(
    winrt::Microsoft::ReactNative::IComponentProps componentProps,
    winrt::Microsoft::ReactNative::ViewProps userProps) noexcept {
  m_componentProps = componentProps;
  m_userProps = userProps;
}

winrt::Microsoft::ReactNative::IComponentProps AbiViewProps::UserProps() const noexcept {
  return m_componentProps;
}

} // namespace Microsoft::ReactNative

namespace winrt::Microsoft::ReactNative::implementation {

UserViewProps::UserViewProps(std::shared_ptr<::Microsoft::ReactNative::AbiViewProps const> viewProps) noexcept
    : m_viewProps(viewProps.get()) {}

void UserViewProps::Disconnect() noexcept {
  m_viewProps = nullptr;
}

float UserViewProps::Opacity() noexcept {
  return m_viewProps ? m_viewProps->opacity : 1.0f;
}

} // namespace winrt::Microsoft::ReactNative::implementation
