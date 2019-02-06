// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include "IWebSocket.h"

namespace facebook {
namespace react {

class WebSocketModule : public facebook::xplat::module::CxxModule
{
public:
  enum MethodId
  {
    Connect    = 0,
    Close      = 1,
    Send       = 2,
    SendBinary = 3,
    Ping       = 4,
    SIZE       = 5
  };

  WebSocketModule();
  std::string getName();
  virtual std::map<std::string, folly::dynamic> getConstants();
  virtual std::vector<Method> getMethods();

private:
  void SendEvent(std::string&& eventName, folly::dynamic&& parameters);
  IWebSocket* GetWebSocket(int64_t id, std::string&& url = std::string());

  std::map<int64_t, std::unique_ptr<IWebSocket>> m_webSockets;
};

} } // facebook::react
