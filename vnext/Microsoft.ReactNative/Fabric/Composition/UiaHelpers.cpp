#include "pch.h"
#include "UiaHelpers.h"
#include <Fabric/Composition/CompositionViewComponentView.h>
#include <inspectable.h>
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

HRESULT UiaSetFocusHelper(::Microsoft::ReactNative::ReactTaggedView& view) noexcept
{
  auto strongView = view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto rootCV = strongView->rootComponentView();
  if (rootCV == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  return rootCV->TrySetFocusedComponent(*strongView) ? S_OK : E_FAIL;
}

HRESULT UpdateUiaProperty(winrt::IInspectable provider, PROPERTYID propId, bool oldValue, bool newValue) noexcept {
  auto spProviderSimple = provider.try_as<IRawElementProviderSimple>();

  if (spProviderSimple == nullptr || !UiaClientsAreListening())
    return S_OK;

  return UpdateUiaProperty(*spProviderSimple.get(), propId, oldValue, newValue);
}

HRESULT UpdateUiaProperty(
    IRawElementProviderSimple &providerSimple,
    PROPERTYID propId,
    bool oldValue,
    bool newValue) noexcept {

  if (oldValue == newValue)
    return S_OK;

  VARIANT oldVariant;
  oldVariant.vt = VT_BOOL;
  oldVariant.boolVal = oldValue ? VARIANT_TRUE : VARIANT_FALSE;

  VARIANT newVariant;
  newVariant.vt = VT_BOOL;
  newVariant.boolVal = newValue ? VARIANT_TRUE : VARIANT_FALSE;

  return UiaRaiseAutomationPropertyChangedEvent(&providerSimple, propId, oldVariant, newVariant);
}

} // namespace winrt::Microsoft::ReactNative::implementation
