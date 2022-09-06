// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "codegen/NativeImageLoaderIOSSpec.g.h"
#include <NativeModules.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Display.h>

namespace Microsoft::ReactNative {

REACT_MODULE(ImageLoader)
struct ImageLoader {
  using ModuleSpec = ReactNativeSpecs::ImageLoaderIOSSpec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_METHOD(getSize)
  void getSize(std::string uri, React::ReactPromise<React::JSValue> &&result) noexcept;

  REACT_METHOD(getSizeWithHeaders)
  void
  getSizeWithHeaders(std::string uri, React::JSValue &&headers, React::ReactPromise<React::JSValue> &&result) noexcept;

  REACT_METHOD(prefetchImage)
  void prefetchImage(std::string uri, React::ReactPromise<React::JSValue> &&result) noexcept;

  REACT_METHOD(prefetchImageWithMetadata)
  void prefetchImageWithMetadata(
      std::string uri,
      std::string queryRootName,
      double rootTag,
      React::ReactPromise<React::JSValue> &&result) noexcept;

  REACT_METHOD(queryCache)
  void queryCache(std::vector<std::string> const &uris, React::ReactPromise<React::JSValue> &&result) noexcept;

 private:
  React::ReactContext m_context;
};

} // namespace Microsoft::ReactNative
