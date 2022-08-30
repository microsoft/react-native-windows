// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "DWriteHelpers.h"

namespace Microsoft::ReactNative {

winrt::com_ptr<::IDWriteFactory> DWriteFactory() noexcept {
  static winrt::com_ptr<::IDWriteFactory> s_dwriteFactory;
  if (!s_dwriteFactory) {
    winrt::check_hresult(::DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED, __uuidof(s_dwriteFactory), reinterpret_cast<::IUnknown **>(s_dwriteFactory.put())));
  }
  return s_dwriteFactory;
}

} // namespace Microsoft::ReactNative
