#include "pch.h"
#include "CompositionDynamicAutomationProvider.h"
#include <Fabric/ComponentView.h>
#include <Unicode.h>
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

  *pRetVal = SafeArrayCreateVector(VT_I4, 0, 2);

  if (*pRetVal == nullptr)
    return E_OUTOFMEMORY;

  int runtimeId[] = {UiaAppendRuntimeId, strongView->tag()};
  for (long i = 0; i < 2; i++) {
    SafeArrayPutElement(*pRetVal, &i, static_cast<void *>(&runtimeId[i]));
  }

  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_BoundingRectangle(UiaRect *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  auto hr = UiaGetBoundingRectangleHelper(m_view, *pRetVal);
  if (FAILED(hr))
    return hr;

  // Since get_BoundingRectangle needs to provide real screen coordinates back to the UIA client
  // we'll use the FragmentRoot's origin to offset our rect because that should have been taken
  // into account already.
  winrt::com_ptr<IRawElementProviderFragmentRoot> spFragmentRoot = nullptr;
  hr = get_FragmentRoot(spFragmentRoot.put());
  if (FAILED(hr))
    return hr;

  auto spFragment = spFragmentRoot.try_as<IRawElementProviderFragment>();
  if (spFragment == nullptr)
    return E_FAIL;

  UiaRect rect;
  hr = spFragment->get_BoundingRectangle(&rect);
  if (FAILED(hr))
    return hr;

  pRetVal->left += rect.left;
  pRetVal->top += rect.top;

  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::GetEmbeddedFragmentRoots(SAFEARRAY **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::SetFocus(void) {
  return UiaSetFocusHelper(m_view);
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
  auto spFragmentRoot = uiaProvider.try_as<IRawElementProviderFragmentRoot>();
  if (spFragmentRoot) {
    *pRetVal = spFragmentRoot.detach();
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

  auto strongView = m_view.view();
  if (strongView == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(strongView->props());
  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;
  auto accessibilityRole = props->accessibilityRole;
  // Invoke control pattern is used to support controls that do not maintain state
  // when activated but rather initiate or perform a single, unambiguous action.
  if (patternId == UIA_InvokePatternId &&
      (accessibilityRole == "button" || accessibilityRole == "imagebutton" || accessibilityRole == "link" ||
       accessibilityRole == "splitbutton" || (accessibilityRole == "menuitem" && props->onAccessibilityTap) ||
       (accessibilityRole == "treeitem" && props->onAccessibilityTap))) {
    *pRetVal = static_cast<IInvokeProvider *>(this);
    AddRef();
  }

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
  } else if (role == "textinput" || role == "searchbox") {
    return UIA_EditControlTypeId;
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

  auto baseView = std::static_pointer_cast<::Microsoft::ReactNative::CompositionBaseComponentView>(strongView);
  if (baseView == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto hr = S_OK;

  switch (propertyId) {
    case UIA_ControlTypePropertyId: {
      pRetVal->vt = VT_I4;
      auto role = props->accessibilityRole.empty() ? baseView->DefaultControlType() : props->accessibilityRole;
      pRetVal->lVal = GetControlType(role);
      break;
    }
    case UIA_AutomationIdPropertyId: {
      pRetVal->vt = VT_BSTR;
      auto wideTestId = ::Microsoft::Common::Unicode::Utf8ToUtf16(props->testId);
      pRetVal->bstrVal = SysAllocString(wideTestId.c_str());
      hr = pRetVal->bstrVal != nullptr ? S_OK : E_OUTOFMEMORY;
      break;
    }
    case UIA_NamePropertyId: {
      pRetVal->vt = VT_BSTR;
      auto wideName = ::Microsoft::Common::Unicode::Utf8ToUtf16(
          props->accessibilityLabel.empty() ? baseView->DefaultAccessibleName() : props->accessibilityLabel);
      pRetVal->bstrVal = SysAllocString(wideName.c_str());
      hr = pRetVal->bstrVal != nullptr ? S_OK : E_OUTOFMEMORY;
      break;
    }
    case UIA_IsKeyboardFocusablePropertyId: {
      pRetVal->vt = VT_BOOL;
      pRetVal->boolVal = props->focusable ? VARIANT_TRUE : VARIANT_FALSE;
      break;
    }
    case UIA_HasKeyboardFocusPropertyId: {
      auto rootCV = strongView->rootComponentView();
      if (rootCV == nullptr)
        return UIA_E_ELEMENTNOTAVAILABLE;

      pRetVal->vt = VT_BOOL;
      pRetVal->boolVal = rootCV->GetFocusedComponent() == strongView.get() ? VARIANT_TRUE : VARIANT_FALSE;
      break;
    }
    case UIA_IsEnabledPropertyId: {
      pRetVal->vt = VT_BOOL;
      pRetVal->boolVal = !props->accessibilityState.disabled ? VARIANT_TRUE : VARIANT_FALSE;
      break;
    }
    case UIA_IsContentElementPropertyId: {
      pRetVal->vt = VT_BOOL;
      pRetVal->boolVal = (props->accessible && props->accessibilityRole != "none") ? VARIANT_TRUE : VARIANT_FALSE;
      break;
    }
    case UIA_IsControlElementPropertyId: {
      pRetVal->vt = VT_BOOL;
      pRetVal->boolVal = (props->accessible && props->accessibilityRole != "none") ? VARIANT_TRUE : VARIANT_FALSE;
      break;
    }

    case UIA_HelpTextPropertyId: {
      pRetVal->vt = VT_BSTR;
      auto helpText = props->accessibilityHint.empty()
          ? ::Microsoft::Common::Unicode::Utf8ToUtf16(baseView->DefaultHelpText())
          : ::Microsoft::Common::Unicode::Utf8ToUtf16(props->accessibilityHint);
      pRetVal->bstrVal = SysAllocString(helpText.c_str());
      hr = pRetVal->bstrVal != nullptr ? S_OK : E_OUTOFMEMORY;
      break;
    }
  }

  return hr;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_HostRawElementProvider(
    IRawElementProviderSimple **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::Invoke() {
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto baseView = std::static_pointer_cast<::Microsoft::ReactNative::CompositionBaseComponentView>(strongView);
  if (baseView == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  baseView.get()->GetEventEmitter().get()->onAccessibilityTap();
  auto uiaProvider = baseView->EnsureUiaProvider();
  auto spProviderSimple = uiaProvider.try_as<IRawElementProviderSimple>();
  if (spProviderSimple != nullptr) {
    UiaRaiseAutomationEvent(spProviderSimple.get(), UIA_Invoke_InvokedEventId);
  }

  return S_OK;
}

} // namespace winrt::Microsoft::ReactNative::implementation
