// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <tuple>

#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/base.h>

#include <ReactPropertyBag.h>

namespace Microsoft::React::Networking {

using HttpFilterFactoryDelegate =
    std::function<winrt::Windows::Web::Http::Filters::IHttpFilter()>;

void SetHttpFilterFactoryDelegate(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties,
    HttpFilterFactoryDelegate const& value) noexcept;
const HttpFilterFactoryDelegate GetHttpFilterFactoryDelegate(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept;

} // namespace Microsoft::React::Networking
