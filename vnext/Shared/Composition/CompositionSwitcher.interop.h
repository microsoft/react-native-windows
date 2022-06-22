
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// These interfaces provide an abstraction that allows us to target either WinComp, or Office's AirSpace composition
// layers

#pragma once

#include <d2d1_1.h>
#include <d3d11.h>
#include <d3d11_4.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>

namespace Microsoft::ReactNative::Composition {

MSO_STRUCT_GUID(ICompositionDrawingSurfaceInterop, "941FDD90-ED27-49CE-A1CD-86ECB2D4A0FA")
struct ICompositionDrawingSurfaceInterop : public IUnknown {
  virtual HRESULT BeginDraw(ID2D1DeviceContext **deviceContextOut, POINT *offset) noexcept = 0;
  virtual HRESULT EndDraw() noexcept = 0;
};

MSO_STRUCT_GUID(IRenderingDeviceReplacedListener, "93A6d34A-0A09-4BE3-94FC-FA3A79D0E0E9")
struct IRenderingDeviceReplacedListener : IUnknown {
  virtual void OnRenderingDeviceLost() = 0;
};

/*
MSO_STRUCT_GUID(ICompositionContext, "80B91D79-CDD2-405A-8440-8891F71DC2E0")
struct ICompositionContext : IUnknown {
  virtual void CreateDrawingSurface(
      winrt::Windows::Foundation::Size surfaceSize,
      winrt::Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat,
      winrt::Windows::Graphics::DirectX::DirectXAlphaMode alphaMode,
      ICompositionDrawingSurfaceInterop **drawingSurfaceOut) noexcept = 0;

  virtual winrt::Microsoft::ReactNative::Composition::SpriteVisual CreateSpriteVisual() noexcept = 0;
  virtual winrt::Microsoft::ReactNative::Composition::ScrollVisual CreateScrollerVisual() noexcept = 0;
  virtual winrt::Microsoft::ReactNative::Composition::IVisual CreateCaratVisual() noexcept = 0;
  virtual winrt::Microsoft::ReactNative::Composition::IDropShadow CreateDropShadow() noexcept = 0;
  virtual winrt::Microsoft::ReactNative::Composition::IBrush CreateColorBrush(winrt::Windows::UI::Color color) noexcept
= 0; virtual winrt::Microsoft::ReactNative::Composition::SurfaceBrush
CreateSurfaceBrush(ICompositionDrawingSurfaceInterop *surface) noexcept = 0;

  // TODO Add and hook up to rootnode - to notify the tree
  // virtual void add_RenderingDeviceReplaced(const IRenderingDeviceReplacedListener& listener) noexcept = 0;
  // virtual void remove_RenderingDeviceReplaced(const IRenderingDeviceReplacedListener& listener) noexcept = 0;
};
*/

} // namespace Microsoft::ReactNative::Composition
