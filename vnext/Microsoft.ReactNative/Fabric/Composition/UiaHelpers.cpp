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

    case NavigateDirection_LastChild:
      __fallthrough;

    case NavigateDirection_FirstChild: {
      auto children = view.Children();
      auto index = direction == NavigateDirection_FirstChild ? 0 : children.Size() - 1;
      if (!children.Size() == 0) {
        uiaProvider = children.GetAt(index)
                          .as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>()
                          ->EnsureUiaProvider();
      }
    } break;

    case NavigateDirection_NextSibling: {
      auto parentCV = view.Parent().as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>();
      if (parentCV != nullptr) {
        auto children = parentCV->Children();
        auto it = std::find(children.begin(), children.end(), view);
        if (++it != children.end()) {
          uiaProvider = (*it)
                            .as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>()
                            ->EnsureUiaProvider();
        }
      }
    } break;

    case NavigateDirection_PreviousSibling: {
      auto parentCV = view.Parent().as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>();
      if (parentCV != nullptr) {
        auto children = parentCV->Children();
        for (auto i = children.Size() - 1; i > 0; i--) {
          auto child = children.GetAt(i);
          if (child == view) {
            uiaProvider = children.GetAt(i - 1)
                              .as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>()
                              ->EnsureUiaProvider();
            break;
          }
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

long GetLiveSetting(const std::string &liveRegion) noexcept {
  if (liveRegion == "polite") {
    return LiveSetting::Polite;
  } else if (liveRegion == "assertive") {
    return LiveSetting::Assertive;
  }
  return LiveSetting::Off;
}

std::string extractAccessibilityValue(const facebook::react::AccessibilityValue &value) noexcept {
  if (value.now.has_value()) {
    return std::to_string(value.now.value());
  } else if (value.text.has_value()) {
    return value.text.value();
  } else {
    return "";
  }
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

} // namespace winrt::Microsoft::ReactNative::implementation
