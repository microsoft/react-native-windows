// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "SandboxBridge.h"

#include <folly/Memory.h>
#include <folly/json.h>

#include <cxxreact/Instance.h>
#include <cxxreact/MethodCall.h>
#include <cxxreact/ModuleRegistry.h>

namespace facebook {
namespace react {

#if !defined(OSS_RN)

// This should match with same #defines in MethodCall.cpp. If native method call
// JSON format changes then this needs to be updated with parsing code below.
#define REQUEST_MODULE_IDS 0
#define REQUEST_METHOD_IDS 1
#define REQUEST_PARAMSS 2
#define REQUEST_CALLID 3

// Added callId to each method call because callId is not necessarily
// sequentially incremented. One batch can include some calls to native modules
// in sandbox and some calls can be in host side. In this case, we basically
// split the original batch in to two. (one for sandbox and th other for host)
#define SANDBOX_REQUEST_MODULE_IDS 0
#define SANDBOX_REQUEST_METHOD_IDS 1
#define SANDBOX_REQUEST_PARAMSS 2
#define SANDBOX_REQUEST_CALLIDS 3
#define SANDBOX_REQUEST_ISENDOFBATCH \
  4 // This is only used by sandbox --(IPC)--> host native module calls.

// Used by host process.
static std::vector<MethodCall> sandboxHostParseMethodCalls(
    folly::dynamic &&jsonData,
    bool &isEndOfBatch) {
  if (jsonData.isNull()) {
    return {};
  }

  if (!jsonData.isArray()) {
    throw std::invalid_argument(folly::to<std::string>(
        "Did not get valid calls back from JS: ", jsonData.typeName()));
  }

  if (jsonData.size() < SANDBOX_REQUEST_PARAMSS + 1) {
    throw std::invalid_argument(folly::to<std::string>(
        "Did not get valid calls back from JS: size == ", jsonData.size()));
  }

  auto &moduleIds = jsonData[SANDBOX_REQUEST_MODULE_IDS];
  auto &methodIds = jsonData[SANDBOX_REQUEST_METHOD_IDS];
  auto &params = jsonData[SANDBOX_REQUEST_PARAMSS];
  auto &callIds = jsonData[SANDBOX_REQUEST_CALLIDS];
  isEndOfBatch = false;

  if (!moduleIds.isArray() || !methodIds.isArray() || !params.isArray()) {
    throw std::invalid_argument(folly::to<std::string>(
        "Did not get valid calls back from JS: ", folly::toJson(jsonData)));
  }

  if (moduleIds.size() != methodIds.size() ||
      moduleIds.size() != params.size() || moduleIds.size() != callIds.size()) {
    throw std::invalid_argument(folly::to<std::string>(
        "Did not get valid calls back from JS: ", folly::toJson(jsonData)));
  }

  if (jsonData.size() > SANDBOX_REQUEST_ISENDOFBATCH) {
    if (!jsonData[SANDBOX_REQUEST_ISENDOFBATCH].isBool()) {
      throw std::invalid_argument(folly::to<std::string>(
          "Did not get valid calls back from JS: %s", folly::toJson(jsonData)));
    } else {
      isEndOfBatch =
          static_cast<bool>(jsonData[SANDBOX_REQUEST_ISENDOFBATCH].getBool());
    }
  }

  std::vector<MethodCall> methodCalls;
  for (size_t i = 0; i < moduleIds.size(); i++) {
    if (!params[i].isArray()) {
      throw std::invalid_argument(
          folly::to<std::string>("Call argument isn't an array"));
    }

    methodCalls.emplace_back(
        static_cast<int>(moduleIds[i].getInt()),
        static_cast<int>(methodIds[i].getInt()),
        std::move(params[i]),
        static_cast<int>(callIds[i].getInt()));
  }

  return methodCalls;
}

// Return a pair of vectors: First is array of local calls and second is array
// of remote calls. (Used by sandbox process)
static std::pair<std::vector<MethodCall>, std::vector<MethodCall>>
sandboxParseMethodCalls(
    folly::dynamic &&jsonData,
    int64_t maxInprocModuleCount) {
  if (jsonData.isNull()) {
    return {};
  }

  if (!jsonData.isArray()) {
    throw std::invalid_argument(folly::to<std::string>(
        "Did not get valid calls back from JS: ", jsonData.typeName()));
  }

  if (jsonData.size() < REQUEST_PARAMSS + 1) {
    throw std::invalid_argument(folly::to<std::string>(
        "Did not get valid calls back from JS: size == ", jsonData.size()));
  }

  auto &moduleIds = jsonData[REQUEST_MODULE_IDS];
  auto &methodIds = jsonData[REQUEST_METHOD_IDS];
  auto &params = jsonData[REQUEST_PARAMSS];
  int callId = -1;

  if (!moduleIds.isArray() || !methodIds.isArray() || !params.isArray()) {
    throw std::invalid_argument(folly::to<std::string>(
        "Did not get valid calls back from JS: ", folly::toJson(jsonData)));
  }

  if (moduleIds.size() != methodIds.size() ||
      moduleIds.size() != params.size()) {
    throw std::invalid_argument(folly::to<std::string>(
        "Did not get valid calls back from JS: ", folly::toJson(jsonData)));
  }

  if (jsonData.size() > REQUEST_CALLID) {
    if (!jsonData[REQUEST_CALLID].isInt()) {
      throw std::invalid_argument(folly::to<std::string>(
          "Did not get valid calls back from JS: %s", folly::toJson(jsonData)));
    } else {
      callId = static_cast<int>(jsonData[REQUEST_CALLID].getInt());
    }
  }

  std::vector<MethodCall> inprocMethodCalls;
  std::vector<MethodCall> remoteMethodCalls;

  for (size_t i = 0; i < moduleIds.size(); i++) {
    if (!params[i].isArray()) {
      throw std::invalid_argument(
          folly::to<std::string>("Call argument isn't an array"));
    }

    auto moduleId = moduleIds[i].getInt();
    if (moduleId < maxInprocModuleCount) {
      inprocMethodCalls.emplace_back(
          static_cast<int>(moduleId),
          static_cast<int>(methodIds[i].getInt()),
          std::move(params[i]),
          callId);
    } else {
      remoteMethodCalls.emplace_back(
          static_cast<int>(moduleId),
          static_cast<int>(methodIds[i].getInt()),
          std::move(params[i]),
          callId);
    }

    // only increment callid if contains valid callid as callid is optional
    // callId looks like added for batching. First, I don't see any batching
    // code in JS side so this maybe not implemented feature. Second, not sure
    // how incrementing callId for each call in the same batch would work with
    // callbacks. [cslim]
    callId += (callId != -1) ? 1 : 0;
  }

  return std::pair<std::vector<MethodCall>, std::vector<MethodCall>>(
      inprocMethodCalls, remoteMethodCalls);
}

SandboxHostNativeBridge::SandboxHostNativeBridge(
    std::shared_ptr<ModuleRegistry> registry,
    std::shared_ptr<InstanceCallback> callback)
    : m_registry(registry), m_callback(callback) {}

std::shared_ptr<ModuleRegistry> SandboxHostNativeBridge::getModuleRegistry() {
  return m_registry;
}

void SandboxHostNativeBridge::callNativeModules(
    JSExecutor &executor,
    folly::dynamic &&calls,
    bool isEndOfBatch) {
  CHECK(m_registry || calls.empty())
      << "native module calls cannot be completed with no native modules";
  m_batchHadNativeModuleCalls = m_batchHadNativeModuleCalls || !calls.empty();

  // An exception anywhere in here stops processing of the batch.  This
  // was the behavior of the Android bridge, and since exception handling
  // terminates the whole bridge, there's not much point in continuing.
  for (auto &call :
       sandboxHostParseMethodCalls(std::move(calls), isEndOfBatch)) {
    m_registry->callNativeMethod(
        call.moduleId, call.methodId, std::move(call.arguments), call.callId);
  }
  if (isEndOfBatch) {
    // onBatchComplete will be called on the native (module) queue, but
    // decrementPendingJSCalls will be called sync. Be aware that the bridge may
    // still be processing native calls when the bridge idle signaler fires.
    if (m_batchHadNativeModuleCalls) {
      m_callback->onBatchComplete();
      m_batchHadNativeModuleCalls = false;
    }
    m_callback->decrementPendingJSCalls();
  }
}

bool SandboxHostNativeBridge::isBatchActive() {
  return m_batchHadNativeModuleCalls;
}

MethodCallResult SandboxHostNativeBridge::callSerializableNativeHook(
    JSExecutor &executor,
    unsigned int moduleId,
    unsigned int methodId,
    folly::dynamic &&args) {
  // Sync call is NYI. Throw exception for now and hopefully we don't have sync
  // methods.
  throw new std::exception("Sync call is not implemented.");
  // return m_registry->callSerializableNativeHook(moduleId, methodId,
  // std::move(args));
}

SandboxJsToNativeBridge::SandboxJsToNativeBridge(
    std::shared_ptr<ModuleRegistry> registry,
    std::shared_ptr<InstanceCallback> callback,
    std::function<void(std::string &&message)> remoteCallFunc)
    : m_registry(registry),
      m_callback(callback),
      m_remoteCallFunc(remoteCallFunc) {}

std::shared_ptr<ModuleRegistry> SandboxJsToNativeBridge::getModuleRegistry() {
  return m_registry;
}

void SandboxJsToNativeBridge::callNativeModules(
    JSExecutor &executor,
    folly::dynamic &&calls,
    bool isEndOfBatch) {
  CHECK(m_registry || calls.empty())
      << "native module calls cannot be completed with no native modules";
  m_batchHadNativeModuleCalls = m_batchHadNativeModuleCalls || !calls.empty();

  // An exception anywhere in here stops processing of the batch.  This ^ was
  // the behavior of the Android bridge, and since exception handling terminates
  // the whole bridge, there's not much point in continuing.

  // Process local (in-proc) NM calls.
  auto moduleSize = static_cast<int64_t>(m_registry->GetModuleSize());
  assert(moduleSize > 0);
  auto callPair = sandboxParseMethodCalls(
      std::move(calls), static_cast<int64_t>(moduleSize));
  for (auto &call : callPair.first) {
    m_registry->callNativeMethod(
        call.moduleId, call.methodId, std::move(call.arguments), call.callId);
  }

  // Process remote NM calls.
  if (callPair.second.size() > 0) {
    folly::dynamic moduleIds = folly::dynamic::array();
    folly::dynamic methodIds = folly::dynamic::array();
    folly::dynamic arguments = folly::dynamic::array();
    folly::dynamic callIds = folly::dynamic::array();

    for (auto &remoteCall : callPair.second) {
      moduleIds.push_back(remoteCall.moduleId);
      methodIds.push_back(remoteCall.methodId);
      arguments.push_back(remoteCall.arguments);
      callIds.push_back(remoteCall.callId);
    }

    // Remote NativeModule calls JSON format
    // [[moduleIds], [methodIds], [arguments], [callIds], isEndOfBatch]
    // Example: [[8,3],[3,1],[["sending email...0"],[98,....]],[180,181],false]
    folly::dynamic remoteCalls = folly::dynamic::array(
        moduleIds, methodIds, arguments, callIds, isEndOfBatch);
    m_remoteCallFunc(std::move(folly::toJson(remoteCalls)));
  }

  if (isEndOfBatch) {
    // onBatchComplete will be called on the native (module) queue, but
    // decrementPendingJSCalls will be called sync. Be aware that the bridge may
    // still be processing native calls when the bridge idle signaler fires.
    if (m_batchHadNativeModuleCalls) {
      m_callback->onBatchComplete();
      m_batchHadNativeModuleCalls = false;
    }
    m_callback->decrementPendingJSCalls();
  }
}

bool SandboxJsToNativeBridge::isBatchActive() {
  return m_batchHadNativeModuleCalls;
}

MethodCallResult SandboxJsToNativeBridge::callSerializableNativeHook(
    JSExecutor &executor,
    unsigned int moduleId,
    unsigned int methodId,
    folly::dynamic &&args) {
  // Sync call is NYI. Throw exception for now and hopefully we don't have sync
  // methods.
  throw new std::exception("Sync call is not implemented.");
  // return m_registry->callSerializableNativeHook(moduleId, methodId,
  // std::move(args));
}

#endif // OSS_RN

} // namespace react
} // namespace facebook
