// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// React Native
#include <cxxreact/CxxModule.h>

// Folly
#include <folly/dynamic.h>

// Standard Library
#include <map>
#include <string>
#include <vector>

namespace Microsoft::React {

class FileReaderModule : public facebook::xplat::module::CxxModule {
 public:
  enum class MethodId {
    ReadAsDataURL = 0,
    ReadAsText = 1,
    SIZE = 2
  };

  FileReaderModule() noexcept;

  ~FileReaderModule() noexcept override;

  //struct SharedState {
  //  /// <summary>
  //  /// Keeps a raw reference to the module object to lazily retrieve the React Instance as needed.
  //  /// </summary>
  //  CxxModule *Module{nullptr};
  //};

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
  /// <summary>
  /// Keeps members that can be accessed threads other than this module's owner accessible.
  /// </summary>
  //std::shared_ptr<SharedState> m_sharedState;
};

} // namespace Microsoft::React
