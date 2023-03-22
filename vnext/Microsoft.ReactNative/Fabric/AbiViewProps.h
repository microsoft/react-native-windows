// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ViewProps.g.h"

#include <react/renderer/components/view/ViewProps.h>
#include "winrt/Microsoft.ReactNative.h"

namespace Microsoft::ReactNative {

class AbiViewProps final : public facebook::react::ViewProps {
 public:
  AbiViewProps() = default;
  AbiViewProps(
      const facebook::react::PropsParserContext &context,
      const AbiViewProps &sourceProps,
      const facebook::react::RawProps &rawProps);
  ~AbiViewProps();

  void SetUserProps(
      winrt::Microsoft::ReactNative::IComponentProps componentProps,
      winrt::Microsoft::ReactNative::ViewProps userProps) noexcept;
  winrt::Microsoft::ReactNative::IComponentProps UserProps() const noexcept;

 private:
  winrt::Microsoft::ReactNative::IComponentProps m_componentProps{nullptr};
  winrt::Microsoft::ReactNative::ViewProps m_userProps{nullptr};
};

} // namespace Microsoft::ReactNative

namespace winrt::Microsoft::ReactNative::implementation {

struct UserViewProps : ViewPropsT<UserViewProps> {
  UserViewProps(std::shared_ptr<::Microsoft::ReactNative::AbiViewProps const> viewProps) noexcept;

  float Opacity() noexcept;

  void Disconnect() noexcept;

 private:
  // Use a raw pointer here to avoid a ref cycle with AbiViewProps.  ~AbiViewProps will clear this pointer
  ::Microsoft::ReactNative::AbiViewProps const *m_viewProps{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::implementation
