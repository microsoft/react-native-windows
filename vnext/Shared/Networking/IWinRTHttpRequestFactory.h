// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>

namespace Microsoft::React::Networking {

struct IWinRTHttpRequestFactory {
  virtual ~IWinRTHttpRequestFactory() noexcept {}

  virtual winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Web::Http::HttpRequestMessage> CreateRequest(
      winrt::Windows::Web::Http::HttpMethod &&method,
      winrt::Windows::Foundation::Uri &&uri,
      winrt::Windows::Foundation::Collections::IMap<winrt::hstring, winrt::Windows::Foundation::IInspectable>
          props) noexcept = 0;
};

} // namespace Microsoft::React::Networking
