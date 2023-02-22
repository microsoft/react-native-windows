#include "pch.h"
#include "CompositionDynamicAutomationProvider.h"

namespace winrt::Microsoft::ReactNative::implementation {

CompositionDynamicAutomationProvider::CompositionDynamicAutomationProvider(
  const std::shared_ptr<IComponentView>& componentView
) noexcept
{

}

HRESULT __stdcall CompositionDynamicAutomationProvider::Navigate(
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

// Implementations should return NULL for a top-level element that is hosted in a window. Other elements should return
// an array that contains UiaAppendRuntimeId (defined in Uiautomationcoreapi.h), followed by a value that is unique
// within an instance of the fragment.
//
// We'll use the View pointer as our identifier for those situations
HRESULT __stdcall CompositionDynamicAutomationProvider::GetRuntimeId(SAFEARRAY **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_BoundingRectangle(UiaRect *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::GetEmbeddedFragmentRoots(SAFEARRAY **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::SetFocus(void) {
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_FragmentRoot(IRawElementProviderFragmentRoot **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  return E_NOTIMPL;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_ProviderOptions(ProviderOptions *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = ProviderOptions_ServerSideProvider | ProviderOptions_UseComThreading;
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::GetPatternProvider(PATTERNID patternId, IUnknown **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::GetPropertyValue(PROPERTYID propertyId, VARIANT *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_HostRawElementProvider(IRawElementProviderSimple **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::ShowContextMenu() noexcept {
  return E_NOTIMPL;
}

}