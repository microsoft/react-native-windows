// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "RedBoxHandler.h"
#if __has_include("RedBox.RedBoxHandler.g.cpp")
#include "RedBox.RedBoxHandler.g.cpp"
#endif

#include <ReactNativeHost.h>
#include <RedBox.h>
#include <unicode.h>

namespace winrt::Microsoft::ReactNative::RedBox::implementation {

RedBoxHandler::RedBoxHandler(ReactNativeHost const &host) noexcept {
  auto hostImpl = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeHost>(host);
  Mso::WeakPtr<Mso::React::IReactHost> wkHost(hostImpl->ReactHost());
  m_redBoxHandler = Mso::React::CreateDefaultRedBoxHandler(std::move(wkHost));
}

Mso::React::ErrorInfo CreateErrorInfo(IErrorInfo info) {
  Mso::React::ErrorInfo ei;
  ei.Id = info.Id();
  ei.Message = ::Microsoft::Common::Unicode::Utf16ToUtf8(info.Message());
  for (auto frame : info.Callstack()) {
    Mso::React::ErrorFrameInfo efi;
    efi.Method = ::Microsoft::Common::Unicode::Utf16ToUtf8(frame.Method());
    efi.File = ::Microsoft::Common::Unicode::Utf16ToUtf8(frame.File());
    efi.Line = frame.Line();
    efi.Column = frame.Column();
    ei.Callstack.push_back(std::move(efi));
  }
  return std::move(ei);
}

inline void RedBoxHandler::ShowNewError(IErrorInfo const &info, ErrorType type) noexcept {
  m_redBoxHandler->showNewError(
      std::move(CreateErrorInfo(info)), static_cast<Mso::React::ErrorType>(static_cast<uint32_t>(type)));
}

inline bool RedBoxHandler::IsDevSupportEnabled() noexcept {
  return m_redBoxHandler->isDevSupportEnabled();
}

inline void RedBoxHandler::UpdateError(IErrorInfo const &info) noexcept {
  m_redBoxHandler->updateError(std::move(CreateErrorInfo(info)));
}

inline void RedBoxHandler::DismissRedBox() noexcept {
  m_redBoxHandler->dismissRedbox();
}

} // namespace winrt::Microsoft::ReactNative::RedBox::implementation
