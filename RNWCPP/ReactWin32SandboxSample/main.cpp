// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// ReactWin32Sandbox.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <DevSettings.h>
#include "JSQueueThread.h"
#include "NativeQueueThread.h"
#include <Sandbox\NamedPipeEndpoint.h>
#include <cxxreact/Instance.h>

std::shared_ptr<facebook::react::InstanceWrapper> s_instance;

static void InitReactInstance(std::function<void(std::string&& message)> sendNativeModuleCall,
    std::string&& jsString,
    std::string&& configsString,
    std::string&& sourceUrl) {
  auto jsErrorCallback = [](const std::exception& exception) noexcept {
    OutputDebugStringA(exception.what());
  };

  auto nativeThread = std::make_shared<NativeMessageQueueThread>(jsErrorCallback);

  auto devSettings = std::make_unique<facebook::react::DevSettings>();
  devSettings->useWebDebugger = false;

  auto errorCallback = [nativeThread](std::string error) {
    OutputDebugStringA(error.c_str());
  };
  devSettings->errorCallback = errorCallback;

  s_instance = facebook::react::CreateReactInstanceForSandbox(
    std::move(jsString),
    std::move(configsString),
    std::move(sourceUrl),
    react::sandbox::CreateAndStartJSQueueThread(), // jsThread (JS execution)
    nativeThread,   // nativeThread (Native bridge & module execution)
    std::move(devSettings),
    std::move(sendNativeModuleCall));

  facebook::react::InitializeLogging([](facebook::react::RCTLogLevel logLevel, const char* message) {
#ifdef _DEBUG
    OutputDebugStringA(message);
    OutputDebugStringA("\n");
#endif
    //if (logLevel >= facebook::react::RCTLogLevel::Warning)
    //  assert(message);
    if (logLevel >= facebook::react::RCTLogLevel::Error) {
      throw std::runtime_error(message);
    }
  });
}

static HANDLE g_shutdown;

static BOOL WINAPI SignalHandler(DWORD type) {
  switch (type) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
      SetEvent(g_shutdown);
      return true;
  }

  return false;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: react-native-sandbox-win32.exe <PipeName>" << std::endl;;
    return 1;
  }

  auto endpoint = std::make_shared<facebook::react::NamedPipeEndpoint>(argv[1]);
  endpoint->RegisterJSCallRequestHandler([endpoint](int64_t requestId, const std::string& method, folly::dynamic&& arguments) {
    if (method == "callFunction") {
      auto jsCall = arguments;

      // (std::string&& module, std::string&& method, folly::dynamic&& params)
      s_instance->GetInstance()->callJSFunction(jsCall[0].asString(), jsCall[1].asString(), std::move(jsCall[2]));
    } else if (method == "invokeCallback") {
      auto jsCall = arguments;
      s_instance->invokeCallback(jsCall[0].asInt(), std::move(jsCall[1]));
    } else if (method == "prepareJSRuntime") {
      // Just reply back for now.
    } else if (method == "executeApplicationScript") {
      InitReactInstance(
        [&endpoint](std::string&& message) {
          // Note: We don't wait until send completed now.
          endpoint->Send(std::move(message));
        },
        std::move(arguments["script"].asString()),
        std::move(arguments["inject"]["__fbBatchedBridgeConfig"].asString()),
        std::move(arguments["url"].asString()));
    }

    folly::dynamic reply = folly::dynamic::object("replyID", requestId);
    endpoint->Send(std::move(folly::toJson(reply)));
  });

  endpoint->Start(facebook::react::EndpointType::Sandbox);

  g_shutdown = CreateEvent(nullptr, false, false, nullptr);
  SetConsoleCtrlHandler(SignalHandler, true);
  WaitForSingleObject(g_shutdown, INFINITE);

  return 0;
}
