// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/base.h>

namespace Microsoft::React::Networking {

struct IRedirectEventSource : winrt::implements<IRedirectEventSource, winrt::Windows::Foundation::IInspectable> {
  virtual bool OnRedirecting(
      winrt::Windows::Web::Http::HttpRequestMessage const &request,
      winrt::Windows::Web::Http::HttpResponseMessage const &response) noexcept = 0;
};

} // namespace Microsoft::React::Networking
