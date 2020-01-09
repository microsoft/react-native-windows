// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <ReactUWPABI.h>
#include <TurboModuleUtils.h>
#include "../ABI/Instance_rt.h"

namespace react::uwp {

std::shared_ptr<facebook::react::Instance> __cdecl GetReactInstanceFromUwpInstance(
    const winrt::react::uwp::Instance &uwpInstance) {
  auto abiInstance = reinterpret_cast<ABI::react::uwp::Instance *>(winrt::get_abi(uwpInstance));
  auto reactInstance = abiInstance->getInstance();
  assert(reactInstance);
  auto fbInstance = reactInstance->GetInnerInstance();
  assert(fbInstance);
  return fbInstance;
}

} // namespace react::uwp
