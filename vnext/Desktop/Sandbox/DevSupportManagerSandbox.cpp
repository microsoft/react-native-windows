// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <DevSupportManager.h>

#include <DevSettings.h>
#include <InstanceManager.h>

#include "Sandbox/SandboxJSExecutor.h"
#include "Sandbox/NamedPipeEndpoint.h"

#include <memory>

using namespace std;

namespace facebook { namespace react {
#if !defined(OSS_RN)

JSECreator DevSupportManager::LoadJavaScriptInSandboxMode(const DevSettings& settings)
{
  return [this, settings](shared_ptr<ExecutorDelegate> delegate, shared_ptr<MessageQueueThread> jsQueue)
  {
    auto sandboxJSE = make_unique<SandboxJSExecutor>(delegate, jsQueue);
    auto sandboxEndpoint = std::make_shared<NamedPipeEndpoint>(settings.sandboxPipeName);
    sandboxJSE->ConnectAsync(sandboxEndpoint, settings.errorCallback).wait();
    return sandboxJSE;
  };
}
#endif // OSS_RN
}}
