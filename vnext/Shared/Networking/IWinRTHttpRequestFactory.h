// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>

namespace Microsoft::React::Networking {

struct IWinRTHttpRequestFactory {
  virtual ~IWinRTHttpRequestFactory() noexcept {}

  // When includeContent is false, the request body is not built (e.g. after a redirect changed the method to GET).
  virtual winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Web::Http::HttpRequestMessage> CreateRequest(
      winrt::Windows::Web::Http::HttpMethod &&method,
      winrt::Windows::Foundation::Uri &&uri,
      winrt::Windows::Foundation::Collections::IMap<winrt::hstring, winrt::Windows::Foundation::IInspectable> props,
      bool includeContent) noexcept = 0;
};

} // namespace Microsoft::React::Networking
