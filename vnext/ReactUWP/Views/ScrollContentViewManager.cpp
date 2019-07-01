// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ScrollContentViewManager.h"

namespace react {
namespace uwp {

ScrollContentViewManager::ScrollContentViewManager(
    const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *ScrollContentViewManager::GetName() const {
  return "RCTScrollContentView";
}

} // namespace uwp
} // namespace react
