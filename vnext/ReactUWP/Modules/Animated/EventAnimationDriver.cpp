// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "EventAnimationDriver.h"
#include "NativeAnimatedNodeManager.h"
#include <jsi/jsi.h>

namespace react { namespace uwp {
  EventAnimationDriver::EventAnimationDriver(const folly::dynamic& eventPath, int64_t animatedValueTag, const std::shared_ptr<NativeAnimatedNodeManager>& manager)
    : m_animatedValueTag(animatedValueTag), m_manager(manager)
  {
    for (const auto& path : eventPath)
    {
      m_eventPath.push_back(path.getString());
    }
  }

  ValueAnimatedNode* EventAnimationDriver::AnimatedValue()
  {
    if (const auto manager = m_manager.lock())
    {
      if (manager->m_valueNodes.count(m_animatedValueTag))
      {
        return manager->m_valueNodes.at(m_animatedValueTag).get();
      }
    }
    return nullptr;
  }

} }
