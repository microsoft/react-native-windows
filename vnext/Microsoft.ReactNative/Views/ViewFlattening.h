// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <INativeUIManager.h>
#include "ShadowNodeBase.h"

namespace winrt::Microsoft::ReactNative {
struct JSValueObject;
}

namespace Microsoft::ReactNative::ViewFlattening {

struct IViewFlatteningHost : INativeUIManagerHost {
  virtual void UnflattenLayout(int64_t tag) = 0;
  virtual bool IsViewFlatteningEnabled() = 0;
};

void AddView(IViewFlatteningHost *host, ShadowNodeBase *parent, ShadowNodeBase *child, int64_t index);
bool IsLayoutOnly(ShadowNodeBase *node, winrt::Microsoft::ReactNative::JSValueObject const &props);
void RemoveAllChildren(IViewFlatteningHost *host, ShadowNodeBase *node);
void RemoveChildAt(IViewFlatteningHost *host, ShadowNodeBase *parent, ShadowNodeBase *child, int64_t index);
void UnflattenNode(IViewFlatteningHost *host, ShadowNodeBase *node);

} // namespace Microsoft::ReactNative::ViewFlattening
