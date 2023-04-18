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

  // TODO: assumes windowed
  if (!IsWindow(m_hwnd))
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto hr = UiaHostProviderFromHwnd(m_hwnd, pRetVal);

  return S_OK;
}

HRESULT __stdcall CompositionRootAutomationProvider::get_BoundingRectangle(UiaRect *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  // TODO: Need host site offsets
  // Assume we're hosted in some other visual-based hosting site
  if (m_hwnd == nullptr || !IsWindow(m_hwnd)) {
    return UiaGetBoundingRectangleHelper(m_view, *pRetVal);
  }

  POINT point{0, 0};
  ClientToScreen(m_hwnd, &point);
  pRetVal->left = point.x;
  pRetVal->top = point.y;
  RECT rect;
  GetClientRect(m_hwnd, &rect);
  point.x = rect.right;
  point.y = rect.bottom;
  ClientToScreen(m_hwnd, &point);
  pRetVal->width = point.x - pRetVal->left;
  pRetVal->height = point.y - pRetVal->top;

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

  auto strongView = m_view.view();

  if (strongView == nullptr) {
    return UIA_E_ELEMENTNOTAVAILABLE;
  }

  auto spRootView = strongView->rootComponentView();
  if (spRootView == nullptr) {
    return UIA_E_ELEMENTNOTAVAILABLE;
  }

  if (m_hwnd == nullptr || !IsWindow(m_hwnd)) {
    // TODO: Add support for non-HWND based hosting
    return E_FAIL;
  }

  POINT clientPoint{static_cast<LONG>(x), static_cast<LONG>(y)};
  ScreenToClient(m_hwnd, &clientPoint);

  auto provider = spRootView->UiaProviderFromPoint(clientPoint);
  auto spFragment = provider.try_as<IRawElementProviderFragment>();
  if (spFragment) {
    *pRetVal = spFragment.detach();
  }

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
    auto spFragment = focusedUiaProvider.try_as<IRawElementProviderFragment>();
    if (spFragment) {
      *pRetVal = spFragment.detach();
    }
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
