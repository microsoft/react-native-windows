#include "pch.h"
#include "CompositionRootAutomationProvider.h"

#include "Fabric/Composition/CompositionRootView.h"
#pragma warning(push)
#pragma warning(disable : 4229)
#define IN
#define OUT
#include <atlsafe.h>
#pragma warning(pop)
#include <intsafe.h>

namespace winrt::Microsoft::ReactNative::implementation {

  CompositionRootAutomationProvider::CompositionRootAutomationProvider(
    winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::CompositionRootView>&& weakRootControl,
    HWND hwnd) noexcept
    : m_weakRootControl{ std::move(weakRootControl) }, m_hwnd{ hwnd } {}

  // Implementations should return NULL for a top-level element that is hosted in a window. Other elements should return
  // an array that contains UiaAppendRuntimeId (defined in Uiautomationcoreapi.h), followed by a value that is unique
  // within an instance of the fragment.
  //
  // We'll use the View pointer as our identifier
  HRESULT __stdcall CompositionRootAutomationProvider::GetRuntimeId(SAFEARRAY** pRetVal) {
    if (pRetVal == nullptr)
      return E_POINTER;

    *pRetVal = nullptr;

    //if (!m_weakRootControl)
    //  return UIA_E_ELEMENTNOTAVAILABLE;

    //CComSafeArray<int32_t> runtimeId;
    //auto hr = runtimeId.Create(3);

    //if (FAILED(hr))
    //  return hr;

    //auto pointer = reinterpret_cast<INT_PTR>(m_weakRootControl.get().get());

    //runtimeId[0] = UiaAppendRuntimeId;
    //runtimeId[1] = LODWORD(pointer);
    //runtimeId[2] = HIDWORD(pointer);

    //*pRetVal = runtimeId.Detach();

    return S_OK;
  }

  HRESULT __stdcall CompositionRootAutomationProvider::GetEmbeddedFragmentRoots(SAFEARRAY** pRetVal) {
    if (pRetVal == nullptr)
      return E_POINTER;

    *pRetVal = nullptr;

    return S_OK;
  }

  HRESULT __stdcall CompositionRootAutomationProvider::SetFocus(void) {
    // TODO: 
    return S_OK;
  }

  HRESULT __stdcall CompositionRootAutomationProvider::GetPatternProvider(PATTERNID patternId, IUnknown** pRetVal) {
    if (pRetVal == nullptr)
      return E_POINTER;

    // TODO:
    *pRetVal = nullptr;

    return S_OK;
  }

  /*
  GetPropValue 30016 ControlType
  GetPropValue 30024 FrameworkId
  GetPropValue 30002 ProcessId
  GetPropValue 30005 Name
  GetPropValue 30004 LocalizedControlType
  GetPropValue 30101 AriaRole
  GetPropValue 30003 ControlType
  GetPropValue 30003
  winrt::Microsoft::ReactNative::implementation::CompositionRootAutomationProvider::get_BoundingRectangle(UiaRect *)
  GetPropValue 30002
  */
  HRESULT __stdcall CompositionRootAutomationProvider::GetPropertyValue(PROPERTYID propertyId, VARIANT* pRetVal) {
    if (pRetVal == nullptr)
      return E_POINTER;

    switch (propertyId) {
    case UIA_ControlTypePropertyId: {
      pRetVal->vt = VT_I4;
      pRetVal->lVal = UIA_PaneControlTypeId;
      break;
    }
    case UIA_NamePropertyId: {
      pRetVal->bstrVal = SysAllocString(L"Test Name");
      if (pRetVal->bstrVal != nullptr) {
        pRetVal->vt = VT_BSTR;
      }
      break;
    }
    default:
      // TODO: remove
      if (pRetVal) {
        pRetVal = pRetVal;
      }
      break;
    }
    return S_OK;
  }

  HRESULT __stdcall CompositionRootAutomationProvider::get_HostRawElementProvider(IRawElementProviderSimple** pRetVal) {
    if (pRetVal == nullptr)
      return E_POINTER;

    if (!IsWindow(m_hwnd))
      return UIA_E_ELEMENTNOTAVAILABLE;

    auto hr = UiaHostProviderFromHwnd(m_hwnd, pRetVal);

    return S_OK;
  }

  HRESULT __stdcall CompositionRootAutomationProvider::get_BoundingRectangle(UiaRect* pRetVal) {
    if (pRetVal == nullptr)
      return E_POINTER;

    auto strongRoot = m_weakRootControl.get();
    if (strongRoot == nullptr)
      return UIA_E_ELEMENTNOTAVAILABLE;

    auto visual = strongRoot->RootVisual();
    if (visual == nullptr)
      return E_FAIL;

    winrt::Windows::Foundation::Numerics::float3 visualOffset;
    visual.Offset(visualOffset);
    pRetVal->left = visualOffset.x;
    pRetVal->top = visualOffset.y;

    winrt::Windows::Foundation::Numerics::float2 visualSize;
    visual.Size(visualSize);
    pRetVal->width = visualSize.x;
    pRetVal->height = visualSize.y;

    return S_OK;
  }

  HRESULT __stdcall CompositionRootAutomationProvider::get_FragmentRoot(IRawElementProviderFragmentRoot** pRetVal) {
    if (pRetVal == nullptr)
      return E_POINTER;

    AddRef();
    *pRetVal = this;

    return S_OK;
  }

  HRESULT __stdcall CompositionRootAutomationProvider::get_ProviderOptions(ProviderOptions* pRetVal) {
    if (pRetVal == nullptr)
      return E_POINTER;

    *pRetVal = ProviderOptions_ServerSideProvider | ProviderOptions_UseComThreading;
    return S_OK;
  }

  HRESULT __stdcall CompositionRootAutomationProvider::ElementProviderFromPoint(
    double x,
    double y,
    IRawElementProviderFragment** pRetVal) {
    if (pRetVal == nullptr)
      return E_POINTER;

    *pRetVal = nullptr;

    return S_OK;
  }

  HRESULT __stdcall CompositionRootAutomationProvider::GetFocus(IRawElementProviderFragment** pRetVal) {
    if (pRetVal == nullptr)
      return E_POINTER;

    *pRetVal = nullptr;

    return S_OK;
  }

  HRESULT __stdcall CompositionRootAutomationProvider::Navigate(
    NavigateDirection direction,
    IRawElementProviderFragment** pRetVal) {
    if (pRetVal == nullptr)
      return E_POINTER;

    *pRetVal = nullptr;

    return S_OK;
  }

} // namespace winrt::Microsoft::ReactNative::implementation
