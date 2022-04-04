// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "IHttpResource.h"

// Windows API
#include <winrt/Windows.Web.Http.h>

// Standard Library
#include <mutex>

namespace Microsoft::React::Networking {

struct RequestArgs : public winrt::implements<RequestArgs, winrt::Windows::Foundation::IInspectable> {
  int64_t RequestId;
  IHttpResource::Headers Headers;
  IHttpResource::BodyData Body;
  bool IncrementalUpdates;
  bool WithCredentials;
  bool IsText;
  int64_t Timeout;
};

typedef winrt::Windows::Foundation::IAsyncOperationWithProgress<
    winrt::Windows::Web::Http::HttpResponseMessage,
    winrt::Windows::Web::Http::HttpProgress>
    ResponseType;

} // namespace Microsoft::React::Networking
