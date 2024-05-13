// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// These interfaces provide an abstraction that allows us to target either Windows Composition, or Office's AirSpace
// composition layers

#pragma once

#include <d2d1_1.h>
#include <d3d11.h>
#include <d3d11_4.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>

namespace Microsoft::ReactNative::Composition::Experimental {

struct __declspec(uuid("941FDD90-ED27-49CE-A1CD-86ECB2D4A0FA")) ICompositionDrawingSurfaceInterop : public IUnknown {
  virtual HRESULT BeginDraw(ID2D1DeviceContext **deviceContextOut, float xDpi, float yDpi, POINT *offset) noexcept = 0;
  virtual HRESULT EndDraw() noexcept = 0;
};

struct __declspec(uuid("93A6d34A-0A09-4BE3-94FC-FA3A79D0E0E9")) IRenderingDeviceReplacedListener : IUnknown {
  virtual void OnRenderingDeviceLost() = 0;
};

struct __declspec(uuid("50E66581-E917-45F4-AC22-1BC953CFA2A8")) IVisualInterop : IUnknown {
  virtual void SetClippingPath(ID2D1Geometry *clippingPath) noexcept = 0;
};

struct __declspec(uuid("4742F122-3EE0-48AA-9EA9-44A00147B55F")) ICompositionContextInterop : IUnknown {
  virtual void D2DFactory(ID2D1Factory1 **outD2DFactory) noexcept = 0;
};

} // namespace Microsoft::ReactNative::Composition::Experimental
