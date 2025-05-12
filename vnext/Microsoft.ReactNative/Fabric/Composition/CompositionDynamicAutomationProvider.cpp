#include "pch.h"
#include "CompositionDynamicAutomationProvider.h"
#include <Fabric/ComponentView.h>
#include <Fabric/Composition/CompositionAnnotationProvider.h>
#include <Fabric/Composition/CompositionTextRangeProvider.h>
#include <Fabric/Composition/ParagraphComponentView.h>
#include <Fabric/Composition/ScrollViewComponentView.h>
#include <Fabric/Composition/SwitchComponentView.h>
#include <Fabric/Composition/TextInput/WindowsTextInputComponentView.h>
#include <Unicode.h>
#include <winrt/Microsoft.UI.Content.h>
#include "RootComponentView.h"
#include "UiaHelpers.h"

namespace winrt::Microsoft::ReactNative::implementation {

CompositionDynamicAutomationProvider::CompositionDynamicAutomationProvider(
    const winrt::Microsoft::ReactNative::Composition::ComponentView &componentView) noexcept
    : m_view{componentView} {
  auto strongView = m_view.view();

  if (!strongView)
    return;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->props());
  if (!props)
    return;

  if (props->accessibilityState.has_value() && props->accessibilityState->selected.has_value()) {
    AddSelectionItemsToContainer(this);
  }

  if (strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::WindowsTextInputComponentView>() ||
      strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ParagraphComponentView>()) {
    m_textProvider = winrt::make<CompositionTextProvider>(
                         strongView.as<winrt::Microsoft::ReactNative::Composition::ComponentView>(), this)
                         .try_as<ITextProvider2>();
  }

  if (strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ViewComponentView>()) {
    m_annotationProvider = winrt::make<CompositionAnnotationProvider>(
                               strongView.as<winrt::Microsoft::ReactNative::Composition::ComponentView>(), this)
                               .try_as<IAnnotationProvider>();
  }
}

CompositionDynamicAutomationProvider::CompositionDynamicAutomationProvider(
    const winrt::Microsoft::ReactNative::Composition::ComponentView &componentView,
    const winrt::Microsoft::UI::Content::ChildSiteLink &childSiteLink) noexcept
    : m_view{componentView}, m_childSiteLink{childSiteLink} {}

