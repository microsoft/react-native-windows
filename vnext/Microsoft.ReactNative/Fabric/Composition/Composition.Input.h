// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <winrt/Microsoft.ReactNative.Composition.Input.h>
#include <react/renderer/core/ReactPrimitives.h>

namespace winrt::Microsoft::ReactNative::Composition::Input::implementation {

struct KeyRoutedEventArgs : winrt::implements<KeyRoutedEventArgs, winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs, winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs>
{
  KeyRoutedEventArgs(facebook::react::Tag tag, uint32_t msg, uint64_t wParam, int64_t lParam);

  int32_t OriginalSource() noexcept;
  winrt::hstring DeviceId() noexcept;
  bool Handled() noexcept;
  void Handled(bool value) noexcept;
  winrt::Windows::System::VirtualKey Key() noexcept;
  winrt::Windows::UI::Core::CorePhysicalKeyStatus KeyStatus() noexcept;
  winrt::Windows::System::VirtualKey OriginalKey() noexcept;

private:
  facebook::react::Tag m_tag { -1 };
  bool m_handled { false };
  winrt::Windows::System::VirtualKey m_key;
  winrt::Windows::UI::Core::CorePhysicalKeyStatus m_keyStatus;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
