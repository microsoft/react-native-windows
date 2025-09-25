// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <codegen/NativeFileReaderModuleSpec.g.h>

#include <IFileReaderResource.h>
#include <NativeModules.h>
#include "IBlobPersistor.h"

// Standard Library
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Microsoft::React {

REACT_MODULE(FileReaderTurboModule, L"FileReaderModule")
struct FileReaderTurboModule {
  using ModuleSpec = ReactNativeSpecs::FileReaderModuleSpec;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(ReadAsDataUrl, L"readAsDataURL")
  void ReadAsDataUrl(
      winrt::Microsoft::ReactNative::JSValue &&data,
      winrt::Microsoft::ReactNative::ReactPromise<std::string> &&result) noexcept;

  REACT_METHOD(ReadAsText, L"readAsText")
  void ReadAsText(
      winrt::Microsoft::ReactNative::JSValue &&data,
      std::string &&encoding,
      winrt::Microsoft::ReactNative::ReactPromise<std::string> &&result) noexcept;

 private:
  std::shared_ptr<IFileReaderResource> m_resource;
};

} // namespace Microsoft::React
