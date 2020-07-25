// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "RedBoxErrorFrameInfo.h"
#include "Unicode.h"

namespace Mso::React {

winrt::hstring RedBoxErrorFrameInfo::File() const noexcept {
  return ::Microsoft::Common::Unicode::Utf8ToUtf16(m_frame.File).c_str();
}

winrt::hstring RedBoxErrorFrameInfo::Method() const noexcept {
  return ::Microsoft::Common::Unicode::Utf8ToUtf16(m_frame.Method).c_str();
}

uint32_t RedBoxErrorFrameInfo::Line() const noexcept {
  return m_frame.Line;
}

uint32_t RedBoxErrorFrameInfo::Column() const noexcept {
  return m_frame.Column;
}

bool RedBoxErrorFrameInfo::Collapse() const noexcept {
  return m_frame.Collapse;
}

} // namespace Mso::React
