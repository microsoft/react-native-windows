// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>

namespace react::uwp {

class WebSocketModule : public facebook::xplat::module::CxxModule {
 public:
  WebSocketModule();
  virtual ~WebSocketModule();

  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  auto getMethods() -> std::vector<Method> override;

  static constexpr char const *Name = "WebSocketModule";

 private:
  class WebSocket;
  std::shared_ptr<WebSocket> m_webSocket;
};

} // namespace react::uwp
