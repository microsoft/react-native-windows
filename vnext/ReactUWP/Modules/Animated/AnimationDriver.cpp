// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AnimationDriver.h"

namespace react {
  namespace uwp {

    AnimationDriver::AnimationDriver(int64_t id, const std::shared_ptr<ValueAnimatedNode>& animatedValue, const Callback& endCallback, const folly::dynamic& config) :
      m_id(id), m_animatedValue(animatedValue), m_endCallback(endCallback), m_config(config)
    {
      m_iterations = [iterations = config.find("iterations"), end = config.items().end()]() {
        if (iterations != end)
        {
          return iterations.dereference().second.getInt();
        }
        return static_cast<int64_t>(1);
      }();
    }

    void AnimationDriver::StartAnimation()
    {
      const auto [animation, scopedBatch] = MakeAnimation(m_config);

      if (m_animatedValue)
      {
        m_animatedValue->PropertySet().StartAnimation(L"offset", animation);
        m_animatedValue->AddActiveAnimation(m_id);
      }
      scopedBatch.End();

      scopedBatch.Completed([endCallback = m_endCallback, animatedValue = m_animatedValue, id = m_id](auto sender, auto)
      {
        if (endCallback)
        {
          endCallback(std::vector<folly::dynamic>{folly::dynamic::object("finished", "true")});
        }
        if (animatedValue)
        {
          animatedValue->RemoveActiveAnimation(id);
        }
      });

      m_animation = animation;
    }

    void AnimationDriver::StopAnimation()
    {
      m_animatedValue->PropertySet().StopAnimation(L"offset");
      m_endCallback(std::vector<folly::dynamic>{folly::dynamic::object("finished", "false")});
    }
  }
}
