// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "IHttpResource.h"

// React Native
#include <cxxreact/CxxModule.h>

namespace Microsoft::React {

///
/// Realizes <c>NativeModules</c> projection.
/// <remarks>See src\Libraries\Network\RCTNetworkingWinShared.js</remarks>
///
class HttpModule : public facebook::xplat::module::CxxModule {
 public:
  enum MethodId { SendRequest = 0, AbortRequest = 1, ClearCookies = 2, LAST = ClearCookies };

  HttpModule() noexcept;

  ~HttpModule() noexcept override;

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
  struct ModuleHolder {
    HttpModule *Module{nullptr};
  };

  std::shared_ptr<IHttpResource> m_resource;
  std::shared_ptr<ModuleHolder> m_holder;
};
} // namespace Microsoft::React
