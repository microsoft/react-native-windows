// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Modules/IHttpModuleProxy.h>
#include <Modules/IUriHandler.h>
#include <Networking/IHttpResource.h>

// React Native
#include <cxxreact/CxxModule.h>

// Windows API
#include <winrt/Windows.Foundation.h>

namespace Microsoft::React {

class HttpModuleProxy final : public IHttpModuleProxy
{
#pragma region IHttpModuleProxy

  void AddUriHandler(std::shared_ptr<IUriHandler> uriHandler) noexcept override;

  void AddRequestBodyHandler(std::shared_ptr<IRequestBodyHandler> requestBodyHandler) noexcept override;

  void AddResponseHandler(std::shared_ptr<IResponseHandler> responseHandler) noexcept override;

#pragma endregion IHttpModuleProxy
};

///
/// Realizes <c>NativeModules</c> projection.
/// <remarks>See src\Libraries\Network\RCTNetworkingWinShared.js</remarks>
///
class HttpModule : public facebook::xplat::module::CxxModule {
 public:
  enum MethodId { SendRequest = 0, AbortRequest = 1, ClearCookies = 2, LAST = ClearCookies };

  HttpModule(winrt::Windows::Foundation::IInspectable const &iProperties) noexcept;

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

  /// <summary>
  /// Exposes a subset of the module's methods.
  /// </summary>
  std::shared_ptr<IHttpModuleProxy> m_proxy;

  // Property bag high level reference.
  winrt::Windows::Foundation::IInspectable m_inspectableProperties;
};
} // namespace Microsoft::React
