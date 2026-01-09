#include "pch.h"
#include "UiaHelpers.h"
#include <Fabric/Composition/CompositionViewComponentView.h>
#include <atlcomcli.h>
#include <inspectable.h>
#include "CompositionRootAutomationProvider.h"
#include "RootComponentView.h"

namespace winrt::Microsoft::ReactNative::implementation {

HRESULT UiaNavigateHelper(
    const winrt::Microsoft::ReactNative::ComponentView &view,
    NavigateDirection direction,
    IRawElementProviderFragment *&retVal) noexcept {
  retVal = nullptr;

  if (!view)
    return UIA_E_ELEMENTNOTAVAILABLE;

  winrt::IInspectable uiaProvider{nullptr};

  switch (direction) {
    case NavigateDirection_Parent: {
      auto parentCV = view.Parent().as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>();
      if (parentCV != nullptr) {
        uiaProvider = parentCV->EnsureUiaProvider();
      } else {
        if (auto root = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(view)
                            ->rootComponentView()) {
          winrt::com_ptr<IRawElementProviderFragmentRoot> spFragmentRoot;
          auto hr = root->GetFragmentRoot(spFragmentRoot.put());
          if (FAILED(hr)) {
            return hr;
          }

          auto spFragment = spFragmentRoot.try_as<IRawElementProviderFragment>();
          if (spFragment != nullptr) {
            retVal = spFragment.detach();
            return S_OK;
          }
        }
      }
    } break;

    case NavigateDirection_LastChild: {
      auto children = view.Children();
      if (children.Size() != 0) {
        uint32_t index = children.Size() - 1;
        do {
          auto child = children.GetAt(index).as<winrt::Microsoft::ReactNative::implementation::ComponentView>();
          if (uiaProvider = child->EnsureUiaProvider()) {
            break;
          }
        } while (index-- != 0);
      }
    } break;
    case NavigateDirection_FirstChild: {
      auto children = view.Children();
      if (children.Size() != 0) {
        uint32_t index = 0;
        do {
          auto child = children.GetAt(index).as<winrt::Microsoft::ReactNative::implementation::ComponentView>();
          if (uiaProvider = child->EnsureUiaProvider()) {
            break;
          }
        } while (++index != children.Size());
      }
    } break;
    case NavigateDirection_NextSibling: {
      auto parentCV = view.Parent().as<winrt::Microsoft::ReactNative::implementation::ComponentView>();
      if (parentCV != nullptr) {
        auto children = parentCV->Children();
        auto it = std::find(children.begin(), children.end(), view);

        while (++it != children.end()) {
          auto nextchild = (*it).as<winrt::Microsoft::ReactNative::implementation::ComponentView>();
          if (uiaProvider = nextchild->EnsureUiaProvider()) {
            break;
          }
        }
      }
    } break;

    case NavigateDirection_PreviousSibling: {
      auto parentCV = view.Parent().as<winrt::Microsoft::ReactNative::implementation::ComponentView>();
      if (parentCV != nullptr) {
        auto children = parentCV->Children();
        auto it = std::find(children.begin(), children.end(), view);

        if (it != children.begin()) {
          do {
            it--;
            auto prevchild = (*it).as<winrt::Microsoft::ReactNative::implementation::ComponentView>();
            if (uiaProvider = prevchild->EnsureUiaProvider()) {
              break;
            }
          } while (it != children.begin());
        }
      }
    } break;
  }

  auto spFragment = uiaProvider.try_as<IRawElementProviderFragment>();
  if (spFragment != nullptr) {
    retVal = spFragment.detach();
  }

  return S_OK;
}

HRESULT UiaGetBoundingRectangleHelper(::Microsoft::ReactNative::ReactTaggedView &view, UiaRect &rect) noexcept {
  auto strongView = view.view();

  if (strongView == nullptr) {
    return UIA_E_ELEMENTNOTAVAILABLE;
  }

  auto clientRect =
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->getClientRect();

  rect.left = clientRect.left;
  rect.top = clientRect.top;
  rect.width = clientRect.right - clientRect.left;
  rect.height = clientRect.bottom - clientRect.top;

  return S_OK;
}

HRESULT UiaSetFocusHelper(::Microsoft::ReactNative::ReactTaggedView &view) noexcept {
  auto strongView = view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto rootCV =
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->rootComponentView();
  if (rootCV == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  return rootCV->TrySetFocusedComponent(strongView, winrt::Microsoft::ReactNative::FocusNavigationDirection::None)
      ? S_OK
      : E_FAIL;
}

bool WasUiaPropertyAdvised(winrt::com_ptr<IRawElementProviderSimple> &providerSimple, PROPERTYID propId) noexcept {
  auto spFragment = providerSimple.try_as<IRawElementProviderFragment>();
  if (spFragment == nullptr)
    return false;

  winrt::com_ptr<IRawElementProviderFragmentRoot> spFragmentRoot;
  spFragment->get_FragmentRoot(spFragmentRoot.put());
  if (spFragmentRoot == nullptr)
    return false;

  auto rootProvider = static_cast<CompositionRootAutomationProvider *>(spFragmentRoot.get());

  return rootProvider->WasPropertyAdvised(propId);
}

void UpdateUiaProperty(winrt::IInspectable provider, PROPERTYID propId, bool oldValue, bool newValue) noexcept {
  auto spProviderSimple = provider.try_as<IRawElementProviderSimple>();

  if (spProviderSimple == nullptr || oldValue == newValue || !WasUiaPropertyAdvised(spProviderSimple, propId))
    return;

  UiaRaiseAutomationPropertyChangedEvent(spProviderSimple.get(), propId, CComVariant(oldValue), CComVariant(newValue));
}

void UpdateUiaProperty(winrt::IInspectable provider, PROPERTYID propId, int oldValue, int newValue) noexcept {
  auto spProviderSimple = provider.try_as<IRawElementProviderSimple>();

  if (spProviderSimple == nullptr || oldValue == newValue || !WasUiaPropertyAdvised(spProviderSimple, propId))
    return;

  UiaRaiseAutomationPropertyChangedEvent(spProviderSimple.get(), propId, CComVariant(oldValue), CComVariant(newValue));
}

void UpdateUiaProperty(winrt::IInspectable provider, PROPERTYID propId, long oldValue, long newValue) noexcept {
  auto spProviderSimple = provider.try_as<IRawElementProviderSimple>();

  if (spProviderSimple == nullptr || oldValue == newValue || !WasUiaPropertyAdvised(spProviderSimple, propId))
    return;

  UiaRaiseAutomationPropertyChangedEvent(spProviderSimple.get(), propId, CComVariant(oldValue), CComVariant(newValue));
}

void UpdateUiaProperty(
    winrt::IInspectable provider,
    PROPERTYID propId,
    const std::string &oldValue,
    const std::string &newValue) noexcept {
  auto spProviderSimple = provider.try_as<IRawElementProviderSimple>();

  if (spProviderSimple == nullptr || oldValue == newValue || !WasUiaPropertyAdvised(spProviderSimple, propId))
    return;

  UiaRaiseAutomationPropertyChangedEvent(
      spProviderSimple.get(), propId, CComVariant(oldValue.c_str()), CComVariant(newValue.c_str()));
}

void UpdateUiaProperty(
    winrt::IInspectable provider,
    PROPERTYID propId,
    const std::optional<std::string> &oldValue,
    const std::optional<std::string> &newValue) noexcept {
  std::string oldData = oldValue.value_or("");
  std::string newData = newValue.value_or("");
  UpdateUiaProperty(provider, propId, oldData, newData);
}

void UpdateUiaPropertiesForAnnotation(
    winrt::IInspectable provider,
    const std::optional<facebook::react::AccessibilityAnnotation> &oldAnnotation,
    const std::optional<facebook::react::AccessibilityAnnotation> &newAnnotation) noexcept {
  // if no value fall back to a default value.
  const auto &old_annotation = oldAnnotation.value_or(facebook::react::AccessibilityAnnotation());
  const auto &new_annotation = newAnnotation.value_or(facebook::react::AccessibilityAnnotation());

  // Update all annotation properties
  UpdateUiaProperty(
      provider,
      UIA_AnnotationAnnotationTypeIdPropertyId,
      GetAnnotationTypeId(old_annotation.typeID),
      GetAnnotationTypeId(new_annotation.typeID));

  UpdateUiaProperty(
      provider, UIA_AnnotationAnnotationTypeNamePropertyId, old_annotation.typeName, new_annotation.typeName);

  UpdateUiaProperty(provider, UIA_AnnotationAuthorPropertyId, old_annotation.author, new_annotation.author);

  UpdateUiaProperty(provider, UIA_AnnotationDateTimePropertyId, old_annotation.dateTime, new_annotation.dateTime);
}

long GetLiveSetting(const std::string &liveRegion) noexcept {
  if (liveRegion == "polite") {
    return LiveSetting::Polite;
  } else if (liveRegion == "assertive") {
    return LiveSetting::Assertive;
  }
  return LiveSetting::Off;
}

long GetAnnotationTypeId(const std::string &annotationType) noexcept {
  if (annotationType == "Author") {
    return AnnotationType_Author;
  } else if (annotationType == "CircularReferenceError") {
    return AnnotationType_CircularReferenceError;
  } else if (annotationType == "Comment") {
    return AnnotationType_Comment;
  } else if (annotationType == "ConflictingChange") {
    return AnnotationType_ConflictingChange;
  } else if (annotationType == "DataValidationError") {
    return AnnotationType_DataValidationError;
  } else if (annotationType == "DeletionChange") {
    return AnnotationType_DeletionChange;
  } else if (annotationType == "EditingLockedChange") {
    return AnnotationType_EditingLockedChange;
  } else if (annotationType == "Endnote") {
    return AnnotationType_Endnote;
  } else if (annotationType == "ExternalChange") {
    return AnnotationType_ExternalChange;
  } else if (annotationType == "Footer") {
    return AnnotationType_Footer;
  } else if (annotationType == "Footnote") {
    return AnnotationType_Footnote;
  } else if (annotationType == "FormatChange") {
    return AnnotationType_FormatChange;
  } else if (annotationType == "FormulaError") {
    return AnnotationType_FormulaError;
  } else if (annotationType == "GrammarError") {
    return AnnotationType_GrammarError;
  } else if (annotationType == "Header") {
    return AnnotationType_Header;
  } else if (annotationType == "Highlighted") {
    return AnnotationType_Highlighted;
  } else if (annotationType == "InsertionChange") {
    return AnnotationType_InsertionChange;
  } else if (annotationType == "Mathematics") {
    return AnnotationType_Mathematics;
  } else if (annotationType == "MoveChange") {
    return AnnotationType_MoveChange;
  } else if (annotationType == "SpellingError") {
    return AnnotationType_SpellingError;
  } else if (annotationType == "TrackChanges") {
    return AnnotationType_TrackChanges;
  } else if (annotationType == "AdvanceProofingIssue") {
    return AnnotationType_AdvancedProofingIssue;
  } else if (annotationType == "UnsyncedChange") {
    return AnnotationType_UnsyncedChange;
  }
  return AnnotationType_Unknown;
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

long GetHeadingLevel(int headingLevel, const std::string &strRole, const facebook::react::Role &role) noexcept {
  if (strRole != "header" && role != facebook::react::Role::Heading) {
    return HeadingLevel_None;
  }

  switch (headingLevel) {
    case 1:
      return HeadingLevel1;
    case 2:
      return HeadingLevel2;
    case 3:
      return HeadingLevel3;
    case 4:
      return HeadingLevel4;
    case 5:
      return HeadingLevel5;
    case 6:
      return HeadingLevel6;
    case 7:
      return HeadingLevel7;
    case 8:
      return HeadingLevel8;
    case 9:
      return HeadingLevel9;
    default:
      return HeadingLevel_None;
  }
}

bool accessibilityAnnotationHasValue(
    const std::optional<facebook::react::AccessibilityAnnotation> &annotation) noexcept {
  return annotation.has_value() &&
      !(annotation.value().typeID.empty() && annotation.value().typeName.empty() && annotation.value().author.empty() &&
        annotation.value().dateTime.empty() && annotation.value().target.empty());
}

void DispatchAccessibilityAction(::Microsoft::ReactNative::ReactTaggedView &view, const std::string &action) noexcept {
  auto strongView = view.view();

  if (!strongView)
    return;

  auto baseView = strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>();
  if (baseView == nullptr)
    return;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(baseView->props());
  if (props == nullptr)
    return;

  auto accessibilityActions = props->accessibilityActions;
  for (size_t i = 0; i < accessibilityActions.size(); i++) {
    if (accessibilityActions[i].name == action) {
      baseView->GetEventEmitter()->onAccessibilityAction(action);
    }
  }
}

ExpandCollapseState GetExpandCollapseState(const bool &expanded) noexcept {
  if (expanded) {
    return ExpandCollapseState_Expanded;
  } else {
    return ExpandCollapseState_Collapsed;
  }
}

void AddSelectionItemsToContainer(CompositionDynamicAutomationProvider *provider) noexcept {
  auto selectionContainerView = provider->GetSelectionContainer();
  if (!selectionContainerView)
    return;

  auto selectionContainerCompView =
      selectionContainerView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>();
  if (!selectionContainerCompView)
    return;

  selectionContainerCompView->EnsureUiaProvider();

  if (!selectionContainerCompView->InnerAutomationProvider())
    return;

  auto simpleProvider = static_cast<IRawElementProviderSimple *>(provider);
  winrt::com_ptr<IRawElementProviderSimple> simpleProviderPtr;
  simpleProviderPtr.copy_from(simpleProvider);
  selectionContainerCompView->InnerAutomationProvider()->AddToSelectionItems(simpleProviderPtr);
}

void RemoveSelectionItemsFromContainer(CompositionDynamicAutomationProvider *provider) noexcept {
  auto selectionContainerView = provider->GetSelectionContainer();
  if (!selectionContainerView)
    return;

  auto selectionContainerCompView =
      selectionContainerView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>();
  if (!selectionContainerCompView)
    return;

  selectionContainerCompView->EnsureUiaProvider();

  if (!selectionContainerCompView->InnerAutomationProvider())
    return;

  auto simpleProvider = static_cast<IRawElementProviderSimple *>(provider);
  winrt::com_ptr<IRawElementProviderSimple> simpleProviderPtr;
  simpleProviderPtr.copy_from(simpleProvider);
  selectionContainerCompView->InnerAutomationProvider()->RemoveFromSelectionItems(simpleProviderPtr);
}

ToggleState GetToggleState(const std::optional<facebook::react::AccessibilityState> &state) noexcept {
  if (state.has_value()) {
    if (state->checked == facebook::react::AccessibilityState::Checked) {
      return ToggleState::ToggleState_On;
    } else if (state->checked == facebook::react::AccessibilityState::Mixed) {
      return ToggleState::ToggleState_Indeterminate;
    }
  }
  return ToggleState::ToggleState_Off;
}

TextDecorationLineStyle GetTextDecorationLineStyle(facebook::react::TextDecorationStyle style) noexcept {
  if (style == facebook::react::TextDecorationStyle::Dashed) {
    return TextDecorationLineStyle_Dash;
  } else if (style == facebook::react::TextDecorationStyle::Dotted) {
    return TextDecorationLineStyle_Dot;
  } else if (style == facebook::react::TextDecorationStyle::Double) {
    return TextDecorationLineStyle_Double;
  } else if (style == facebook::react::TextDecorationStyle::Solid) {
    return TextDecorationLineStyle_Single;
  } else {
    return TextDecorationLineStyle_Single;
  }
}

} // namespace winrt::Microsoft::ReactNative::implementation
