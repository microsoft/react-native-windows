// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "TextInputProps.g.h"

#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/imagemanager/primitives.h>
#include "winrt/Microsoft.ReactNative.Composition.Experimental.h"
#include "winrt/Microsoft.ReactNative.h"
#include "Composition/TextInput/WindowsTextInputProps.h"

namespace Microsoft::ReactNative {

class AbiTextInputProps final : public facebook::react::WindowsTextInputProps
{
 public:
  AbiTextInputProps() = default;
  AbiTextInputProps(
      const facebook::react::PropsParserContext &context,
      const AbiTextInputProps &sourceProps,
      const facebook::react::RawProps &rawProps);
  ~AbiTextInputProps();

  void SetUserProps(
      winrt::Microsoft::ReactNative::IComponentProps userProps,
      winrt::Microsoft::ReactNative::ViewProps innerProps) noexcept;
  winrt::Microsoft::ReactNative::IComponentProps UserProps() const noexcept;
  winrt::Microsoft::ReactNative::ViewProps ViewProps() const noexcept;

 private:
  winrt::Microsoft::ReactNative::IComponentProps m_userProps{nullptr};
  winrt::Microsoft::ReactNative::ViewProps m_innerProps{nullptr};
};

} // namespace Microsoft::ReactNative

namespace winrt::Microsoft::ReactNative::implementation {

struct TextInputProps : TextInputPropsT<TextInputProps, ViewProps> {
  using Super = TextInputPropsT<TextInputProps, ViewProps>;
  TextInputProps(facebook::react::SharedViewProps props, bool holdRef = true) noexcept;

  winrt::hstring Placeholder() noexcept;
  bool SecureTextEntry() noexcept;

protected:
  const facebook::react::WindowsTextInputProps &windowsTextInputProps() const noexcept;
};

} // namespace winrt::Microsoft::ReactNative::implementation
