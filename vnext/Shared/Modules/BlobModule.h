// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <codegen/NativeBlobModuleSpec.g.h>

#include <Networking/IBlobResource.h>

// Windows API
#include <winrt/base.h>

// Standard Library
#include <string>
#include <vector>

namespace Microsoft::React {

REACT_MODULE(BlobTurboModule, L"BlobModule")
struct BlobTurboModule {
  using ModuleSpec = ReactNativeSpecs::BlobModuleSpec;

  REACT_INIT(Initialize)
  void Initialize(
      winrt::Microsoft::ReactNative::ReactContext const &reactContext,
      facebook::jsi::Runtime &runtime) noexcept;

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
  void CreateFromParts(std::vector<winrt::Microsoft::ReactNative::JSValue> &&parts, std::string &&withId) noexcept;

  REACT_METHOD(Release, L"release")
  void Release(std::string &&blobId) noexcept;

 private:
  std::shared_ptr<Networking::IBlobResource> m_resource;
  winrt::Microsoft::ReactNative::ReactContext m_context;
};

} // namespace Microsoft::React
