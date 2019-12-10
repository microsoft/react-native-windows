// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <TurboModuleUtils.h>
#include <ReactUWPABI.h>
#include "../ABI/Instance_rt.h"

namespace react::uwp {

std::shared_ptr<facebook::react::Instance> GetFacebookReactInstance(const winrt::react::uwp::Instance &uwpInstance) {
  winrt::com_ptr<ABI::react::uwp::IInstance> abiIInstance;
  winrt::copy_to_abi(uwpInstance, *abiIInstance.put_void());
  assert(abiIInstance);
  ABI::react::uwp::Instance *abiInstance = static_cast<ABI::react::uwp::Instance *>(abiIInstance.get());
  auto reactInstance = abiInstance->getInstance();
  auto fbInstance = reactInstance->GetInnerInstance();
  return fbInstance;
}

} // namespace react::uwp
