#include "pch.h"
#include "CompositionRootAutomationProvider.h"

#include "Fabric/Composition/CompositionRootView.h"

namespace winrt::Microsoft::ReactNative::implementation {

CompositionRootAutomationProvider::CompositionRootAutomationProvider(
    winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::CompositionRootView> &&weakRootControl,
    HWND hwnd) noexcept
    : m_weakRootControl{std::move(weakRootControl)}, m_hwnd{hwnd} {}

// Implementations should return NULL for a top-level element that is hosted in a window. Other elements should return
// an array that contains UiaAppendRuntimeId (defined in Uiautomationcoreapi.h), followed by a value that is unique
// within an instance of the fragment.
//
// We'll use the View pointer as our identifier for those situations
HRESULT __stdcall CompositionRootAutomationProvider::GetRuntimeId(SAFEARRAY **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  return S_OK;
}

HRESULT __stdcall CompositionRootAutomationProvider::GetEmbeddedFragmentRoots(SAFEARRAY **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  return S_OK;
}

HRESULT __stdcall CompositionRootAutomationProvider::SetFocus(void) {
  return S_OK;
}

HRESULT __stdcall CompositionRootAutomationProvider::GetPatternProvider(PATTERNID patternId, IUnknown **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  return S_OK;
}

HRESULT __stdcall CompositionRootAutomationProvider::GetPropertyValue(PROPERTYID propertyId, VARIANT *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  return S_OK;
}

HRESULT __stdcall CompositionRootAutomationProvider::get_HostRawElementProvider(IRawElementProviderSimple **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  if (!IsWindow(m_hwnd))
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto hr = UiaHostProviderFromHwnd(m_hwnd, pRetVal);

  return S_OK;
}

HRESULT __stdcall CompositionRootAutomationProvider::get_BoundingRectangle(UiaRect *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  return S_OK;
}

HRESULT __stdcall CompositionRootAutomationProvider::get_FragmentRoot(IRawElementProviderFragmentRoot **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  AddRef();
  *pRetVal = this;

  return S_OK;
}

HRESULT __stdcall CompositionRootAutomationProvider::get_ProviderOptions(ProviderOptions *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = ProviderOptions_ServerSideProvider | ProviderOptions_UseComThreading;
  return S_OK;
}

HRESULT __stdcall CompositionRootAutomationProvider::ElementProviderFromPoint(
    double x,
    double y,
    IRawElementProviderFragment **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  return S_OK;
}

HRESULT __stdcall CompositionRootAutomationProvider::GetFocus(IRawElementProviderFragment **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  return S_OK;
}

HRESULT __stdcall CompositionRootAutomationProvider::Navigate(
    NavigateDirection direction,
    IRawElementProviderFragment **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  if (!m_weakRootControl)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto strongRootControl = m_weakRootControl.get();



  return S_OK;
}

} // namespace winrt::Microsoft::ReactNative::implementation
