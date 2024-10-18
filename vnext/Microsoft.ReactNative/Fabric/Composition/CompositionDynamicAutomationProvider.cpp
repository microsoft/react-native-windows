#include "pch.h"
#include "CompositionDynamicAutomationProvider.h"
#include <Fabric/ComponentView.h>
#include <Fabric/Composition/SwitchComponentView.h>
#include <Fabric/Composition/TextInput/WindowsTextInputComponentView.h>
#include <Unicode.h>
#include "RootComponentView.h"
#include "UiaHelpers.h"

namespace winrt::Microsoft::ReactNative::implementation {

CompositionDynamicAutomationProvider::CompositionDynamicAutomationProvider(
    const winrt::Microsoft::ReactNative::Composition::ComponentView &componentView) noexcept
    : m_view{componentView} {}

HRESULT __stdcall CompositionDynamicAutomationProvider::Navigate(
    NavigateDirection direction,
    IRawElementProviderFragment **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  return UiaNavigateHelper(m_view.view(), direction, *pRetVal);
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

  int runtimeId[] = {UiaAppendRuntimeId, strongView.Tag()};
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
  if (FAILED(hr)) {
    assert(false);
    return hr;
  }

  auto spFragment = spFragmentRoot.try_as<IRawElementProviderFragment>();
  if (spFragment == nullptr) {
    assert(false);
    return E_FAIL;
  }

  UiaRect rect;
  hr = spFragment->get_BoundingRectangle(&rect);
  if (FAILED(hr)) {
    assert(false);
    return hr;
  }

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

  if (!strongView) {
    return UIA_E_ELEMENTNOTAVAILABLE;
  }

  if (auto root = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)
                      ->rootComponentView()) {
    return root->GetFragmentRoot(pRetVal);
  }

