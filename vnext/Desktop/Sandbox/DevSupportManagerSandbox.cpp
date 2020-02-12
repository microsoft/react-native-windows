// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <DevSupportManager.h>

#include <DevSettings.h>
#include <InstanceManager.h>

#include "Sandbox/NamedPipeEndpoint.h"
#include "Sandbox/SandboxJSExecutor.h"

#include <memory>

using namespace std;

namespace facebook {
namespace react {
#ifdef PATCH_RN

JSECreator DevSupportManager::LoadJavaScriptInSandboxMode(const DevSettings &settings) {
  return [this, settings](shared_ptr<ExecutorDelegate> delegate, shared_ptr<MessageQueueThread> jsQueue) {
    auto sandboxJSE = make_unique<SandboxJSExecutor>(delegate, jsQueue);
    auto sandboxEndpoint = std::make_shared<NamedPipeEndpoint>(settings.sandboxPipeName);
    sandboxJSE->ConnectAsync(sandboxEndpoint, settings.errorCallback).wait();
    return sandboxJSE;
  };
}
#endif // PATCH_RN
} // namespace react
} // namespace facebook
