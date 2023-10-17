
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Composition/CompositionSwitcher.interop.h>
#include <winrt/Windows.UI.Composition.h>

namespace Microsoft::ReactNative {

namespace Composition {

// Windows composition specific interface to extract the inner compositor object
MSO_STRUCT_GUID(IWindowsCompositionCompositor, "4FA84446-B662-440F-AE53-43A223D2068E")
struct IWindowsCompositionCompositor : public IUnknown {
  virtual winrt::Windows::UI::Composition::Compositor InnerCompositor() const noexcept = 0;
};

// Windows composition specific interface to extract the inner composition object
MSO_STRUCT_GUID(IWindowsCompositionVisual, "2F7F7E82-0BEA-4Cf8-9531-E81338754187")
struct IWindowsCompositionVisual : public IUnknown {
  virtual winrt::Windows::UI::Composition::Visual InnerVisual() const noexcept = 0;
};

// Windows composition specific interface to extract the inner composition shadow object
MSO_STRUCT_GUID(IWindowsCompositionDropShadow, "C6474C79-B99A-4312-8FE6-7BC0C9594E5C")
struct IWindowsCompositionDropShadow : public IUnknown {
  virtual winrt::Windows::UI::Composition::DropShadow InnerShadow() const noexcept = 0;
};

// Windows composition specific interface to extract the inner composition brush object
MSO_STRUCT_GUID(IWindowsCompositionBrush, "CFD63CA2-856E-4D5D-B2E2-68B982407051")
struct IWindowsCompositionBrush : public IUnknown {
  virtual winrt::Windows::UI::Composition::CompositionBrush InnerBrush() const noexcept = 0;
};

// Windows composition specific interface to extract the drawing surface object
MSO_STRUCT_GUID(IWindowsCompositionDrawingSurfaceInner, "985963C3-1133-43CA-BAD2-AC4C893C3A1F")
struct IWindowsCompositionDrawingSurfaceInner : public IUnknown {
  virtual winrt::Windows::UI::Composition::ICompositionSurface Inner() const noexcept = 0;
};

#ifdef USE_WINUI3
// Windows composition specific interface to extract the inner compositor object
MSO_STRUCT_GUID(IMicrosoftCompositionCompositor, "B7B3E027-6A87-4946-B397-247AAB8634C6")
struct IMicrosoftCompositionCompositor : public IUnknown {
  virtual winrt::Microsoft::UI::Composition::Compositor InnerCompositor() const noexcept = 0;
};

// Microsoft composition specific interface to extract the inner composition object
MSO_STRUCT_GUID(IMicrosoftCompositionVisual, "FF8731D8-8AFA-4F6F-9E76-7F538F3C1157")
struct IMicrosoftCompositionVisual : public IUnknown {
  virtual winrt::Microsoft::UI::Composition::Visual InnerVisual() const noexcept = 0;
};

// Microsoft composition specific interface to extract the inner composition shadow object
MSO_STRUCT_GUID(IMicrosoftCompositionDropShadow, "78E94CC1-8E8A-4CF7-9301-23305FFC97A2")
struct IMicrosoftCompositionDropShadow : public IUnknown {
  virtual winrt::Microsoft::UI::Composition::DropShadow InnerShadow() const noexcept = 0;
};

// Microsoft composition specific interface to extract the inner composition brush object
MSO_STRUCT_GUID(IMicrosoftCompositionBrush, "831CA206-B777-4660-88BC-19EB78045E40")
struct IMicrosoftCompositionBrush : public IUnknown {
  virtual winrt::Microsoft::UI::Composition::CompositionBrush InnerBrush() const noexcept = 0;
};

// Microsoft composition specific interface to extract the drawing surface object
MSO_STRUCT_GUID(IMicrosoftCompositionDrawingSurfaceInner, "4ADDC62F-51D0-4C71-8BE4-3256E628D07A")
struct IMicrosoftCompositionDrawingSurfaceInner : public IUnknown {
  virtual winrt::Microsoft::UI::Composition::ICompositionSurface Inner() const noexcept = 0;
};
#endif

} // namespace Composition

bool CheckForDeviceRemoved(HRESULT hr);

} // namespace Microsoft::ReactNative
