// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NativeAnimatedNodesManager.h"
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
#include <Windows.Foundation.h>
#include <ReactUWP\Modules\NativeUIManager.h>

namespace react {
  namespace uwp {
    void NativeAnimatedNodesManager::CreateAnimatedNode(int64_t tag, const folly::dynamic& config, const std::weak_ptr<IReactInstance>& instance, const std::shared_ptr<NativeAnimatedNodesManager>& manager)
    {
      if (m_animationNodes.count(tag) > 0 ||
          m_propsNodes.count(tag) > 0 ||
          m_styleNodes.count(tag) > 0 ||
          m_valueNodes.count(tag) > 0)
      {
        throw new std::invalid_argument("AnimatedNode with tag " + std::to_string(tag) + " already exists.");
        return;
      }

      auto node = [config, tag, instance, manager, this]()
      {
        switch (auto type = AnimatedNodeTypeFromString(config.find("type").dereference().second.getString()))
        {
        case AnimatedNodeType::style:
        {
          m_styleNodes.emplace(tag, std::make_shared<StyleAnimatedNode>(StyleAnimatedNode(tag, config, manager)));
          return static_cast<std::shared_ptr<AnimatedNode>>(m_styleNodes.at(tag));
        }
        case AnimatedNodeType::value:
        {
          m_valueNodes.emplace(tag, std::make_shared<ValueAnimatedNode>(ValueAnimatedNode(tag, config, manager)));
          return static_cast<std::shared_ptr<AnimatedNode>>(m_valueNodes.at(tag));
        }
        case AnimatedNodeType::props:
        {
          m_propsNodes.emplace(tag, std::make_shared<PropsAnimatedNode>(PropsAnimatedNode(tag, config, instance, manager)));
          auto node = m_propsNodes.at(tag);
          node->UpdateView();
          return static_cast<std::shared_ptr<AnimatedNode>>(node);
        }
        case AnimatedNodeType::interpolation:
        {
          m_valueNodes.emplace(tag, std::make_shared<InterpolationAnimatedNode>(InterpolationAnimatedNode(tag, config, manager)));
          return static_cast<std::shared_ptr<AnimatedNode>>(m_valueNodes.at(tag));
        }
        case AnimatedNodeType::addition:
        {
          m_valueNodes.emplace(tag, std::make_shared<AdditionAnimatedNode>(AdditionAnimatedNode(tag, config, manager)));
          return static_cast<std::shared_ptr<AnimatedNode>>(m_valueNodes.at(tag));
        }
        case AnimatedNodeType::subtraction:
        {
          m_valueNodes.emplace(tag, std::make_shared<SubtractionAnimatedNode>(SubtractionAnimatedNode(tag, config, manager)));
          return static_cast<std::shared_ptr<AnimatedNode>>(m_valueNodes.at(tag));
        }
        case AnimatedNodeType::division:
        {
          m_valueNodes.emplace(tag, std::make_shared<DivisionAnimatedNode>(DivisionAnimatedNode(tag, config, manager)));
          return static_cast<std::shared_ptr<AnimatedNode>>(m_valueNodes.at(tag));
        }
        case AnimatedNodeType::multiplication:
        {
          m_valueNodes.emplace(tag, std::make_shared<MultiplicationAnimatedNode>(MultiplicationAnimatedNode(tag, config, manager)));
          return static_cast<std::shared_ptr<AnimatedNode>>(m_valueNodes.at(tag));
        }
        case AnimatedNodeType::modulus:
        {
          m_valueNodes.emplace(tag, std::make_shared<ModulusAnimatedNode>(ModulusAnimatedNode(tag, config, manager)));
          return static_cast<std::shared_ptr<AnimatedNode>>(m_valueNodes.at(tag));
        }
        case AnimatedNodeType::diffclamp:
        {
          m_valueNodes.emplace(tag, std::make_shared<DiffClampAnimatedNode>(DiffClampAnimatedNode(tag, config, manager)));
          return static_cast<std::shared_ptr<AnimatedNode>>(m_valueNodes.at(tag));
        }
        default:
        //case AnimatedNodeType::transform:
        {
          assert(type == transform);
          m_transformNodes.emplace(tag, std::make_shared<TransformAnimatedNode>(TransformAnimatedNode(tag, config, manager)));
          return static_cast<std::shared_ptr<AnimatedNode>>(m_transformNodes.at(tag));
        }
          //  return static_cast<AnimatedNode>(ValueAnimatedNode(tag, config));
          //case AnimatedNodeType::tracking:
          //  return static_cast<AnimatedNode>(ValueAnimatedNode(tag, config));
          //default:
          //  assert(false);
          //  //throw new InvalidOperationException(Invariant($"Unsupported node type: '{type}'"));
        }
      }();

      m_nodes.insert({ tag, node });

      if (auto inst = instance.lock())
      {
        static_cast<NativeUIManager*>(inst->NativeUIManager())->AddBatchCompletedCallback([this]() { ProcessDelayedPropsNodes(); });
      }
    }


