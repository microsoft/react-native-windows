// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "IRequestBodyHandler.h"
#include "IResponseHandler.h"
#include "IUriHandler.h"

// Standard Library
#include <functional>

namespace Microsoft::React {

/// <summary>
/// Provides partial access to HttpModule methods directly to other native modules
/// without switching to the JavaScript queue thread.
/// </summary>
struct IHttpModuleProxy {
  virtual ~IHttpModuleProxy() noexcept {}

  // TODO: Implement custom URI handlers.
  virtual void AddUriHandler(std::shared_ptr<IUriHandler> uriHandler) noexcept = 0;

  virtual void AddRequestBodyHandler(std::shared_ptr<IRequestBodyHandler> requestBodyHandler) noexcept = 0;

  virtual void AddResponseHandler(std::shared_ptr<IResponseHandler> responseHandler) noexcept = 0;
};

} // namespace Microsoft::React
