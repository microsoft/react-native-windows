
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

bool CheckForDeviceRemoved(HRESULT hr);

struct CompContext {
  CompContext(winrt::Windows::UI::Composition::Compositor const &compositor);

  winrt::Windows::UI::Composition::Compositor Compositor() const noexcept;
  winrt::com_ptr<ID2D1Factory1> D2DFactory() noexcept;
  winrt::com_ptr<ID3D11Device> D3DDevice() noexcept;
  winrt::com_ptr<ID2D1Device> D2DDevice() noexcept;
  winrt::Windows::UI::Composition::CompositionGraphicsDevice CompositionGraphicsDevice() noexcept;

 private:
  winrt::Windows::UI::Composition::Compositor m_compositor{nullptr};
  winrt::com_ptr<ID2D1Factory1> m_d2dFactory;
  winrt::com_ptr<ID3D11Device> m_d3dDevice;
  winrt::com_ptr<ID2D1Device> m_d2dDevice;
  winrt::Windows::UI::Composition::CompositionGraphicsDevice m_compositionGraphicsDevice{nullptr};
  winrt::com_ptr<ID3D11DeviceContext> m_d3dDeviceContext;
};

} // namespace Microsoft::ReactNative