HRESULT __stdcall CompositionDynamicAutomationProvider::Navigate(
    NavigateDirection direction,
    IRawElementProviderFragment **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  if (m_childSiteLink) {
    if (direction == NavigateDirection_FirstChild || direction == NavigateDirection_LastChild) {
      auto fragment = m_childSiteLink.AutomationProvider().try_as<IRawElementProviderFragment>();
      *pRetVal = fragment.detach();
      return S_OK;
    }
  }

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

bool accessibilityValueHasTextValue(const facebook::react::AccessibilityValue &value) {
  return value.text.has_value();
}

bool accessibilityValueHasNumericValue(const facebook::react::AccessibilityValue &value) {
  return (value.min.has_value() && value.max.has_value() && value.now.has_value());
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

bool togglableControl(const facebook::react::SharedViewProps props) {
  if (props->accessibilityState.has_value() &&
      props->accessibilityState->checked != facebook::react::AccessibilityState::None) {
    return true;
  }
  auto accessibilityActions = props->accessibilityActions;
  for (size_t i = 0; i < accessibilityActions.size(); i++) {
    if (accessibilityActions[i].name == "toggle") {
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

  if (patternId == UIA_ScrollPatternId &&
      strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ScrollViewComponentView>()) {
    *pRetVal = static_cast<IScrollProvider *>(this);
    AddRef();
  }

  if (patternId == UIA_ValuePatternId &&
      ((strongView
            .try_as<winrt::Microsoft::ReactNative::Composition::implementation::WindowsTextInputComponentView>() &&
        !accessibilityValueHasNumericValue(props->accessibilityValue)) ||
       accessibilityValueHasTextValue(props->accessibilityValue))) {
    *pRetVal = static_cast<IValueProvider *>(this);
    AddRef();
  }

  if (patternId == UIA_RangeValuePatternId && accessibilityValueHasNumericValue(props->accessibilityValue)) {
    *pRetVal = static_cast<IValueProvider *>(this);
    AddRef();
  }

  if (patternId == UIA_TogglePatternId &&
      (strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::SwitchComponentView>() ||
       togglableControl(props))) {
    *pRetVal = static_cast<IToggleProvider *>(this);
    AddRef();
  }

  if (patternId == UIA_ExpandCollapsePatternId && expandableControl(props)) {
    *pRetVal = static_cast<IExpandCollapseProvider *>(this);
    AddRef();
  }

  if (patternId == UIA_SelectionPatternId && props->accessibilityState.has_value() &&
      props->accessibilityState->multiselectable.has_value() && props->accessibilityState->required.has_value()) {
    *pRetVal = static_cast<ISelectionProvider *>(this);
    AddRef();
  }

  if (patternId == UIA_SelectionItemPatternId && props->accessibilityState.has_value() &&
      props->accessibilityState->selected.has_value()) {
    *pRetVal = static_cast<ISelectionItemProvider *>(this);
    AddRef();
  }

  if (patternId == UIA_TextPatternId &&
      (strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::WindowsTextInputComponentView>() ||
       strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ParagraphComponentView>())) {
    m_textProvider.as<IUnknown>().copy_to(pRetVal);
  }

  if (patternId == UIA_TextPattern2Id &&
      strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::WindowsTextInputComponentView>()) {
    m_textProvider.as<IUnknown>().copy_to(pRetVal);
  }
  if (patternId == UIA_AnnotationPatternId &&
      strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ViewComponentView>() &&
      accessibilityAnnotationHasValue(props->accessibilityAnnotation)) {
    m_annotationProvider.as<IUnknown>().copy_to(pRetVal);
  }

  return S_OK;
}

long GetControlTypeFromString(const std::string &role) noexcept {
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
  } else if (role == "pane") {
    return UIA_PaneControlTypeId;
  }
  assert(false);
  return UIA_GroupControlTypeId;
}

long GetControlTypeFromRole(const facebook::react::Role &role) noexcept {
  switch (role) {
    case facebook::react::Role::Alert:
      return UIA_TextControlTypeId;
    case facebook::react::Role::Application:
      return UIA_WindowControlTypeId;
    case facebook::react::Role::Button:
      return UIA_ButtonControlTypeId;
    case facebook::react::Role::Checkbox:
      return UIA_CheckBoxControlTypeId;
    case facebook::react::Role::Columnheader:
      return UIA_HeaderControlTypeId;
    case facebook::react::Role::Combobox:
      return UIA_ComboBoxControlTypeId;
    case facebook::react::Role::Document:
      return UIA_DocumentControlTypeId;
    case facebook::react::Role::Grid:
      return UIA_GroupControlTypeId;
    case facebook::react::Role::Group:
      return UIA_GroupControlTypeId;
    case facebook::react::Role::Heading:
      return UIA_TextControlTypeId;
    case facebook::react::Role::Img:
      return UIA_ImageControlTypeId;
    case facebook::react::Role::Link:
      return UIA_HyperlinkControlTypeId;
    case facebook::react::Role::List:
      return UIA_ListControlTypeId;
    case facebook::react::Role::Listitem:
      return UIA_ListItemControlTypeId;
    case facebook::react::Role::Menu:
      return UIA_MenuControlTypeId;
    case facebook::react::Role::Menubar:
      return UIA_MenuBarControlTypeId;
    case facebook::react::Role::Menuitem:
      return UIA_MenuItemControlTypeId;
    case facebook::react::Role::None:
      return UIA_GroupControlTypeId;
    case facebook::react::Role::Presentation:
      return UIA_GroupControlTypeId;
    case facebook::react::Role::Progressbar:
      return UIA_ProgressBarControlTypeId;
    case facebook::react::Role::Radio:
      return UIA_RadioButtonControlTypeId;
    case facebook::react::Role::Radiogroup:
      return UIA_GroupControlTypeId;
    case facebook::react::Role::Rowgroup:
      return UIA_GroupControlTypeId;
    case facebook::react::Role::Rowheader:
      return UIA_HeaderControlTypeId;
    case facebook::react::Role::Scrollbar:
      return UIA_ScrollBarControlTypeId;
    case facebook::react::Role::Searchbox:
      return UIA_EditControlTypeId;
    case facebook::react::Role::Separator:
      return UIA_SeparatorControlTypeId;
    case facebook::react::Role::Slider:
      return UIA_SliderControlTypeId;
    case facebook::react::Role::Spinbutton:
      return UIA_SpinnerControlTypeId;
    case facebook::react::Role::Status:
      return UIA_StatusBarControlTypeId;
    case facebook::react::Role::Summary:
      return UIA_GroupControlTypeId;
    case facebook::react::Role::Switch:
      return UIA_ButtonControlTypeId;
    case facebook::react::Role::Tab:
      return UIA_TabItemControlTypeId;
    case facebook::react::Role::Table:
      return UIA_TableControlTypeId;
    case facebook::react::Role::Tablist:
      return UIA_TabControlTypeId;
    case facebook::react::Role::Tabpanel:
      return UIA_TabControlTypeId;
    case facebook::react::Role::Timer:
      return UIA_ButtonControlTypeId;
    case facebook::react::Role::Toolbar:
      return UIA_ToolBarControlTypeId;
    case facebook::react::Role::Tooltip:
      return UIA_ToolTipControlTypeId;
    case facebook::react::Role::Tree:
      return UIA_TreeControlTypeId;
    case facebook::react::Role::Treegrid:
      return UIA_TreeControlTypeId;
    case facebook::react::Role::Treeitem:
      return UIA_TreeItemControlTypeId;
  }
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
      pRetVal->lVal = props->role == facebook::react::Role::None ? props->accessibilityRole.empty()
              ? GetControlTypeFromString(compositionView->DefaultControlType())
              : GetControlTypeFromString(props->accessibilityRole)
                                                                 : GetControlTypeFromRole(props->role);
      break;
    }
    case UIA_AutomationIdPropertyId: {
      pRetVal->vt = VT_BSTR;
      auto id = !props->nativeId.empty() ? ::Microsoft::Common::Unicode::Utf8ToUtf16(props->nativeId) : ::Microsoft::Common::Unicode::Utf8ToUtf16(props->testId);
      pRetVal->bstrVal = SysAllocString(id.c_str());
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
      pRetVal->boolVal =
          (props->accessible && (props->accessibilityRole != "none" || props->role != facebook::react::Role::None))
          ? VARIANT_TRUE
          : VARIANT_FALSE;
      break;
    }
    case UIA_IsControlElementPropertyId: {
      pRetVal->vt = VT_BOOL;
      pRetVal->boolVal =
          (props->accessible && (props->accessibilityRole != "none" || props->role != facebook::react::Role::None))
          ? VARIANT_TRUE
          : VARIANT_FALSE;
      break;
    }
    case UIA_IsOffscreenPropertyId: {
      pRetVal->vt = VT_BOOL;
      pRetVal->boolVal = (compositionView->getClipState() == ClipState::FullyClipped) ? VARIANT_TRUE : VARIANT_FALSE;
      break;
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
    case UIA_LevelPropertyId: {
      pRetVal->vt = VT_I4;
      pRetVal->lVal = props->accessibilityLevel;
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

HRESULT __stdcall CompositionDynamicAutomationProvider::get_HorizontalScrollPercent(double *pRetVal) {
  BOOL horizontallyScrollable;
  auto hr = get_HorizontallyScrollable(&horizontallyScrollable);
  if (!SUCCEEDED(hr)) {
    return hr;
  }
  if (!horizontallyScrollable) {
    *pRetVal = UIA_ScrollPatternNoScroll;
  } else {
    auto strongView = m_view.view();
    auto scrollComponentView =
        strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ScrollViewComponentView>();
    *pRetVal = scrollComponentView->getScrollPositionX();
  }
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_VerticalScrollPercent(double *pRetVal) {
  BOOL verticallyScrollable;
  auto hr = get_VerticallyScrollable(&verticallyScrollable);
  if (!SUCCEEDED(hr)) {
    return hr;
  }
  if (!verticallyScrollable) {
    *pRetVal = UIA_ScrollPatternNoScroll;
  } else {
    auto strongView = m_view.view();
    auto scrollComponentView =
        strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ScrollViewComponentView>();
    *pRetVal = scrollComponentView->getScrollPositionY();
  }
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_HorizontalViewSize(double *pRetVal) {
  BOOL horizontallyScrollable;
  auto hr = get_HorizontallyScrollable(&horizontallyScrollable);
  if (!SUCCEEDED(hr)) {
    return hr;
  }
  if (!horizontallyScrollable) {
    *pRetVal = 100;
  } else {
    auto strongView = m_view.view();
    auto scrollComponentView =
        strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ScrollViewComponentView>();
    *pRetVal = scrollComponentView->getHorizontalSize();
  }
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_VerticalViewSize(double *pRetVal) {
  BOOL verticallyScrollable;
  auto hr = get_VerticallyScrollable(&verticallyScrollable);
  if (!SUCCEEDED(hr)) {
    return hr;
  }
  if (!verticallyScrollable) {
    *pRetVal = 100;
  } else {
    auto strongView = m_view.view();
    auto scrollComponentView =
        strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ScrollViewComponentView>();
    *pRetVal = scrollComponentView->getVerticalSize();
  }
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_HorizontallyScrollable(BOOL *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ScrollViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->props());
  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;
  *pRetVal = (props->horizontal && props->scrollEnabled);
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_VerticallyScrollable(BOOL *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ScrollViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->props());
  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;
  *pRetVal = (!props->horizontal && props->scrollEnabled);
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::Scroll(
    ScrollAmount horizontalAmount,
    ScrollAmount verticalAmount) {
  DispatchAccessibilityAction(m_view, "scroll");
  auto strongView = m_view.view();
  auto scrollComponentView =
      strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ScrollViewComponentView>();
  BOOL verticallyScrollable;
  BOOL horizontallyScrollable;
  float vertical = 0.0f;
  float horizontal = 0.0f;
  auto hr = get_VerticallyScrollable(&verticallyScrollable);
  if (!SUCCEEDED(hr)) {
    return hr;
  }
  if (verticallyScrollable) {
    if (verticalAmount == ScrollAmount_LargeIncrement) {
      scrollComponentView->pageDown(true);
    } else if (verticalAmount == ScrollAmount_LargeDecrement) {
      scrollComponentView->pageUp(true);
    } else if (verticalAmount == ScrollAmount_SmallIncrement) {
      scrollComponentView->lineDown(true);
    } else if (verticalAmount == ScrollAmount_SmallDecrement) {
      scrollComponentView->lineUp(true);
    }
  }
  hr = get_HorizontallyScrollable(&horizontallyScrollable);
  if (!SUCCEEDED(hr)) {
    return hr;
  }
  if (horizontallyScrollable) {
    if (horizontalAmount == ScrollAmount_LargeIncrement) {
      scrollComponentView->pageDown(true);
    } else if (horizontalAmount == ScrollAmount_LargeDecrement) {
      scrollComponentView->pageUp(true);
    } else if (horizontalAmount == ScrollAmount_SmallIncrement) {
      scrollComponentView->lineRight(true);
    } else if (horizontalAmount == ScrollAmount_SmallDecrement) {
      scrollComponentView->lineLeft(true);
    }
  }
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::SetScrollPercent(
    double horiztonalPercent,
    double verticalPercent) {
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
  // TODO: Edit once/if onAccessibilityAction props supports returning UIA event data. See
  // https://github.com/react-native-community/discussions-and-proposals/issues/843.
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
  if (props->accessibilityState.has_value() && props->accessibilityState->readOnly.has_value()) {
    *pRetVal = props->accessibilityState->readOnly.value();
  } else {
    // Use default IsReadOnly value.
    *pRetVal = false;
  }
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_LargeChange(double *pRetVal) {
  // no-op
  return S_OK;
}
HRESULT __stdcall CompositionDynamicAutomationProvider::get_Maximum(double *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->props());

  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  *pRetVal = props->accessibilityValue.max.value();
  return S_OK;
}
HRESULT __stdcall CompositionDynamicAutomationProvider::get_Minimum(double *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->props());

  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  *pRetVal = props->accessibilityValue.min.value();
  return S_OK;
}
HRESULT __stdcall CompositionDynamicAutomationProvider::get_SmallChange(double *pRetVal) {
  // no-op
  return S_OK;
}
HRESULT __stdcall CompositionDynamicAutomationProvider::get_Value(double *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->props());

  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  *pRetVal = props->accessibilityValue.now.value();
  return S_OK;
}
HRESULT __stdcall CompositionDynamicAutomationProvider::SetValue(double val) {
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  // TODO: Edit once/if onAccessibilityAction props supports returning UIA event data. See
  // https://github.com/react-native-community/discussions-and-proposals/issues/843.
  DispatchAccessibilityAction(m_view, "setValue");
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_ToggleState(ToggleState *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->props());

  *pRetVal = (props->accessibilityState.has_value() &&
              props->accessibilityState->checked != facebook::react::AccessibilityState::None)
      ? GetToggleState(props->accessibilityState)
      : winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->getToggleState();
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

HRESULT __stdcall CompositionDynamicAutomationProvider::get_CanSelectMultiple(BOOL *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props =
      std::static_pointer_cast<const facebook::react::ViewProps>(winrt::get_self<ComponentView>(strongView)->props());

  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  *pRetVal = (props->accessibilityState.has_value() && props->accessibilityState->multiselectable.has_value())
      ? props->accessibilityState->multiselectable.value()
      : false;

  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_IsSelectionRequired(BOOL *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->props());

  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  *pRetVal = (props->accessibilityState.has_value() && props->accessibilityState->required.has_value())
      ? props->accessibilityState->required.value()
      : false;

  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::GetSelection(SAFEARRAY **pRetVal) {
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  std::vector<int> selectedItems;
  for (size_t i = 0; i < m_selectionItems.size(); i++) {
    auto selectionItem = m_selectionItems.at(i);
    auto provider = selectionItem.as<CompositionDynamicAutomationProvider>();
    BOOL selected;
    auto hr = provider->get_IsSelected(&selected);
    if (hr == S_OK && selected) {
      selectedItems.push_back(int(i));
    }
  }

  *pRetVal = SafeArrayCreateVector(VT_UNKNOWN, 0, ULONG(selectedItems.size()));
  if (*pRetVal == nullptr)
    return E_OUTOFMEMORY;

  for (size_t i = 0; i < selectedItems.size(); i++) {
    auto pos = static_cast<long>(i);
    SafeArrayPutElement(*pRetVal, &pos, m_selectionItems.at(selectedItems.at(i)).get());
  }
  return S_OK;
}

void CompositionDynamicAutomationProvider::AddToSelectionItems(winrt::com_ptr<IRawElementProviderSimple> &item) {
  if (std::find(m_selectionItems.begin(), m_selectionItems.end(), item) != m_selectionItems.end()) {
    return;
  }
  m_selectionItems.push_back(item);
}

void CompositionDynamicAutomationProvider::RemoveFromSelectionItems(winrt::com_ptr<IRawElementProviderSimple> &item) {
  std::erase(m_selectionItems, item);
}

HRESULT __stdcall CompositionDynamicAutomationProvider::AddToSelection() {
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  DispatchAccessibilityAction(m_view, "addToSelection");
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_IsSelected(BOOL *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->props());

  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  *pRetVal = (props->accessibilityState.has_value() && props->accessibilityState->selected.has_value())
      ? props->accessibilityState->selected.value()
      : false;

  return S_OK;
}

IRawElementProviderSimple *findSelectionContainer(winrt::Microsoft::ReactNative::ComponentView current) {
  if (!current)
    return nullptr;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(current)->props());
  if (props->accessibilityState.has_value() && props->accessibilityState->multiselectable.has_value() &&
      props->accessibilityState->required.has_value()) {
    auto uiaProvider =
        current.as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>()->EnsureUiaProvider();
    if (uiaProvider != nullptr) {
      auto spProviderSimple = uiaProvider.try_as<IRawElementProviderSimple>();
      if (spProviderSimple != nullptr) {
        spProviderSimple->AddRef();
        return spProviderSimple.get();
      }
    }
  } else {
    return findSelectionContainer(current.Parent());
  }
  return nullptr;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::get_SelectionContainer(IRawElementProviderSimple **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  *pRetVal = findSelectionContainer(strongView.Parent());
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::RemoveFromSelection() {
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;
  DispatchAccessibilityAction(m_view, "removeFromSelection");
  return S_OK;
}

HRESULT __stdcall CompositionDynamicAutomationProvider::Select() {
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;
  DispatchAccessibilityAction(m_view, "select");
  return S_OK;
}

} // namespace winrt::Microsoft::ReactNative::implementation
