// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <Shared/IRedBoxHandler.h>
#include "winrt/Microsoft.ReactNative.h"

namespace Mso::React {

struct RedBoxErrorFrameInfo
    : public winrt::implements<RedBoxErrorFrameInfo, winrt::Microsoft::ReactNative::IRedBoxErrorFrameInfo> {
  RedBoxErrorFrameInfo(Mso::React::ErrorFrameInfo &&errorFrameInfo) : m_frame(std::move(errorFrameInfo)) {}

  winrt::hstring File() const noexcept;

  winrt::hstring Method() const noexcept;

  uint32_t Line() const noexcept;

  uint32_t Column() const noexcept;

  bool Collapse() const noexcept;

 private:
  Mso::React::ErrorFrameInfo m_frame;
};

} // namespace Mso::React
