// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

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
  std::weak_ptr<IBlobPersistor> m_weakBlobPersistor;
};

} // namespace Microsoft::React
