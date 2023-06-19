#include "pch.h"
#include "UiaHelpers.h"
#include <Fabric/Composition/CompositionViewComponentView.h>
#include <atlcomcli.h>
#include <inspectable.h>
#include "CompositionRootAutomationProvider.h"
#include "RootComponentView.h"

namespace winrt::Microsoft::ReactNative::implementation {

HRESULT UiaNavigateHelper(
    ::Microsoft::ReactNative::ReactTaggedView &view,
    NavigateDirection direction,
    IRawElementProviderFragment *&retVal) noexcept {
  retVal = nullptr;

  auto spComponentView = view.view();

  if (!spComponentView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  winrt::IInspectable uiaProvider{nullptr};

  switch (direction) {
    case NavigateDirection_Parent: {
      auto pParentCV = static_cast<::Microsoft::ReactNative::CompositionBaseComponentView *>(spComponentView->parent());
      if (pParentCV != nullptr) {
        uiaProvider = pParentCV->EnsureUiaProvider();
      }
    } break;

    case NavigateDirection_LastChild:
      __fallthrough;

    case NavigateDirection_FirstChild: {
      auto children = spComponentView->children();
      auto index = direction == NavigateDirection_FirstChild ? 0 : children.size() - 1;
      if (!children.empty()) {
        uiaProvider =
            static_cast<::Microsoft::ReactNative::CompositionBaseComponentView *>(children[index])->EnsureUiaProvider();
      }
    } break;

    case NavigateDirection_NextSibling: {
      auto pParentCV = static_cast<::Microsoft::ReactNative::CompositionBaseComponentView *>(spComponentView->parent());
      if (pParentCV != nullptr) {
        auto children = pParentCV->children();
        auto it = std::find(children.begin(), children.end(), spComponentView.get());
        if (++it != children.end()) {
          uiaProvider = static_cast<::Microsoft::ReactNative::CompositionBaseComponentView *>(*it)->EnsureUiaProvider();
        }
      }
    } break;

    case NavigateDirection_PreviousSibling: {
      auto pParentCV = static_cast<::Microsoft::ReactNative::CompositionBaseComponentView *>(spComponentView->parent());
      if (pParentCV != nullptr) {
        auto children = pParentCV->children();
        auto it = std::find(children.rbegin(), children.rend(), spComponentView.get());
        if (++it != children.rend()) {
          uiaProvider = static_cast<::Microsoft::ReactNative::CompositionBaseComponentView *>(*it)->EnsureUiaProvider();
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

  auto clientRect = strongView->getClientRect();

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

  auto rootCV = strongView->rootComponentView();
  if (rootCV == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  return rootCV->TrySetFocusedComponent(*strongView) ? S_OK : E_FAIL;
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

} // namespace winrt::Microsoft::ReactNative::implementation
