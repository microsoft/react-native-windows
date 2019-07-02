// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AnimationDriver.h"

namespace react {
  namespace uwp {

    AnimationDriver::AnimationDriver(int64_t id, int64_t animatedValueTag, const Callback& endCallback, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodeManager>& manager) :
      m_id(id), m_animatedValueTag(animatedValueTag), m_endCallback(endCallback), m_config(config), m_manager(manager)
    {
      m_iterations = [iterations = config.find("iterations"), end = config.items().end()]() {
        if (iterations != end)
        {
          return static_cast<int64_t>(iterations.dereference().second.asDouble());
        }
        return static_cast<int64_t>(1);
      }();
    }

    AnimationDriver::~AnimationDriver()
    {
      if (m_scopedBatch)
        m_scopedBatch.Completed(m_scopedBatchCompletedToken);
    }

    void AnimationDriver::StartAnimation()
    {
      const auto [animation, scopedBatch] = MakeAnimation(m_config);

      const auto animatedValue = GetAnimatedValue();

      if (animatedValue)
      {
        animatedValue->PropertySet().StartAnimation(L"offset", animation);
        animatedValue->AddActiveAnimation(m_id);
      }
      scopedBatch.End();

      m_scopedBatchCompletedToken = scopedBatch.Completed([endCallback = m_endCallback, animatedValue, id = m_id](auto sender, auto)
      {
        if (endCallback)
        {
          endCallback(std::vector<folly::dynamic>{folly::dynamic::object("finished", true)});
        }
        if (animatedValue)
        {
          animatedValue->RemoveActiveAnimation(id);
        }
      });

      m_animation = animation;
      m_scopedBatch = scopedBatch;
    }

    void AnimationDriver::StopAnimation()
    {
      if (const auto animatedValue = GetAnimatedValue())
      {
        animatedValue->PropertySet().StopAnimation(L"offset");
        m_endCallback(std::vector<folly::dynamic>{folly::dynamic::object("finished", false)});
      }
    }

    ValueAnimatedNode* AnimationDriver::GetAnimatedValue()
    {
      if (auto manager = m_manager.lock())
      {
        return manager->GetValueAnimatedNode(m_animatedValueTag);
      }
      return static_cast<ValueAnimatedNode*>(nullptr);
    }
  }
}
