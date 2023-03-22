#include "pch.h"
#include "CompositionRootAutomationProvider.h"
#include "UiaHelpers.h"

namespace winrt::Microsoft::ReactNative::implementation {

CompositionRootAutomationProvider::CompositionRootAutomationProvider(
    const std::shared_ptr<::Microsoft::ReactNative::RootComponentView> &componentView) noexcept
    : m_view{componentView} {}

// Implementations should return NULL for a top-level element that is hosted in a window. Other elements should return
// an array that contains UiaAppendRuntimeId (defined in Uiautomationcoreapi.h), followed by a value that is unique
// within an instance of the fragment.
//
// We'll use the react tag as our identifier for those situations
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

  auto strongView = m_view.view();

  if (strongView == nullptr) {
    return UIA_E_ELEMENTNOTAVAILABLE;
  }

  auto rootCV = std::static_pointer_cast<::Microsoft::ReactNative::RootComponentView>(strongView);
  auto focusedComponent = rootCV->GetFocusedComponent();

  if (focusedComponent) {
    auto focusedUiaProvider = focusedComponent->EnsureUiaProvider();
    winrt::com_ptr<IRawElementProviderFragment> spFragment;
    focusedUiaProvider.as(spFragment);
    *pRetVal = spFragment.detach();
  }

  return S_OK;
}

void CompositionRootAutomationProvider::SetHwnd(HWND hwnd) noexcept {
  m_hwnd = hwnd;
}

HRESULT __stdcall CompositionRootAutomationProvider::Navigate(
    NavigateDirection direction,
    IRawElementProviderFragment **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  // Fragment roots do not enable navigation to a parent or siblings; navigation among fragment roots is handled by the
  // default window providers. Elements in fragments must navigate only to other elements within that fragment.
  if (direction == NavigateDirection_FirstChild || direction == NavigateDirection_LastChild) {
    return UiaNavigateHelper(m_view, direction, *pRetVal);
  }
  *pRetVal = nullptr;
  return S_OK;
}

} // namespace winrt::Microsoft::ReactNative::implementation