    void NativeAnimatedNodesManager::ConnectAnimatedNodeToView(int64_t propsNodeTag, int64_t viewTag)
    {
      auto node = m_propsNodes.at(propsNodeTag);
      node->ConnectToView(viewTag);
    }

    void NativeAnimatedNodesManager::DisconnectAnimatedNodeToView(int64_t propsNodeTag, int64_t viewTag)
    {
      auto node = m_propsNodes.at(propsNodeTag);
      node->DisconnectFromView(viewTag);
    }

    void NativeAnimatedNodesManager::ConnectAnimatedNode(int64_t parentNodeTag, int64_t childNodeTag)
    {
      assert(m_nodes.count(parentNodeTag) && m_nodes.count(childNodeTag));
      m_nodes.at(parentNodeTag)->AddChild(m_nodes.at(childNodeTag));
    }

    void NativeAnimatedNodesManager::DisconnectAnimatedNode(int64_t parentNodeTag, int64_t childNodeTag)
    {
      assert(m_nodes.count(parentNodeTag));
      m_nodes.at(parentNodeTag)->RemoveChild(childNodeTag);
    }

    void NativeAnimatedNodesManager::StopAnimation(int64_t animationId)
    {
      if (m_activeAnimations.count(animationId))
      {
        if (auto animation = m_activeAnimations.at(animationId))
        {
          //Do some callback on animation ...

          m_activeAnimations.erase(animationId);
        }
      }
    }

    void NativeAnimatedNodesManager::StartAnimatingNode(int64_t animationId, int64_t animatedNodeTag, const folly::dynamic& animationConfig, Callback endCallback)
    {
      auto animation = [animationId, animatedNode = m_valueNodes.at(animatedNodeTag), endCallback, animationConfig]() {
        //return std::make_shared<AnimatedNode>(ValueAnimatedNode(tag, config));
        //return winrt::make_self<ValueAnimatedNode>(tag, config);
        switch (AnimationTypeFromString(animationConfig.find("type").dereference().second.getString()))
        {
        case AnimationType::decay:
          return static_cast<std::shared_ptr<AnimationDriver>>(std::make_shared<DecayAnimationDriver>(DecayAnimationDriver(animationId, animatedNode, endCallback, animationConfig)));
        case AnimationType::frames:
          return static_cast<std::shared_ptr<AnimationDriver>>(std::make_shared<FrameAnimationDriver>(FrameAnimationDriver(animationId, animatedNode, endCallback, animationConfig)));
        //case AnimationType::spring:
          //return static_cast<std::shared_ptr<AnimationDriver>>(std::make_shared<SpringAnimationDriver>(SpringAnimationDriver(tag, config)));
        }
        return static_cast<std::shared_ptr<AnimationDriver>>(std::make_shared<FrameAnimationDriver>(FrameAnimationDriver(animationId, animatedNode, endCallback, animationConfig)));
      }();
      m_activeAnimations.insert({ animationId, animation });
    }

