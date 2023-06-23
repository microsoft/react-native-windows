// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Networking/IHttpResource.h>
#include <NativeModules.h>
#include <NativeNetworkingWindowsSpec.g.h>


// React Native
#include <cxxreact/CxxModule.h>

// Windows API
#include <winrt/Windows.Foundation.h>

namespace Microsoft::React {

REACT_MODULE(HttpTurboModule, L"Networking")
struct HttpTurboModule {
  using ModuleSpec = ReactNativeSpecs::NetworkingWindowsSpec;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(SendRequest, L"sendRequest")
  void SendRequest(ReactNativeSpecs::NetworkingWindowsSpec_sendRequest_query&& query, std::function<void(double)> const& callback) noexcept;

  REACT_METHOD(AbortRequest, L"abortRequest")
  void AbortRequest(double requestId) noexcept;

  REACT_METHOD(ClearCookies, L"clearCookies")
  void ClearCookies(std::function<void(bool)> const& callback) noexcept;

  REACT_METHOD(AddListener, L"addListener")
  void AddListener(std::string&& eventName) noexcept;

  REACT_METHOD(RemoveListeners, L"removeListeners")
  void RemoveListeners(double count) noexcept;
};

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
