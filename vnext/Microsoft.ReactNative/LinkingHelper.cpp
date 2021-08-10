#include "pch.h"
#include "LinkingHelper.h"

namespace winrt::Microsoft::ReactNative {

static IReactPropertyName InitialUrlProperty() {
  static IReactPropertyName propName =
      ReactPropertyBagHelper::GetName(ReactPropertyBagHelper::GetNamespace(L"ReactNative.Linking"), L"InitialUrl");
  return propName;
}

/*static*/ void LinkingHelper::SetInitialUrl(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties,
    winrt::hstring const &url) noexcept {
  properties.Set(InitialUrlProperty(), winrt::box_value(url));
}

/*static*/ winrt::hstring LinkingHelper::InitialUrl(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) noexcept {
  return winrt::unbox_value_or<winrt::hstring>(properties.Get(InitialUrlProperty()), {});
}

} // namespace winrt::Microsoft::ReactNative
