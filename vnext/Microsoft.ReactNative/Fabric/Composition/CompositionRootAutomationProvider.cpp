#include "pch.h"
#include "CompositionRootAutomationProvider.h"
#include <algorithm>
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
  return UiaSetFocusHelper(m_view);
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

  auto spRootView = std::static_pointer_cast<::Microsoft::ReactNative::RootComponentView>(strongView);

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

// RAII wrapper to unaccess SafeArray data so I can early return in the relevant functions
class SafeArrayAccessScope {
  SAFEARRAY *m_pArray = nullptr;

 public:
  SafeArrayAccessScope(SAFEARRAY *psa) noexcept : m_pArray(psa) {}
  ~SafeArrayAccessScope() noexcept {
    if (m_pArray != nullptr)
      SafeArrayUnaccessData(m_pArray);
  }
};

void AdviseEventAddedImpl(
    std::vector<CompositionRootAutomationProvider::AdvisedEvent> &advisedEvents,
    EVENTID idEvent) noexcept {
  auto it = std::find_if(
      advisedEvents.begin(),
      advisedEvents.end(),
      [idEvent](const CompositionRootAutomationProvider::AdvisedEvent &ae) noexcept { return ae.Event == idEvent; });

  if (it == advisedEvents.end()) {
    advisedEvents.emplace_back(CompositionRootAutomationProvider::AdvisedEvent{idEvent, 1 /*Count*/});
  } else {
    it->Count++;
  }
}

HRESULT CompositionRootAutomationProvider::AdvisePropertiesAdded(SAFEARRAY *psaProperties) noexcept {
  if (psaProperties == nullptr)
    return E_POINTER;

  long *pValues = nullptr;
  auto hr = SafeArrayAccessData(psaProperties, reinterpret_cast<void **>(&pValues));
  if (FAILED(hr))
    return hr;

  SafeArrayAccessScope accessScope(psaProperties);

  if (SafeArrayGetDim(psaProperties) != 1)
    return E_INVALIDARG;

  VARTYPE vt;
  hr = SafeArrayGetVartype(psaProperties, &vt);
  if (FAILED(hr) || vt != VT_I4)
    return E_INVALIDARG;

  long lower;
  hr = SafeArrayGetLBound(psaProperties, 1, &lower);
  if (FAILED(hr))
    return hr;

  long upper;
  hr = SafeArrayGetUBound(psaProperties, 1, &upper);
  if (FAILED(hr))
    return hr;

  long count = upper - lower + 1;

  for (int i = 0; i < count; i++) {
    AdviseEventAddedImpl(m_advisedProperties, pValues[i]);
  }
  return S_OK;
}

HRESULT CompositionRootAutomationProvider::AdviseEventAdded(EVENTID idEvent, SAFEARRAY *psaProperties) {
  if (idEvent == UIA_AutomationPropertyChangedEventId) {
    return AdvisePropertiesAdded(psaProperties);
  }
  AdviseEventAddedImpl(m_advisedEvents, idEvent);
  return S_OK;
}

HRESULT AdviseEventRemovedImpl(
    std::vector<CompositionRootAutomationProvider::AdvisedEvent> &advisedEvents,
    EVENTID idEvent) noexcept {
  auto it = std::find_if(
      advisedEvents.begin(),
      advisedEvents.end(),
      [idEvent](const CompositionRootAutomationProvider::AdvisedEvent &ae) noexcept { return ae.Event == idEvent; });

  if (it == advisedEvents.end()) {
    assert(false);
    return UIA_E_INVALIDOPERATION;
  } else if (it->Count == 1) {
    advisedEvents.erase(it);
  } else {
    it->Count--;
  }
  return S_OK;
}

HRESULT CompositionRootAutomationProvider::AdvisePropertiesRemoved(SAFEARRAY *psaProperties) noexcept {
  if (psaProperties == nullptr)
    return E_POINTER;

  long *pValues = nullptr;
  auto hr = SafeArrayAccessData(psaProperties, reinterpret_cast<void **>(&pValues));
  if (FAILED(hr))
    return hr;

  SafeArrayAccessScope accessScope(psaProperties);

  if (SafeArrayGetDim(psaProperties) != 1)
    return E_INVALIDARG;

  VARTYPE vt;
  hr = SafeArrayGetVartype(psaProperties, &vt);
  if (FAILED(hr) || vt != VT_I4)
    return E_INVALIDARG;

  long lower;
  hr = SafeArrayGetLBound(psaProperties, 1, &lower);
  if (FAILED(hr))
    return hr;

  long upper;
  hr = SafeArrayGetUBound(psaProperties, 1, &upper);
  if (FAILED(hr))
    return hr;

  long count = upper - lower + 1;
  auto returnHr = S_OK;
  for (int i = 0; i < count; i++) {
    auto hr = AdviseEventRemovedImpl(m_advisedProperties, pValues[i]);
    if (FAILED(hr)) {
      returnHr = hr;
    }
  }
  return returnHr;
}

HRESULT
CompositionRootAutomationProvider::AdviseEventRemoved(EVENTID idEvent, SAFEARRAY *psaProperties) {
  if (idEvent == UIA_AutomationPropertyChangedEventId) {
    return AdvisePropertiesRemoved(psaProperties);
  }

  return AdviseEventRemovedImpl(m_advisedEvents, idEvent);
}

bool CompositionRootAutomationProvider::WasEventAdvised(EVENTID event) noexcept {
  return std::any_of(m_advisedEvents.begin(), m_advisedEvents.end(), [event](const AdvisedEvent &ae) {
    return ae.Event == event && ae.Count > 0;
  });
}

bool CompositionRootAutomationProvider::WasPropertyAdvised(PROPERTYID prop) noexcept {
  return std::any_of(m_advisedProperties.begin(), m_advisedProperties.end(), [prop](const AdvisedEvent &ae) {
    return ae.Property == prop && ae.Count > 0;
  });
}

} // namespace winrt::Microsoft::ReactNative::implementation