  return UIA_E_ELEMENTNOTAVAILABLE;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_ProviderOptions(ProviderOptions *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = ProviderOptions_ServerSideProvider | ProviderOptions_UseComThreading;
  return S_OK;
}

bool accessibilityValueHasValue(const facebook::react::AccessibilityValue &value) {
  return (value.min.has_value() && value.max.has_value()) || value.now.has_value() || value.text.has_value();
}

bool expandableControl(const facebook::react::SharedViewProps props) {
  if (props->accessibilityState.has_value() && props->accessibilityState->expanded.has_value())
    return true;
  auto accessibilityActions = props->accessibilityActions;
  for (size_t i = 0; i < accessibilityActions.size(); i++) {
    if (accessibilityActions[i].name == "expand" || accessibilityActions[i].name == "collapse") {
      return true;
    }
  }
  return false;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::GetPatternProvider(PATTERNID patternId, IUnknown **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  auto strongView = m_view.view();
  if (strongView == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto compositionView = strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>();
  if (compositionView == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;
  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->props());
  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;
  auto accessibilityRole =
      props->accessibilityRole.empty() ? compositionView->DefaultControlType() : props->accessibilityRole;
  // Invoke control pattern is used to support controls that do not maintain state
  // when activated but rather initiate or perform a single, unambiguous action.
  if (patternId == UIA_InvokePatternId && (props->onAccessibilityTap)) {
    *pRetVal = static_cast<IInvokeProvider *>(this);
    AddRef();
  }

  if (patternId == UIA_ScrollItemPatternId) {
    *pRetVal = static_cast<IScrollItemProvider *>(this);
    AddRef();
  }

  if (patternId == UIA_ValuePatternId &&
      (strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::WindowsTextInputComponentView>() ||
       accessibilityValueHasValue(props->accessibilityValue))) {
    *pRetVal = static_cast<IValueProvider *>(this);
    AddRef();
  }

  if (patternId == UIA_TogglePatternId &&
      strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::SwitchComponentView>()) {
    *pRetVal = static_cast<IToggleProvider *>(this);
    AddRef();
  }

  if (patternId == UIA_ExpandCollapsePatternId && expandableControl(props)) {
    *pRetVal = static_cast<IExpandCollapseProvider *>(this);
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

  auto baseView = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView);
  if (baseView == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(baseView->props());
  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto compositionView = strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>();
  if (compositionView == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto hr = S_OK;

  switch (propertyId) {
    case UIA_ControlTypePropertyId: {
      pRetVal->vt = VT_I4;
      auto role = props->accessibilityRole.empty() ? compositionView->DefaultControlType() : props->accessibilityRole;
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
          props->accessibilityLabel.empty() ? compositionView->DefaultAccessibleName() : props->accessibilityLabel);
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
      auto rootCV = baseView->rootComponentView();
      if (rootCV == nullptr)
        return UIA_E_ELEMENTNOTAVAILABLE;

      pRetVal->vt = VT_BOOL;
      pRetVal->boolVal = rootCV->GetFocusedComponent() == strongView ? VARIANT_TRUE : VARIANT_FALSE;
      break;
    }
    case UIA_IsEnabledPropertyId: {
      pRetVal->vt = VT_BOOL;
      pRetVal->boolVal =
          !(props->accessibilityState && props->accessibilityState->disabled) ? VARIANT_TRUE : VARIANT_FALSE;
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
    case UIA_IsOffscreenPropertyId: {
      pRetVal->vt = VT_BOOL;
      pRetVal->boolVal = (compositionView->getClipState() == ClipState::FullyClipped) ? VARIANT_TRUE : VARIANT_FALSE;
    }
    case UIA_HelpTextPropertyId: {
      pRetVal->vt = VT_BSTR;
      auto helpText = props->accessibilityHint.empty()
          ? ::Microsoft::Common::Unicode::Utf8ToUtf16(compositionView->DefaultHelpText())
          : ::Microsoft::Common::Unicode::Utf8ToUtf16(props->accessibilityHint);
      pRetVal->bstrVal = SysAllocString(helpText.c_str());
      hr = pRetVal->bstrVal != nullptr ? S_OK : E_OUTOFMEMORY;
      break;
    }
    case UIA_PositionInSetPropertyId: {
      pRetVal->vt = VT_I4;
      pRetVal->lVal = props->accessibilityPosInSet;
      break;
    }
    case UIA_SizeOfSetPropertyId: {
      pRetVal->vt = VT_I4;
      pRetVal->lVal = props->accessibilitySetSize;
      break;
    }
    case UIA_LiveSettingPropertyId: {
      pRetVal->vt = VT_I4;
      pRetVal->lVal = GetLiveSetting(props->accessibilityLiveRegion);
      break;
    }
    case UIA_ItemStatusPropertyId: {
      pRetVal->vt = VT_BSTR;
      pRetVal->bstrVal = (props->accessibilityState.has_value() && props->accessibilityState->busy)
          ? SysAllocString(L"Busy")
          : SysAllocString(L"");
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

  auto baseView = strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>();
  if (baseView == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  baseView->GetEventEmitter().get()->onAccessibilityTap();
  auto uiaProvider =
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->EnsureUiaProvider();
  auto spProviderSimple = uiaProvider.try_as<IRawElementProviderSimple>();
  if (spProviderSimple != nullptr) {
    UiaRaiseAutomationEvent(spProviderSimple.get(), UIA_Invoke_InvokedEventId);
  }
  DispatchAccessibilityAction(m_view, "invoke");
  DispatchAccessibilityAction(m_view, "activate");

  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::ScrollIntoView() {
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  winrt::Microsoft::ReactNative::implementation::BringIntoViewOptions scrollOptions;
  winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)
      ->StartBringIntoView(std::move(scrollOptions));
  DispatchAccessibilityAction(m_view, "scrollIntoView");

  return S_OK;
}

BSTR StringToBSTR(const std::string &str) {
  // Calculate the required BSTR size in bytes
  int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
  if (len == 0) {
    return nullptr; // Conversion error
  }

  // Allocate memory for the BSTR
  BSTR bstr = SysAllocStringLen(nullptr, len - 1); // len includes the null terminator

  // Convert the std::string to BSTR
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, bstr, len);

  return bstr;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::SetValue(LPCWSTR val) {
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)
      ->setAcccessiblityValue(winrt::to_string(val));
  DispatchAccessibilityAction(m_view, "setValue");
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_Value(BSTR *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  *pRetVal = StringToBSTR(winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)
                              ->getAccessiblityValue()
                              .value_or(""));
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_IsReadOnly(BOOL *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->props());
  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;
  auto accessibilityRole = props->accessibilityRole;
  if (accessibilityRole.empty()) {
    // Control is using default control type. Use default IsReadOnly value.
    *pRetVal = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)
                   ->getAcccessiblityIsReadOnly();
  } else if (
      accessibilityRole == "textinput" || accessibilityRole == "searchbox" || accessibilityRole == "adjustable" ||
      accessibilityRole == "spinbutton" || accessibilityRole == "combobox") {
    // Control is using customized control type which should not be IsReadOnly for value pattern.
    *pRetVal = false;
  } else {
    // Control is using customized control type which should be IsReadOnly for value pattern.
    *pRetVal = true;
  }
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_ToggleState(ToggleState *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  *pRetVal =
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->getToggleState();
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::Toggle() {
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->Toggle();
  DispatchAccessibilityAction(m_view, "toggle");
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_ExpandCollapseState(ExpandCollapseState *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->props());

  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  *pRetVal = props->accessibilityState->expanded.has_value()
      ? GetExpandCollapseState(props->accessibilityState->expanded.value())
      : ExpandCollapseState_Collapsed;
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::Expand() {
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;
  DispatchAccessibilityAction(m_view, "expand");
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::Collapse() {
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;
  DispatchAccessibilityAction(m_view, "collapse");
  return S_OK;
}

} // namespace winrt::Microsoft::ReactNative::implementation
