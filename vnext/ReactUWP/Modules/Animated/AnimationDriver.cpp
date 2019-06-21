// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AnimationDriver.h"

namespace react {
  namespace uwp {

    AnimationDriver::AnimationDriver(int64_t id, const std::shared_ptr<ValueAnimatedNode>& animatedValue, const Callback& endCallback) :
      m_id(id), m_animatedValue(animatedValue), m_endCallback(endCallback)
    {
    }
  }
}
