// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <future>
#include <folly/dynamic.h>

namespace facebook {
namespace react {

enum class EndpointType
{
  Host,
  Sandbox
};

// Callback to notify if request to sandbox was sent successfully or not.
using SendRequestCallback = std::function<void(bool sent)>;

// For sandbox to handle JS call "Native Host -> JS Sandbox"
using JSCallRequestHandler = std::function<void(int64_t requestId, const std::string& method, folly::dynamic&& arguments)>;

// Ack from JS Sandbox to Native Host
using ReplyMessageHandler = std::function<void(int64_t replyid)>;

// JS Sandbox -> Native Host
using NativeModuleCallHandler = std::function<void(folly::dynamic&& calls)>;

// Send NativeModule call to host.
using SendNativeModuleCall = std::function<void(std::string&& message)>;

struct SandboxEndpoint
{
  virtual bool Start(EndpointType endpointType) = 0;
  virtual void Shutdown() = 0;

  // Send JS call request to sandbox.
  virtual void SendRequest(int64_t requestId,
                           const std::string& methodName,
                           const folly::dynamic& arguments,
                           SendRequestCallback&& callback) = 0;
  // Send message to host.
  virtual void Send(std::string&& message) = 0;

  virtual void RegisterJSCallRequestHandler(const JSCallRequestHandler& handler) = 0;
  virtual void RegisterReplyHandler(const ReplyMessageHandler& handler) = 0;
  virtual void RegisterNativeModuleCallHandler(const NativeModuleCallHandler& handler) = 0;
};

} }