    void NativeAnimatedNodesManager::DropAnimatedNode(int64_t tag)
    {
      m_nodes.erase(tag);
      m_valueNodes.erase(tag);
      m_propsNodes.erase(tag);
      m_styleNodes.erase(tag);
    }

    void NativeAnimatedNodesManager::SetAnimatedNodeValue(int64_t tag, double value)
    {
      if (auto valueNode = m_valueNodes.at(tag))
      {
        valueNode->RawValue(static_cast<float>(value));
      }
    }

    void NativeAnimatedNodesManager::SetAnimatedNodeOffset(int64_t tag, double offset)
    {
      if (auto valueNode = m_valueNodes.at(tag))
      {
        valueNode->Offset(static_cast<float>(offset));
      }
    }

    void NativeAnimatedNodesManager::FlattenAnimatedNodeOffset(int64_t tag)
    {
      if (auto valueNode = m_valueNodes.at(tag))
      {
        valueNode->FlattenOffset();
      }
    }

    void NativeAnimatedNodesManager::ExtractAnimatedNodeOffset(int64_t tag)
    {
      if (auto valueNode = m_valueNodes.at(tag))
      {
        valueNode->ExtractOffset();
      }
    }

    void NativeAnimatedNodesManager::AddAnimatedEventToView(int64_t viewTag, const std::string& eventName, const folly::dynamic& eventMapping)
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

    void NativeAnimatedNodesManager::RemoveAnimatedEventFromView(int64_t viewTag, const std::string& eventName, int64_t animatedValueTag)
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

    void NativeAnimatedNodesManager::ProcessDelayedPropsNodes()
    {
      for (auto tag : m_delayedPropsNodes)
      {
        m_propsNodes.at(tag)->StartAnimations();
      }
    }

    AnimatedNodeType NativeAnimatedNodesManager::AnimatedNodeTypeFromString(const std::string& string)
    {
      if (string == "style") return AnimatedNodeType::style;
      else if (string == "value") return AnimatedNodeType::value;
      else if (string == "props") return AnimatedNodeType::props;
      else if (string == "interpolation") return AnimatedNodeType::interpolation;
      else if (string == "addition") return AnimatedNodeType::addition;
      else if (string == "subtraction") return AnimatedNodeType::subtraction;
      else if (string == "division") return AnimatedNodeType::division;
      else if (string == "multiplication") return AnimatedNodeType::multiplication;
      else if (string == "modulus") return AnimatedNodeType::modulus;
      else if (string == "diffclamp") return AnimatedNodeType::diffclamp;
      else if (string == "transform") return AnimatedNodeType::transform;
      else
      {
        assert(string == "tracking");
        return AnimatedNodeType::tracking;
      }
    }

    AnimationType NativeAnimatedNodesManager::AnimationTypeFromString(const std::string& string)
    {
      if (string == "decay") return AnimationType::decay;
      else if (string == "frames") return AnimationType::frames;
      else
      {
        assert(string == "spring");
        return AnimationType::spring;
      }
    }

    FacadeType NativeAnimatedNodesManager::StringToFacadeType(const std::string& string)
    {
      if (string == "opacity") return FacadeType::opacity;
      if (string == "rotate") return FacadeType::rotation;
      if (string == "rotateX") return FacadeType::rotationX;
      if (string == "rotateY") return FacadeType::rotationY;
      if (string == "scale") return FacadeType::scale;
      if (string == "scaleX") return FacadeType::scaleX;
      if (string == "scaleY") return FacadeType::scaleY;
      if (string == "translateX") return FacadeType::translateX;
      if (string == "translateY") return FacadeType::translateY;

      assert(string == "perspective");
      return FacadeType::perspective;
    }
  }
}
