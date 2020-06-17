// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <DevSupportManager.h>

#include <DevSettings.h>
#include <InstanceManager.h>

#include "Sandbox/NamedPipeEndpoint.h"
#include "Sandbox/SandboxJSExecutor.h"

#include <memory>

using namespace std;

namespace Microsoft::React {
#ifdef PATCH_RN

facebook::react::JSECreator Microsoft::React::DevSupportManager::LoadJavaScriptInSandboxMode(
    const facebook::react::DevSettings &settings) {
  return [this, settings](
             shared_ptr<facebook::react::ExecutorDelegate> delegate,
             shared_ptr<facebook::react::MessageQueueThread> jsQueue) {
    auto sandboxJSE = make_unique<facebook::react::SandboxJSExecutor>(delegate, jsQueue);
    auto sandboxEndpoint = std::make_shared<facebook::react::NamedPipeEndpoint>(settings.sandboxPipeName);
    sandboxJSE->ConnectAsync(sandboxEndpoint, settings.errorCallback).wait();
    return sandboxJSE;
  };
}
#endif // PATCH_RN
} // namespace Microsoft::React
