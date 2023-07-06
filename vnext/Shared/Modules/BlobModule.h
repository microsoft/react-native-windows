// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <NativeBlobModuleSpec.g.h>

#include <Modules/IBlobPersistor.h>
#include <Modules/IRequestBodyHandler.h>
#include <Modules/IResponseHandler.h>
#include <Modules/IWebSocketModuleContentHandler.h>
#include <Networking/IBlobResource.h>

// React Native
#include <cxxreact/CxxModule.h>

// Boost Libraries
#include <boost/uuid/uuid_generators.hpp>

// Windows API
#include <winrt/base.h>

// Standard Library
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Microsoft::React {

REACT_MODULE(BlobTurboModule, L"BlobModule")
struct BlobTurboModule {
  using ModuleSpec = ReactNativeSpecs::BlobModuleSpec;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_GET_CONSTANTS(GetConstants)
  ReactNativeSpecs::BlobModuleSpec_Constants GetConstants() noexcept;

  REACT_METHOD(AddNetworkingHandler, L"addNetworkingHandler")
  void AddNetworkingHandler() noexcept;

  REACT_METHOD(AddWebSocketHandler, L"addWebSocketHandler")
  void AddWebSocketHandler(double id) noexcept;

  REACT_METHOD(RemoveWebSocketHandler, L"removeWebSocketHandler")
  void RemoveWebSocketHandler(double id) noexcept;

  REACT_METHOD(SendOverSocket, L"sendOverSocket")
  void SendOverSocket(winrt::Microsoft::ReactNative::JSValue &&blob, double socketID) noexcept;

  REACT_METHOD(CreateFromParts, L"createFromParts")
  void CreateFromParts(std::vector<winrt::Microsoft::ReactNative::JSValue> const &parts, std::string &&withId) noexcept;

  REACT_METHOD(Release, L"release")
  void Release(std::string &&blobId) noexcept;
};

class BlobModule : public facebook::xplat::module::CxxModule {
  std::shared_ptr<Networking::IBlobResource> m_resource;

 public:
  enum class MethodId {
    AddNetworkingHandler = 0,
    AddWebSocketHandler = 1,
    RemoveWebSocketHandler = 2,
    SendOverSocket = 3,
    CreateFromParts = 4,
    Release = 5,
    SIZE = 6
  };

  BlobModule(winrt::Windows::Foundation::IInspectable const &inspectableProperties) noexcept;

  ~BlobModule() noexcept override;

  struct SharedState {
    /// <summary>
    /// Keeps a raw reference to the module object to lazily retrieve the React Instance as needed.
    /// </summary>
    CxxModule *Module{nullptr};
  };

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
  std::shared_ptr<SharedState> m_sharedState;
};

} // namespace Microsoft::React
