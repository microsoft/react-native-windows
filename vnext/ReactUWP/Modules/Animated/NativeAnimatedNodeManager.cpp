// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NativeAnimatedNodeManager.h"
#include "ValueAnimatedNode.h"
#include "StyleAnimatedNode.h"
#include "InterpolationAnimatedNode.h"
#include "AdditionAnimatedNode.h"
#include "MultiplicationAnimatedNode.h"
#include "DivisionAnimatedNode.h"
#include "SubtractionAnimatedNode.h"
#include "ModulusAnimatedNode.h"
#include "DiffClampAnimatedNode.h"
#include "TransformAnimatedNode.h"

#include "AnimationDriver.h"
#include "FrameAnimationDriver.h"
#include "DecayAnimationDriver.h"

#include "FacadeType.h"
#include "AnimatedNodeType.h"
#include "AnimationType.h"

#include <Windows.Foundation.h>
#include <ReactUWP\Modules\NativeUIManager.h>

namespace react {
  namespace uwp {
    void NativeAnimatedNodeManager::CreateAnimatedNode(int64_t tag, const folly::dynamic& config, const std::weak_ptr<IReactInstance>& instance, const std::shared_ptr<NativeAnimatedNodeManager>& manager)
    {
      if (m_animationNodes.count(tag) > 0 ||
          m_propsNodes.count(tag) > 0 ||
          m_styleNodes.count(tag) > 0 ||
          m_valueNodes.count(tag) > 0)
      {
        throw new std::invalid_argument("AnimatedNode with tag " + std::to_string(tag) + " already exists.");
        return;
      }

      switch (auto type = AnimatedNodeTypeFromString(config.find("type").dereference().second.getString()))
      {
        case AnimatedNodeType::Style:
        {
          m_styleNodes.emplace(tag, std::make_shared<StyleAnimatedNode>(StyleAnimatedNode(tag, config, manager)));
          break;
        }
        case AnimatedNodeType::Value:
        {
          m_valueNodes.emplace(tag, std::make_shared<ValueAnimatedNode>(ValueAnimatedNode(tag, config, manager)));
          break;
        }
        case AnimatedNodeType::Props:
        {
          m_propsNodes.emplace(tag, std::make_shared<PropsAnimatedNode>(PropsAnimatedNode(tag, config, instance, manager)));
          break;
        }
        case AnimatedNodeType::Interpolation:
        {
          m_valueNodes.emplace(tag, std::make_shared<InterpolationAnimatedNode>(InterpolationAnimatedNode(tag, config, manager)));
          break;
        }
        case AnimatedNodeType::Addition:
        {
          m_valueNodes.emplace(tag, std::make_shared<AdditionAnimatedNode>(AdditionAnimatedNode(tag, config, manager)));
          break;
        }
        case AnimatedNodeType::Subtraction:
        {
          m_valueNodes.emplace(tag, std::make_shared<SubtractionAnimatedNode>(SubtractionAnimatedNode(tag, config, manager)));
          break;
        }
        case AnimatedNodeType::Division:
        {
          m_valueNodes.emplace(tag, std::make_shared<DivisionAnimatedNode>(DivisionAnimatedNode(tag, config, manager)));
          break;
        }
        case AnimatedNodeType::Multiplication:
        {
          m_valueNodes.emplace(tag, std::make_shared<MultiplicationAnimatedNode>(MultiplicationAnimatedNode(tag, config, manager)));
          break;
        }
        case AnimatedNodeType::Modulus:
        {
          m_valueNodes.emplace(tag, std::make_shared<ModulusAnimatedNode>(ModulusAnimatedNode(tag, config, manager)));
          break;
        }
        case AnimatedNodeType::Diffclamp:
        {
          m_valueNodes.emplace(tag, std::make_shared<DiffClampAnimatedNode>(DiffClampAnimatedNode(tag, config, manager)));
          break;
        }
        case AnimatedNodeType::Transform:
        {
          m_transformNodes.emplace(tag, std::make_shared<TransformAnimatedNode>(TransformAnimatedNode(tag, config, manager)));
          break;
        }
        case AnimatedNodeType::Tracking:
        {
          break;
        }
        default:
        {
          assert(false);
          break;
        }
      }
    }

    void NativeAnimatedNodeManager::ConnectAnimatedNodeToView(int64_t propsNodeTag, int64_t viewTag)
    {
      auto node = m_propsNodes.at(propsNodeTag);
      node->ConnectToView(viewTag);
    }

    void NativeAnimatedNodeManager::DisconnectAnimatedNodeToView(int64_t propsNodeTag, int64_t viewTag)
    {
      auto node = m_propsNodes.at(propsNodeTag);
      node->DisconnectFromView(viewTag);
    }

    void NativeAnimatedNodeManager::ConnectAnimatedNode(int64_t parentNodeTag, int64_t childNodeTag)
    {
      if (auto parentNode = GetAnimatedNode(parentNodeTag))
      {
        if (auto childNode = GetAnimatedNode(childNodeTag))
        {
          parentNode->AddChild(childNode);
        }
      }
    }

    void NativeAnimatedNodeManager::DisconnectAnimatedNode(int64_t parentNodeTag, int64_t childNodeTag)
    {
      if (auto parentNode = GetAnimatedNode(parentNodeTag))
      {
        parentNode->RemoveChild(childNodeTag);
      }
    }

    void NativeAnimatedNodeManager::StopAnimation(int64_t animationId)
    {
      if (m_activeAnimations.count(animationId))
      {
        if (auto animation = m_activeAnimations.at(animationId))
        {
          animation->StopAnimation();
          m_activeAnimations.erase(animationId);
        }
      }
    }

