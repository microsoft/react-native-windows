#include "pch.h"
#include "CompositionDynamicAutomationProvider.h"
#include <Fabric/ComponentView.h>
#pragma warning(push)
#pragma warning(disable : 4229)
#define IN
#define OUT
#include <atlsafe.h>
#pragma warning(pop)
#include "RootComponentView.h"
#include "UiaHelpers.h"

namespace winrt::Microsoft::ReactNative::implementation {

CompositionDynamicAutomationProvider::CompositionDynamicAutomationProvider(
    const std::shared_ptr<::Microsoft::ReactNative::CompositionBaseComponentView> &componentView) noexcept
    : m_view{std::static_pointer_cast<::Microsoft::ReactNative::IComponentView>(componentView)} {}

HRESULT __stdcall CompositionDynamicAutomationProvider::Navigate(
    NavigateDirection direction,
    IRawElementProviderFragment **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  return UiaNavigateHelper(m_view, direction, *pRetVal);
}

// Implementations should return NULL for a top-level element that is hosted in a window. Other elements should return
// an array that contains UiaAppendRuntimeId (defined in Uiautomationcoreapi.h), followed by a value that is unique
// within an instance of the fragment.
//
// We'll use the react tag as our identifier for those situations
HRESULT __stdcall CompositionDynamicAutomationProvider::GetRuntimeId(SAFEARRAY **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  CComSafeArray<int32_t> runtimeId;
  auto hr = runtimeId.Create(2);

  if (FAILED(hr))
    return hr;

  runtimeId[0] = UiaAppendRuntimeId;
  runtimeId[1] = strongView->tag();

  *pRetVal = runtimeId.Detach();

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

  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto rootCV = strongView->rootComponentView();
  if (rootCV == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto uiaProvider = rootCV->EnsureUiaProvider();
  if (uiaProvider != nullptr) {
    winrt::com_ptr<IRawElementProviderFragmentRoot> spReps;
    uiaProvider.as(spReps);
    *pRetVal = spReps.detach();
  }

  return S_OK;
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

long GetControlType(const std::string &role) noexcept {
  if (role == "adjustable") {
    return UIA_SliderControlTypeId;
  } else if (role == "group" || role == "search" || role == "radiogroup" || role == "timer" || role.empty()) {
    return UIA_GroupControlTypeId;
  } else if (role == "button" || role == "imagebutton" || role == "switch" || role == "togglebutton") {
    return UIA_ButtonControlTypeId;
  } else if (role == "checkbox") {
    return UIA_CheckBoxControlTypeId;
  } else if (role == "combobox") {
    return UIA_ComboBoxControlTypeId;
  } else if (role == "alert" || role == "header" || role == "summary" || role == "text") {
    return UIA_TextControlTypeId;
  } else if (role == "image") {
    return UIA_ImageControlTypeId;
  } else if (role == "keyboardkey") {
    return UIA_CustomControlTypeId;
  } else if (role == "link") {
    return UIA_HyperlinkControlTypeId;
  }
  // list and listitem were added by RNW to better support UIA Control Types
  else if (role == "list") {
    return UIA_ListControlTypeId;
  } else if (role == "listitem") {
    return UIA_ListItemControlTypeId;
  } else if (role == "menu") {
    return UIA_MenuControlTypeId;
  } else if (role == "menubar") {
    return UIA_MenuBarControlTypeId;
  } else if (role == "menuitem") {
    return UIA_MenuItemControlTypeId;
  }
  // If role is "none", remove the element from the control tree
  // and expose it as a plain element would in the raw tree.
  else if (role == "none") {
    return UIA_GroupControlTypeId;
  } else if (role == "progressbar") {
    return UIA_ProgressBarControlTypeId;
  } else if (role == "radio") {
    return UIA_RadioButtonControlTypeId;
  } else if (role == "scrollbar") {
    return UIA_ScrollBarControlTypeId;
  } else if (role == "spinbutton") {
    return UIA_SpinnerControlTypeId;
  } else if (role == "splitbutton") {
    return UIA_SplitButtonControlTypeId;
  } else if (role == "tab") {
    return UIA_TabItemControlTypeId;
  } else if (role == "tablist") {
    return UIA_TabControlTypeId;
  } else if (role == "toolbar") {
    return UIA_ToolBarControlTypeId;
  } else if (role == "tree") {
    return UIA_TreeControlTypeId;
  } else if (role == "treeitem") {
    return UIA_TreeItemControlTypeId;
  }
  assert(false);
  return UIA_GroupControlTypeId;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::GetPropertyValue(PROPERTYID propertyId, VARIANT *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  auto strongView = m_view.view();
  if (strongView == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(strongView->props());
  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  switch (propertyId) {
    case UIA_ControlTypePropertyId: {
      pRetVal->vt = VT_I4;
      auto role = props->accessibilityRole;
      pRetVal->lVal = GetControlType(role);
      break;
    }
    case UIA_AutomationIdPropertyId: {
      pRetVal->vt = VT_BSTR;
      auto testId = props->testId;
      CComBSTR temp(testId.c_str());
      pRetVal->bstrVal = temp.Detach();
      break;
    }
    case UIA_NamePropertyId: {
      pRetVal->vt = VT_BSTR;
      auto name = props->accessibilityLabel;
      CComBSTR temp(name.c_str());
      pRetVal->bstrVal = temp.Detach();
      break;
    }
  }

  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_HostRawElementProvider(
    IRawElementProviderSimple **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  return S_OK;
}

} // namespace winrt::Microsoft::ReactNative::implementation
