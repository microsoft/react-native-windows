#include "pch.h"
#include "CompositionRootAutomationProvider.h"
#include <algorithm>
#include "UiaHelpers.h"

#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Input.h>
#endif

#include <Fabric/Composition/ReactNativeIsland.h>

namespace winrt::Microsoft::ReactNative::implementation {

CompositionRootAutomationProvider::CompositionRootAutomationProvider(
    const winrt::Microsoft::ReactNative::ReactNativeIsland &rootView) noexcept
    : m_wkRootView{rootView} {}

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
#ifdef USE_WINUI3
  if (m_island) {
    auto focusController = winrt::Microsoft::UI::Input::InputFocusController::GetForIsland(m_island);

    if (focusController) {
      if (focusController.TrySetFocus()) {
        return S_OK;
      }
    }
  }
#endif

  if (m_hwnd) {
    ::SetFocus(m_hwnd);
    return S_OK;
  }

  return S_FALSE;
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

  switch (propertyId) {
    case UIA_ControlTypePropertyId: {
      pRetVal->vt = VT_I4;
      pRetVal->lVal = UIA_GroupControlTypeId;
      break;
    }
  }

  return S_OK;
}

HRESULT __stdcall CompositionRootAutomationProvider::get_HostRawElementProvider(IRawElementProviderSimple **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

#ifdef USE_WINUI3
  if (m_island) {
    winrt::Windows::Foundation::IInspectable host = m_island.GetAutomationHostProvider();
    *pRetVal = host.as<IRawElementProviderSimple>().detach();
    return S_OK;
  }
#endif

  // TODO: assumes windowed
  if (!IsWindow(m_hwnd)) {
    assert(false);
    return UIA_E_ELEMENTNOTAVAILABLE;
  }

  auto hr = UiaHostProviderFromHwnd(m_hwnd, pRetVal);

  return S_OK;
}

HRESULT __stdcall CompositionRootAutomationProvider::get_BoundingRectangle(UiaRect *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

#ifdef USE_WINUI3
  if (m_island) {
    auto cc = m_island.CoordinateConverter();
    auto origin = cc.ConvertLocalToScreen(winrt::Windows::Foundation::Point{0, 0});
    pRetVal->left = origin.X;
    pRetVal->top = origin.Y;

    auto size = m_island.ActualSize();
    pRetVal->width = size.x;
    pRetVal->height = size.y;

    return S_OK;
  }
#endif

  // TODO: Need host site offsets
  // Assume we're hosted in some other visual-based hosting site
  if (m_hwnd == nullptr || !IsWindow(m_hwnd)) {
    assert(false);
    pRetVal->left = pRetVal->top = pRetVal->width = pRetVal->height = 0;
    return E_FAIL;
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

winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView *
CompositionRootAutomationProvider::rootComponentView() noexcept {
  if (auto rootView = m_wkRootView.get()) {
    auto innerRootView = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(rootView);
    if (auto view = innerRootView->GetComponentView()) {
      return view;
    }
  }

  return nullptr;
}

HRESULT __stdcall CompositionRootAutomationProvider::ElementProviderFromPoint(
    double x,
    double y,
    IRawElementProviderFragment **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  if (auto rootView = rootComponentView()) {
#ifdef USE_WINUI3
    if (m_island) {
      auto cc = m_island.CoordinateConverter();
      auto local = cc.ConvertScreenToLocal(
          winrt::Windows::Graphics::PointInt32{static_cast<int32_t>(x), static_cast<int32_t>(y)});
      auto provider = rootView->UiaProviderFromPoint(
          {static_cast<LONG>(local.X * m_island.RasterizationScale()),
           static_cast<LONG>(local.Y * m_island.RasterizationScale())});
      auto spFragment = provider.try_as<IRawElementProviderFragment>();
      if (spFragment) {
        *pRetVal = spFragment.detach();
      }

      return S_OK;
    }
#endif

    if (m_hwnd) {
      if (!IsWindow(m_hwnd)) {
        // TODO: Add support for non-HWND based hosting
        assert(false);
        return E_FAIL;
      }

      POINT clientPoint{static_cast<LONG>(x), static_cast<LONG>(y)};
      ScreenToClient(m_hwnd, &clientPoint);

      auto provider = rootView->UiaProviderFromPoint(clientPoint);
      auto spFragment = provider.try_as<IRawElementProviderFragment>();
      if (spFragment) {
        *pRetVal = spFragment.detach();
        return S_OK;
      }
    }
  }

  AddRef();
  *pRetVal = this;
  return S_OK;
}

HRESULT __stdcall CompositionRootAutomationProvider::GetFocus(IRawElementProviderFragment **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  auto rootView = rootComponentView();

  if (rootView == nullptr) {
    return UIA_E_ELEMENTNOTAVAILABLE;
  }

  const auto &focusedComponent = rootView->GetFocusedComponent();

  if (focusedComponent) {
    auto focusedUiaProvider =
        winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(focusedComponent)
            ->EnsureUiaProvider();
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

#ifdef USE_WINUI3
void CompositionRootAutomationProvider::SetIsland(winrt::Microsoft::UI::Content::ContentIsland &island) noexcept {
  m_island = island;
}
#endif

HRESULT __stdcall CompositionRootAutomationProvider::Navigate(
    NavigateDirection direction,
    IRawElementProviderFragment **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  // Fragment roots do not enable navigation to a parent or siblings; navigation among fragment roots is handled by the
  // default window providers. Elements in fragments must navigate only to other elements within that fragment.
  if (direction == NavigateDirection_FirstChild || direction == NavigateDirection_LastChild) {
    if (auto rootView = rootComponentView()) {
      auto uiaProvider = rootView->EnsureUiaProvider();
      auto spFragment = uiaProvider.try_as<IRawElementProviderFragment>();
      if (spFragment) {
        *pRetVal = spFragment.detach();
        return S_OK;
      }
    }
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
