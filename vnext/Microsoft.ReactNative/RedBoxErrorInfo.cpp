// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "RedBoxErrorInfo.h"
#include "DynamicReader.h"
#include "RedBoxErrorFrameInfo.h"
#include "Unicode.h"

#include "winrt/Windows.Foundation.Collections.h"

namespace Mso::React {
RedBoxErrorInfo::RedBoxErrorInfo(Mso::React::ErrorInfo &&errorInfo) : m_errorInfo(std::move(errorInfo)) {}

winrt::hstring RedBoxErrorInfo::Message() const noexcept {
  return ::Microsoft::Common::Unicode::Utf8ToUtf16(m_errorInfo.Message).c_str();
}

winrt::hstring RedBoxErrorInfo::OriginalMessage() const noexcept {
  return ::Microsoft::Common::Unicode::Utf8ToUtf16(m_errorInfo.OriginalMessage).c_str();
}

winrt::hstring RedBoxErrorInfo::Name() const noexcept {
  return ::Microsoft::Common::Unicode::Utf8ToUtf16(m_errorInfo.Name).c_str();
}

winrt::hstring RedBoxErrorInfo::ComponentStack() const noexcept {
  return ::Microsoft::Common::Unicode::Utf8ToUtf16(m_errorInfo.ComponentStack).c_str();
}

winrt::Microsoft::ReactNative::IJSValueReader RedBoxErrorInfo::ExtraData() const noexcept {
  return winrt::make<winrt::Microsoft::ReactNative::DynamicReader>(m_errorInfo.ExtraData);
}

uint32_t RedBoxErrorInfo::Id() const noexcept {
  return m_errorInfo.Id;
}

winrt::Windows::Foundation::Collections::IVectorView<winrt::Microsoft::ReactNative::IRedBoxErrorFrameInfo>
RedBoxErrorInfo::Callstack() noexcept {
  if (!m_callstack) {
    m_callstack = winrt::single_threaded_vector<winrt::Microsoft::ReactNative::IRedBoxErrorFrameInfo>();
    for (auto frame : m_errorInfo.Callstack) {
      m_callstack.Append(winrt::make<RedBoxErrorFrameInfo>(std::move(frame)));
    }
  }

  return m_callstack.GetView();
}

} // namespace Mso::React