    void NativeAnimatedNodeManager::StartAnimatingNode(int64_t animationId, int64_t animatedNodeTag, const folly::dynamic& animationConfig, const Callback& endCallback)
    {
      auto animation = [animationId, animatedNode = m_valueNodes.at(animatedNodeTag), endCallback, animationConfig]()
      {
        switch (AnimationTypeFromString(animationConfig.find("type").dereference().second.getString()))
        {
        case AnimationType::Decay:
          return static_cast<std::shared_ptr<AnimationDriver>>(std::make_shared<DecayAnimationDriver>(DecayAnimationDriver(animationId, animatedNode, endCallback, animationConfig)));
        case AnimationType::Frames:
          return static_cast<std::shared_ptr<AnimationDriver>>(std::make_shared<FrameAnimationDriver>(FrameAnimationDriver(animationId, animatedNode, endCallback, animationConfig)));
        case AnimationType::Spring:
          //TODO: implement spring animations tracked by issue #2681
          return static_cast<std::shared_ptr<AnimationDriver>>(nullptr);
        default:
          assert(false);
          return static_cast<std::shared_ptr<AnimationDriver>>(nullptr);
        }
      }();

      if (animation)
      {
        animation->StartAnimation();
        m_activeAnimations.insert({ animationId, animation });
      }
    }

    void NativeAnimatedNodeManager::DropAnimatedNode(int64_t tag)
    {
      m_valueNodes.erase(tag);
      m_propsNodes.erase(tag);
      m_styleNodes.erase(tag);
      m_transformNodes.erase(tag);
    }

    void NativeAnimatedNodeManager::SetAnimatedNodeValue(int64_t tag, double value)
    {
      if (auto valueNode = m_valueNodes.at(tag))
      {
        valueNode->RawValue(static_cast<float>(value));
      }
    }

    void NativeAnimatedNodeManager::SetAnimatedNodeOffset(int64_t tag, double offset)
    {
      if (auto valueNode = m_valueNodes.at(tag))
      {
        valueNode->Offset(static_cast<float>(offset));
      }
    }

    void NativeAnimatedNodeManager::FlattenAnimatedNodeOffset(int64_t tag)
    {
      if (auto valueNode = m_valueNodes.at(tag))
      {
        valueNode->FlattenOffset();
      }
    }

    void NativeAnimatedNodeManager::ExtractAnimatedNodeOffset(int64_t tag)
    {
      if (auto valueNode = m_valueNodes.at(tag))
      {
        valueNode->ExtractOffset();
      }
    }

    void NativeAnimatedNodeManager::AddAnimatedEventToView(int64_t viewTag, const std::string& eventName, const folly::dynamic& eventMapping)
    {
      auto valueNode = m_valueNodes.at(eventMapping.find("animatedValueTag").dereference().second.getInt());
      auto pathList = eventMapping.find("nativeEventPath").dereference().second.getString();


      auto key = std::make_tuple(viewTag, eventName);
      if (m_eventDrivers.count(key))
      {
        m_eventDrivers.at(key).emplace(std::make_shared<EventAnimationDriver>(EventAnimationDriver(pathList, valueNode)));
      }
      else
      {
        m_eventDrivers.emplace(key, std::unordered_set{ std::make_shared<EventAnimationDriver>(EventAnimationDriver(pathList, valueNode)) } );
      }
    }

    void NativeAnimatedNodeManager::RemoveAnimatedEventFromView(int64_t viewTag, const std::string& eventName, int64_t animatedValueTag)
    {
      auto key = std::make_tuple(viewTag, eventName);
      if (m_eventDrivers.count(key))
      {
        auto drivers = m_eventDrivers.at(key);

        for (auto driver : drivers)
        {
          if (auto value = driver->AnimatedValue().lock())
          {
            if(value->Tag() == animatedValueTag)
            {
              drivers.erase(driver);
            }
          }
        }

        if (!drivers.size())
        {
          m_eventDrivers.erase(key);
        }
      }
    }

    void NativeAnimatedNodeManager::ProcessDelayedPropsNodes()
    {
      // If StartAnimations fails we'll put the props nodes back into this queue to
      // try again when the next batch completes. Because of this we need to copy the
      // props to change into a local and clear the member before we begin.
      auto delayedPropsNodes = m_delayedPropsNodes;
      m_delayedPropsNodes.clear();
      for (auto tag : delayedPropsNodes)
      {
        m_propsNodes.at(tag)->StartAnimations();
      }
    }

    void NativeAnimatedNodeManager::AddDelayedPropsNode(int64_t propsNodeTag, const std::shared_ptr<IReactInstance>& instance)
    {
      m_delayedPropsNodes.push_back(propsNodeTag);
      if (m_delayedPropsNodes.size() <= 1)
      {
        static_cast<NativeUIManager*>(instance->NativeUIManager())->AddBatchCompletedCallback([this]() { ProcessDelayedPropsNodes(); });
      }
    }

    std::shared_ptr<AnimatedNode> NativeAnimatedNodeManager::GetAnimatedNode(int64_t tag)
    {
      if (m_valueNodes.count(tag))
      {
        return static_cast<std::shared_ptr<AnimatedNode>>(m_valueNodes.at(tag));
      }
      if (m_styleNodes.count(tag))
      {
        return static_cast<std::shared_ptr<AnimatedNode>>(m_styleNodes.at(tag));
      }
      if (m_propsNodes.count(tag))
      {
        return static_cast<std::shared_ptr<AnimatedNode>>(m_propsNodes.at(tag));
      }
      if (m_transformNodes.count(tag))
      {
        return static_cast<std::shared_ptr<AnimatedNode>>(m_transformNodes.at(tag));
      }
      return static_cast<std::shared_ptr<AnimatedNode>>(nullptr);
    }
  }
}
