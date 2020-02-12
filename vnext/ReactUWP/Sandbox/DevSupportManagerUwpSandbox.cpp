// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <IDevSupportManager.h>
#include <Modules/DevSupportManagerUwp.h>

#include <memory>

using namespace facebook::react;

namespace react {
namespace uwp {
#ifdef PATCH_RN
JSECreator DevSupportManager::LoadJavaScriptInSandboxMode(const DevSettings &settings) {
  // ASSERT: SandboxMode not yet implemented for UWP.
  assert(false);
  return nullptr;
}
#endif

} // namespace uwp
} // namespace react
