// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#undef WINRT_LEAN_AND_MEAN

#include "HttpFilterFactorySettings.h"
#include <ReactPropertyBag.h>
#include "RedirectHttpFilter.h"

#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Web.Http.h>

namespace Microsoft::React::Networking {

winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<HttpFilterFactoryDelegate>>
HttpFilterFactoryDelegateProperty() noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyId<
      winrt::Microsoft::ReactNative::ReactNonAbiValue<HttpFilterFactoryDelegate>>
      propId{L"ReactNative.Http", L"HttpFilterFactory"};
  return propId;
}

void SetHttpFilterFactoryDelegate(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties,
    HttpFilterFactoryDelegate const &value) noexcept {
  properties.Set(HttpFilterFactoryDelegateProperty(), value);
}
const HttpFilterFactoryDelegate GetHttpFilterFactoryDelegate(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept {
  const auto delegate = properties.Get(HttpFilterFactoryDelegateProperty());
  if (!delegate) {
    return nullptr;
  }
  return delegate.Value();
}

} // namespace Microsoft::React::Networking
