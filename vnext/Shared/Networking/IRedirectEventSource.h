// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>

namespace Microsoft::React::Networking {

struct IRedirectEventSource2 : winrt::implements<IRedirectEventSource2, winrt::Windows::Foundation::IInspectable>{
    virtual bool OnRedirecting2(
      winrt::Windows::Web::Http::HttpRequestMessage const &request,
      winrt::Windows::Web::Http::HttpResponseMessage const &response) noexcept = 0;
};

}
