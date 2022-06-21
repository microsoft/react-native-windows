
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Composition/CompositionSwitcher.h>

#include <winrt/Windows.UI.Composition.h>

namespace Microsoft::ReactNative {

namespace Composition {

// WinComp specific interface to extract the inner composition object
MSO_STRUCT_GUID(ICompositionVisual, "2F7F7E82-0BEA-4Cf8-9531-E81338754187")
struct ICompositionVisual : public IUnknown {
  virtual winrt::Windows::UI::Composition::Visual InnerVisual() const noexcept = 0;
};

// WinComp specific interface to extract the inner composition shadow object
MSO_STRUCT_GUID(ICompositionDropShadow, "C6474C79-B99A-4312-8FE6-7BC0C9594E5C")
struct ICompositionDropShadow : public IUnknown {
  virtual winrt::Windows::UI::Composition::DropShadow InnerShadow() const noexcept = 0;
};

// WinComp specific interface to extract the inner composition brush object
MSO_STRUCT_GUID(ICompositionBrush, "CFD63CA2-856E-4D5D-B2E2-68B982407051")
struct ICompositionBrush : public IUnknown {
  virtual winrt::Windows::UI::Composition::CompositionBrush InnerBrush() const noexcept = 0;
};

// WinComp specific interface to extract the drawing surface object
MSO_STRUCT_GUID(ICompositionDrawingSurfaceInner, "985963C3-1133-43CA-BAD2-AC4C893C3A1F")
struct ICompositionDrawingSurfaceInner : public IUnknown {
  virtual winrt::Windows::UI::Composition::ICompositionSurface Inner() const noexcept = 0;
};

winrt::Windows::UI::Composition::Visual CompVisualFromVisual(
    const winrt::com_ptr<Composition::IVisual> &visual) noexcept;
winrt::Windows::UI::Composition::DropShadow CompShadowFromShadow(const winrt::com_ptr<IDropShadow> &shadow) noexcept;
winrt::Windows::UI::Composition::CompositionBrush CompBrushFromBrush(const winrt::com_ptr<IBrush> &brush) noexcept;
winrt::Windows::UI::Composition::ICompositionSurface CompDrawingSurfaceFromDrawingSurface(
    const winrt::com_ptr<ICompositionDrawingSurface> &surface) noexcept;

} // namespace Composition

bool CheckForDeviceRemoved(HRESULT hr);

} // namespace Microsoft::ReactNative
