// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "IHttpResource.h"

// Folly
#include <folly/dynamic.h>

// Windows API
#include <winrt/Windows.Web.Http.h>

// Standard Library
#include <mutex>

namespace Microsoft::React::Networking {

struct RequestArgs : public winrt::implements<RequestArgs, winrt::Windows::Foundation::IInspectable> {
  int64_t RequestId;
  IHttpResource::Headers Headers;
  folly::dynamic Data;
  bool IncrementalUpdates;
  bool WithCredentials;
  std::string ResponseType;
  int64_t Timeout;
};

typedef winrt::Windows::Foundation::
    IAsyncOperationWithProgress<winrt::Windows::Web::Http::HttpResponseMessage, winrt::Windows::Web::Http::HttpProgress>
        ResponseOperation;

} // namespace Microsoft::React::Networking
