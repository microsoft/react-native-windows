
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Composition/CompositionSwitcher.interop.h>
#include <guid/msoGuid.h>

#include <winrt/Windows.UI.Composition.h>

namespace Microsoft::ReactNative {

namespace Composition {

// Windows composition specific interface to extract the inner composition object
MSO_STRUCT_GUID(ICompositionVisual, "2F7F7E82-0BEA-4Cf8-9531-E81338754187")
struct ICompositionVisual : public IUnknown {
  virtual winrt::Windows::UI::Composition::Visual InnerVisual() const noexcept = 0;
};

// Windows composition specific interface to extract the inner composition shadow object
MSO_STRUCT_GUID(ICompositionDropShadow, "C6474C79-B99A-4312-8FE6-7BC0C9594E5C")
struct ICompositionDropShadow : public IUnknown {
  virtual winrt::Windows::UI::Composition::DropShadow InnerShadow() const noexcept = 0;
};

// Windows composition specific interface to extract the inner composition brush object
MSO_STRUCT_GUID(ICompositionBrush, "CFD63CA2-856E-4D5D-B2E2-68B982407051")
struct ICompositionBrush : public IUnknown {
  virtual winrt::Windows::UI::Composition::CompositionBrush InnerBrush() const noexcept = 0;
};

// Windows composition specific interface to extract the drawing surface object
MSO_STRUCT_GUID(ICompositionDrawingSurfaceInner, "985963C3-1133-43CA-BAD2-AC4C893C3A1F")
struct ICompositionDrawingSurfaceInner : public IUnknown {
  virtual winrt::Windows::UI::Composition::ICompositionSurface Inner() const noexcept = 0;
};

} // namespace Composition

bool CheckForDeviceRemoved(HRESULT hr);

} // namespace Microsoft::ReactNative
