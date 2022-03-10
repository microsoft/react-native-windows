
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CompParagraphComponentView.h"

#include <Utils/ValueUtils.h>
#include <Windows.Graphics.DirectX.Direct3D11.interop.h>
#include <d2d1_1.h>
#include <d3d11.h>
#include <d3d11_4.h>
#include <dwrite.h>
#include <unicode.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>

namespace Microsoft::ReactNative {

// TODO these are more global than they should be

winrt::com_ptr<ID2D1Factory1> D2DFactory();
winrt::com_ptr<ID3D11Device> D3DDevice();

winrt::com_ptr<ID2D1Device> D2DDevice();

winrt::Windows::UI::Composition::CompositionGraphicsDevice CompositionGraphicsDevice(
    const winrt::Windows::UI::Composition::Compositor &compositor);

winrt::com_ptr<::IDWriteFactory> DWriteFactory();

bool CheckForDeviceRemoved(HRESULT hr);

} // namespace Microsoft::ReactNative
