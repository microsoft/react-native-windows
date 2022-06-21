
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// These interfaces provide an abstraction that allows us to target either WinComp, or Office's AirSpace composition
// layers

#pragma once

#include <d2d1_1.h>
#include <d3d11.h>
#include <d3d11_4.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>

namespace Microsoft::ReactNative::Composition {

MSO_STRUCT_GUID(ICompositionDrawingSurface, "941FDD90-ED27-49CE-A1CD-86ECB2D4A0FA")
struct ICompositionDrawingSurface : public IUnknown {
  virtual HRESULT BeginDraw(ID2D1DeviceContext **deviceContextOut, POINT *offset) noexcept = 0;
  virtual HRESULT EndDraw() noexcept = 0;
};

enum class CompositionStretch {
  None,
  Fill,
  Uniform,
  UniformToFill,
};

MSO_STRUCT_GUID(IBrush, "692F58B0-7A15-4716-8D63-00061D3FA363")
struct IBrush : public IUnknown {};

MSO_STRUCT_GUID(ISurfaceBrush, "4ED1484D-687C-439E-BD39-157680B5052D")
struct ISurfaceBrush : public IBrush {
  virtual void HorizontalAlignmentRatio(float ratio) noexcept = 0;
  virtual void VerticalAlignmentRatio(float ratio) noexcept = 0;
  virtual void Stretch(CompositionStretch mode) noexcept = 0;
};

MSO_STRUCT_GUID(IDropShadow, "1AE6DF3C-8E86-4552-A78E-5C1C5AFEFC84")
struct IDropShadow : public IUnknown {
  virtual void Offset(winrt::Windows::Foundation::Numerics::float3 const &offset) noexcept = 0;
  virtual void Opacity(float opacity) noexcept = 0;
  virtual void BlurRadius(float radius) noexcept = 0;
  virtual void Color(winrt::Windows::UI::Color color) noexcept = 0;
};

MSO_STRUCT_GUID(IVisual, "2FD536CA-289D-42F8-9540-D65BB20D2419")
struct IVisual : public IUnknown {
  virtual void InsertAt(IVisual *visual, uint32_t index) noexcept = 0;
  virtual void Remove(IVisual *visual) noexcept = 0;
  virtual void Opacity(float opacity) noexcept = 0;
  virtual void Scale(winrt::Windows::Foundation::Numerics::float3 const &scale) noexcept = 0;
  virtual void RotationAngle(float rotation) noexcept = 0;
  virtual void IsVisible(bool IsVisible) noexcept = 0;
  virtual void Size(winrt::Windows::Foundation::Numerics::float2 const &size) noexcept = 0;
  virtual void Offset(winrt::Windows::Foundation::Numerics::float3 const &offset) noexcept = 0;
};

MSO_STRUCT_GUID(ISpriteVisual, "00D8A172-5727-4A69-B97F-B588656D7AF5")
struct ISpriteVisual : public IVisual {
  virtual void Brush(IBrush *brush) noexcept = 0;
  virtual void Shadow(IDropShadow *shadow) noexcept = 0;
};

MSO_STRUCT_GUID(IScrollerVisual, "DF33137A-3436-43FC-86EF-FC3B54370C84")
struct IScrollerVisual : public ISpriteVisual {
  virtual void Brush(IBrush *brush) noexcept = 0;
  // TODO use proper event
  virtual void SetOnScrollCallback(
      std::function<void(winrt::Windows::Foundation::Numerics::float2)> &&callback) noexcept = 0;
  virtual void ContentSize(winrt::Windows::Foundation::Numerics::float2 const &size) noexcept = 0;
  virtual winrt::Windows::Foundation::Numerics::float3 const ScrollPosition() noexcept = 0;
  virtual void ScrollBy(winrt::Windows::Foundation::Numerics::float3 const &offset) noexcept = 0;
};

MSO_STRUCT_GUID(IRenderingDeviceReplacedListener, "93A6d34A-0A09-4BE3-94FC-FA3A79D0E0E9")
struct IRenderingDeviceReplacedListener : IUnknown {
  virtual void OnRenderingDeviceLost() = 0;
};

MSO_STRUCT_GUID(ICompositionContext, "80B91D79-CDD2-405A-8440-8891F71DC2E0")
struct ICompositionContext : IUnknown {
  virtual void CreateDrawingSurface(
      winrt::Windows::Foundation::Size surfaceSize,
      winrt::Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat,
      winrt::Windows::Graphics::DirectX::DirectXAlphaMode alphaMode,
      ICompositionDrawingSurface **drawingSurfaceOut) noexcept = 0;

  virtual void CreateSpriteVisual(ISpriteVisual **visualOut) noexcept = 0;
  virtual void CreateScrollerVisual(IScrollerVisual **scrollerOut) noexcept = 0;
  virtual void CreateCaratVisual(IVisual **visualOut) noexcept = 0;
  virtual void CreateDropShadow(IDropShadow **shadowOut) noexcept = 0;
  virtual void CreateColorBrush(winrt::Windows::UI::Color color, IBrush **brushOut) noexcept = 0;
  virtual void CreateSurfaceBrush(ICompositionDrawingSurface *surface, ISurfaceBrush **surfaceBrushOut) noexcept = 0;

  // TODO Add and hook up to rootnode - to notify the tree
  // virtual void add_RenderingDeviceReplaced(const IRenderingDeviceReplacedListener& listener) noexcept = 0;
  // virtual void remove_RenderingDeviceReplaced(const IRenderingDeviceReplacedListener& listener) noexcept = 0;
};

} // namespace Microsoft::ReactNative::Composition
