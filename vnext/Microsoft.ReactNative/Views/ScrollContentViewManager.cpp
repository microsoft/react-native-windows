// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ScrollContentViewManager.h"

namespace react::uwp {

ScrollContentViewManager::ScrollContentViewManager(const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *ScrollContentViewManager::GetName() const {
  return "RCTScrollContentView";
}

} // namespace react::uwp
