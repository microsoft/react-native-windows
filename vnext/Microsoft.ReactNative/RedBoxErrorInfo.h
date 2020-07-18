// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <Shared/IRedBoxHandler.h>
#include "winrt/Microsoft.ReactNative.h"

namespace Mso::React {

struct RedBoxErrorInfo : public winrt::implements<RedBoxErrorInfo, winrt::Microsoft::ReactNative::IRedBoxErrorInfo> {
  RedBoxErrorInfo(Mso::React::ErrorInfo &&errorInfo);

  winrt::hstring Message() const noexcept;
  winrt::hstring OriginalMessage() const noexcept;
  winrt::hstring Name() const noexcept;
  winrt::hstring ComponentStack() const noexcept;
  winrt::Microsoft::ReactNative::IJSValueReader ExtraData() const noexcept;
  uint32_t Id() const noexcept;

  winrt::Windows::Foundation::Collections::IVectorView<winrt::Microsoft::ReactNative::IRedBoxErrorFrameInfo>
  Callstack() noexcept;

 private:
  winrt::Windows::Foundation::Collections::IVector<winrt::Microsoft::ReactNative::IRedBoxErrorFrameInfo> m_callstack{
      nullptr};

  Mso::React::ErrorInfo m_errorInfo;
};

} // namespace Mso::React
