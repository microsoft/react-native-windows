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
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(getSize)
  void getSize(std::string uri, winrt::Microsoft::ReactNative::ReactPromise<std::vector<double>> &&result) noexcept;

  REACT_METHOD(getSizeWithHeaders)
  void getSizeWithHeaders(
      std::string uri,
      winrt::Microsoft::ReactNative::JSValue &&headers,
      winrt::Microsoft::ReactNative::ReactPromise<
          Microsoft::ReactNativeSpecs::ImageLoaderIOSSpec_getSizeWithHeaders_returnType> &&result) noexcept;

  REACT_METHOD(prefetchImage)
  void prefetchImage(std::string uri, winrt::Microsoft::ReactNative::ReactPromise<bool> &&result) noexcept;

  REACT_METHOD(prefetchImageWithMetadata)
  void prefetchImageWithMetadata(
      std::string uri,
      std::string queryRootName,
      double rootTag,
      winrt::Microsoft::ReactNative::ReactPromise<bool> &&result) noexcept;

  REACT_METHOD(queryCache)
  void queryCache(
      std::vector<std::string> const &uris,
      winrt::Microsoft::ReactNative::ReactPromise<winrt::Microsoft::ReactNative::JSValue> &&result) noexcept;

 private:
  winrt::Microsoft::ReactNative::ReactContext m_context;
};

} // namespace Microsoft::ReactNative
