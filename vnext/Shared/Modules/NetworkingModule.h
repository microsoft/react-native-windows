// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// NetworkingModule
// provides the 'Networking' native module backing RCTNetworking.js

#pragma once

#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include <memory>
#include <vector>

namespace Microsoft::React {

class NetworkingModule : public facebook::xplat::module::CxxModule {
 public:
  NetworkingModule();
  virtual ~NetworkingModule();

  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  auto getMethods() -> std::vector<Method> override;

  static constexpr char const *Name = "Networking";

  class NetworkingHelper;

 private:
  std::shared_ptr<NetworkingHelper> m_networking;
};

} // namespace Microsoft::React
