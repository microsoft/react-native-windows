// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <dispatchQueue/dispatchQueue.h>

namespace Microsoft::ReactNative {

class ReactInspectorThread {
 public:
  static Mso::DispatchQueue &Instance() {
    static Mso::DispatchQueue queue = Mso::DispatchQueue::MakeSerialQueue();
    return queue;
  }
};

} // namespace Microsoft::ReactNative
