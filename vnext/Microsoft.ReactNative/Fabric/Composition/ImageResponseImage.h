
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <wincodec.h>
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct ImageResponseImage {
  winrt::com_ptr<IWICBitmap> m_wicbmp;
  winrt::Microsoft::ReactNative::Composition::Experimental::UriBrushFactory m_brushFactory{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
