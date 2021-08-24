// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ShadowNodeBase.h>

namespace Microsoft::ReactNative {

static inline bool IsNodeType(ShadowNodeBase const *node, wchar_t const *name) {
  return std::wcscmp(node->GetViewManager()->GetName(), name) == 0;
}

static inline bool IsTextShadowNode(ShadowNodeBase const *node) {
  return IsNodeType(node, L"RCTText");
}

static inline bool IsVirtualTextShadowNode(ShadowNodeBase const *node) {
  return IsNodeType(node, L"RCTVirtualText");
}

static inline bool IsRawTextShadowNode(ShadowNodeBase const *node) {
  return IsNodeType(node, L"RCTRawText");
}

}; // namespace Microsoft::ReactNative
