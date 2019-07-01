// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <IHttpResource.h>
#include <cxxreact/CxxModule.h>

namespace facebook {
namespace react {

// NetworkingModule
// provides the 'Networking' native module backing RCTNetworking.js
class NetworkingModule : public facebook::xplat::module::CxxModule {
  static int64_t s_lastRequestId;
  std::unordered_map<int64_t, std::unique_ptr<IHttpResource>> m_resources;

  IHttpResource *GetResource(int64_t requestId) noexcept;
  void OnDataReceived(int64_t requestId, const std::string &data) noexcept;
  void OnRequestError(
      int64_t requestId,
      const std::string &error,
      bool isTimeOut) noexcept;
  void OnRequestSuccess(int64_t requestId) noexcept;
  void OnResponseReceived(
      int64_t requestId,
      int64_t statusCode,
      const folly::dynamic &headers,
      const std::string &url) noexcept;
  void SendEvent(std::string &&eventName, folly::dynamic &&parameters);

 public:
  NetworkingModule();

#pragma region CxxModule members

  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  std::vector<Method> getMethods() override;

#pragma endregion CxxModule members

  static const char *name;
};

} // namespace react
} // namespace facebook
