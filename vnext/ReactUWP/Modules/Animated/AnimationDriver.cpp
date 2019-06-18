// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AnimationDriver.h"

namespace react {
  namespace uwp {

    AnimationDriver::AnimationDriver(int64_t id, std::shared_ptr<ValueAnimatedNode> animatedValue, Callback endCallback) :
      m_id(id), m_animatedValue(animatedValue), m_endCallback(endCallback)
    {
    }
  }
}
