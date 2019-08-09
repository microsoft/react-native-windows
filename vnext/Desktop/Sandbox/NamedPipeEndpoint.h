// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <folly/dynamic.h>
#include <future>
#include "Sandbox/SandboxEndpoint.h"

namespace facebook {
namespace react {

class NamedPipeEndpoint final : public SandboxEndpoint {
 public:
  NamedPipeEndpoint() = delete;
  NamedPipeEndpoint(const std::string &pipeName);
  ~NamedPipeEndpoint();
  NamedPipeEndpoint(NamedPipeEndpoint &other) = delete;
  NamedPipeEndpoint(NamedPipeEndpoint &&other) = delete;
  NamedPipeEndpoint &operator=(const NamedPipeEndpoint &other) = delete;
  NamedPipeEndpoint &operator=(NamedPipeEndpoint &&other) = delete;

  // ISandboxTransport methods
  bool Start(EndpointType endpointType) override;
  void Shutdown() override;

  void SendRequest(
      int64_t requestId,
      const std::string &methodName,
      const folly::dynamic &arguments,
      SendRequestCallback &&callback) override;
  void Send(std::string &&message) override;

  void RegisterJSCallRequestHandler(
      const JSCallRequestHandler &handler) override;
  void RegisterReplyHandler(const ReplyMessageHandler &handler) override;
  void RegisterNativeModuleCallHandler(
      const NativeModuleCallHandler &handler) override;

 private:
  struct Impl;
  std::unique_ptr<Impl> m_pimpl;
};

} // namespace react
} // namespace facebook
