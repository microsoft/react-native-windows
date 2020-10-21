// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ScrollContentViewManager.h"

namespace Microsoft::ReactNative {

ScrollContentViewManager::ScrollContentViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *ScrollContentViewManager::GetName() const {
  return L"RCTScrollContentView";
}

} // namespace Microsoft::ReactNative
