// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "RedBox.RedBoxHandler.g.h"

namespace Mso::React {
struct IRedBoxHandler;
}

namespace winrt::Microsoft::ReactNative::RedBox::implementation {

struct RedBoxHandler : RedBoxHandlerT<RedBoxHandler> {
  RedBoxHandler(ReactNativeHost const &host) noexcept;

  void ShowNewError(IErrorInfo const &info, ErrorType type) noexcept;
  bool IsDevSupportEnabled() noexcept;
  void UpdateError(IErrorInfo const &info) noexcept;
  void DismissRedBox() noexcept;
private:
  std::shared_ptr<Mso::React::IRedBoxHandler> m_redBoxHandler;
};

} // namespace winrt::Microsoft::ReactNative::RedBox::implementation

namespace winrt::Microsoft::ReactNative::RedBox::factory_implementation {

struct RedBoxHandler : RedBoxHandlerT<RedBoxHandler, implementation::RedBoxHandler> {};

} // namespace winrt::Microsoft::ReactNative::RedBox::factory_implementation

namespace winrt::Microsoft::ReactNative::RedBox::implementation {

} // namespace winrt::Microsoft::ReactNative::RedBox::implementation
