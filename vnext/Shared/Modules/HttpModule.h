// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Networking/IHttpResource.h>

// React Native
#include <cxxreact/CxxModule.h>

// Windows API
#include <winrt/Windows.Foundation.h>

namespace Microsoft::React {

///
/// Realizes <c>NativeModules</c> projection.
/// <remarks>See src\Libraries\Network\RCTNetworking.windows.js</remarks>
///
class HttpModule : public facebook::xplat::module::CxxModule {
 public:
  enum MethodId { SendRequest = 0, AbortRequest = 1, ClearCookies = 2, LAST = ClearCookies };

  HttpModule(winrt::Windows::Foundation::IInspectable const &inspectableProperties) noexcept;

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

  std::shared_ptr<Networking::IHttpResource> m_resource;
  std::shared_ptr<ModuleHolder> m_holder;
  bool m_isResourceSetup{false};

  // Property bag high level reference.
  winrt::Windows::Foundation::IInspectable m_inspectableProperties;
};
} // namespace Microsoft::React
