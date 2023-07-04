// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <NativeBlobModuleSpec.g.h>

#include <Modules/IBlobPersistor.h>
#include <Modules/IRequestBodyHandler.h>
#include <Modules/IResponseHandler.h>
#include <Modules/IWebSocketModuleContentHandler.h>

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
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const& reactContext) noexcept {}

  REACT_GET_CONSTANTS(GetConstants)
  ReactNativeSpecs::BlobModuleSpec_Constants GetConstants() noexcept {
    return { "BLOB_URI_SCHEME", "BLOB_URI_HOST" };
  }

  REACT_METHOD(AddNetworkingHandler, L"addNetworkingHandler")
  void AddNetworkingHandler() noexcept {}

  REACT_METHOD(AddWebSocketHandler, L"addWebSocketHandler")
  void AddWebSocketHandler(double id) noexcept {}

  REACT_METHOD(RemoveWebSocketHandler, L"removeWebSocketHandler")
  void RemoveWebSocketHandler(double id) noexcept {}

  REACT_METHOD(SendOverSocket, L"sendOverSocket")
  void SendOverSocket(winrt::Microsoft::ReactNative::JSValue &&blob, double socketID) noexcept {}

  REACT_METHOD(CreateFromParts, L"createFromParts")
  void CreateFromParts(std::vector<winrt::Microsoft::ReactNative::JSValue> const &parts, std::string &&withId) noexcept {}

  REACT_METHOD(Release, L"release")
  void Release(std::string &&blobId) noexcept {}
};

class MemoryBlobPersistor final : public IBlobPersistor {
  std::unordered_map<std::string, std::vector<uint8_t>> m_blobs;
  std::mutex m_mutex;
  boost::uuids::random_generator m_guidGenerator;

 public:
#pragma region IBlobPersistor

  winrt::array_view<uint8_t const> ResolveMessage(std::string &&blobId, int64_t offset, int64_t size) override;

  void RemoveMessage(std::string &&blobId) noexcept override;

  void StoreMessage(std::vector<uint8_t> &&message, std::string &&blobId) noexcept override;

  std::string StoreMessage(std::vector<uint8_t> &&message) noexcept override;

#pragma endregion IBlobPersistor
};

class BlobWebSocketModuleContentHandler final : public IWebSocketModuleContentHandler {
  std::unordered_set<int64_t> m_socketIds;
  std::mutex m_mutex;
  std::shared_ptr<IBlobPersistor> m_blobPersistor;

 public:
  BlobWebSocketModuleContentHandler(std::shared_ptr<IBlobPersistor> blobPersistor) noexcept;

#pragma region IWebSocketModuleContentHandler

  void ProcessMessage(std::string &&message, folly::dynamic &params) override;

  void ProcessMessage(std::vector<uint8_t> &&message, folly::dynamic &params) override;

  void ProcessMessage(std::string &&message, winrt::Microsoft::ReactNative::JSValueObject &params) noexcept override;

  void ProcessMessage(std::vector<uint8_t> &&message, winrt::Microsoft::ReactNative::JSValueObject &params) noexcept
      override;

#pragma endregion IWebSocketModuleContentHandler

  void Register(int64_t socketID) noexcept;

  void Unregister(int64_t socketID) noexcept;
};

class BlobModuleRequestBodyHandler final : public IRequestBodyHandler {
  std::shared_ptr<IBlobPersistor> m_blobPersistor;

 public:
  BlobModuleRequestBodyHandler(std::shared_ptr<IBlobPersistor> blobPersistor) noexcept;

#pragma region IRequestBodyHandler

  bool Supports(winrt::Microsoft::ReactNative::JSValueObject &data) override;

  winrt::Microsoft::ReactNative::JSValueObject ToRequestBody(
      winrt::Microsoft::ReactNative::JSValueObject &data,
      std::string &contentType) override;

#pragma endregion IRequestBodyHandler
};

class BlobModuleResponseHandler final : public IResponseHandler {
  std::shared_ptr<IBlobPersistor> m_blobPersistor;

 public:
  BlobModuleResponseHandler(std::shared_ptr<IBlobPersistor> blobPersistor) noexcept;

#pragma region IResponseHandler

  bool Supports(std::string &responseType) override;

  winrt::Microsoft::ReactNative::JSValueObject ToResponseData(std::vector<uint8_t> &&content) override;

#pragma endregion IResponseHandler
};

class BlobModule : public facebook::xplat::module::CxxModule {
  std::shared_ptr<MemoryBlobPersistor> m_blobPersistor;
  std::shared_ptr<BlobWebSocketModuleContentHandler> m_contentHandler;
  std::shared_ptr<BlobModuleRequestBodyHandler> m_requestBodyHandler;
  std::shared_ptr<BlobModuleResponseHandler> m_responseHandler;

  // Property bag high level reference.
  winrt::Windows::Foundation::IInspectable m_inspectableProperties;

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
