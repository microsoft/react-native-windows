// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <NativeFileReaderModuleSpec.g.h>

#include <IFileReaderResource.h>
#include <NativeModules.h>
#include "IBlobPersistor.h"

// React Native
#include <cxxreact/CxxModule.h>

// Folly
#include <folly/dynamic.h>

// Standard Library
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Microsoft::React {

class FileReaderModule : public facebook::xplat::module::CxxModule {
 public:
  enum class MethodId { ReadAsDataURL = 0, ReadAsText = 1, SIZE = 2 };

  FileReaderModule(std::weak_ptr<IBlobPersistor> weakBlobPersistor) noexcept;

  ~FileReaderModule() noexcept override;

#pragma region CxxModule

  /// <summary>
  /// <see cref="facebook::xplat::module::CxxModule::getName" />
  /// </summary>
  std::string getName() override;

  /// <summary>
  /// <see cref="facebook::xplat::module::CxxModule::getConstants" />
  /// </summary>
  std::map<std::string, folly::dynamic> getConstants() override;

  /// <summary>
  /// <see cref="facebook::xplat::module::CxxModule::getMethods" />
  /// </summary>
  /// <remarks>See See react-native/Libraries/WebSocket/WebSocket.js</remarks>
  std::vector<Method> getMethods() override;

#pragma endregion CxxModule

 private:
  std::shared_ptr<IFileReaderResource> m_resource;
};

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
