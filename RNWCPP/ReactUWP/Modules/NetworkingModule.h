// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// NetworkingModule
// provides the 'Networking' native module backing RCTNetworking.js

#pragma once

#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include <memory>
#include <vector>

namespace react { namespace uwp {


class NetworkingModule : public facebook::xplat::module::CxxModule
{
public:
  NetworkingModule();
  virtual ~NetworkingModule();

  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  auto getMethods() -> std::vector<Method> override;

  static const char* name;

  class NetworkingHelper;
private:
  std::shared_ptr<NetworkingHelper> m_networking;
};

} }
