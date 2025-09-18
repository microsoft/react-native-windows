// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <codegen/NativeNetworkingIOSSpec.g.h>
#include <NativeModules.h>
#include <Networking/IHttpResource.h>

// Windows API
#include <winrt/Windows.Foundation.h>

namespace Microsoft::React {

REACT_MODULE(HttpTurboModule, L"Networking")
struct HttpTurboModule {
  using ModuleSpec = ReactNativeSpecs::NetworkingIOSSpec;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(SendRequest, L"sendRequest")
  void SendRequest(
      ReactNativeSpecs::NetworkingIOSSpec_sendRequest_query &&query,
      std::function<void(double)> const &callback) noexcept;

  REACT_METHOD(AbortRequest, L"abortRequest")
  void AbortRequest(double requestId) noexcept;

  REACT_METHOD(ClearCookies, L"clearCookies")
  void ClearCookies(std::function<void(bool)> const &callback) noexcept;

  REACT_METHOD(AddListener, L"addListener")
  void AddListener(std::string &&eventName) noexcept;

  REACT_METHOD(RemoveListeners, L"removeListeners")
  void RemoveListeners(double count) noexcept;

 private:
  std::shared_ptr<Networking::IHttpResource> m_resource;
  winrt::Microsoft::ReactNative::ReactContext m_context;
  int64_t m_requestId{0};
};

} // namespace Microsoft::React
