// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "PropsAnimatedNode.h"
#include <ReactUWP\Modules\NativeUIManager.h>
#include <Views/ShadowNodeBase.h>
#include "StyleAnimatedNode.h"
#include "NativeAnimatedNodeManager.h"

namespace react { namespace uwp {
  PropsAnimatedNode::PropsAnimatedNode(int64_t tag, const folly::dynamic& config, const std::weak_ptr<IReactInstance>& instance, const std::shared_ptr<NativeAnimatedNodeManager>& manager) :
    AnimatedNode(tag, manager), m_instance(instance)
  {
    for (const auto& entry : config.find("props").dereference().second.items())
    {
      m_propMapping.insert({ entry.first.getString(), entry.second.getInt() });
    }
  }

  void PropsAnimatedNode::ConnectToView(int64_t viewTag)
  {
    if (m_connectedViewTag != s_connectedViewTagUnset)
    {
      throw new std::invalid_argument("Animated node " + std::to_string(m_tag) + " has already been attached to a view already exists.");
      return;
    }
    m_connectedViewTag = viewTag;
    UpdateView();
  }

  void PropsAnimatedNode::DisconnectFromView(int64_t viewTag)
  {
    if (m_connectedViewTag != viewTag)
    {
      throw new std::invalid_argument("Attempting to disconnect view that has not been connected with the given animated node.");
      return;
    }

    std::vector<int64_t> keys{};
    for (const auto& anim : m_expressionAnimations)
    {
      keys.push_back(anim.first);
    }
    for(const auto& key : keys)
    {
      DisposeCompletedAnimation(key);
    }

    if (m_centerPointAnimation)
    {
      if (const auto target = GetUIElement())
      {
        target.StopAnimation(m_centerPointAnimation);
      }
      m_centerPointAnimation = nullptr;
    }

    m_connectedViewTag = s_connectedViewTagUnset;
    m_needsCenterPointAnimation = false;
  }

  void PropsAnimatedNode::RestoreDefaultValues()
  {

  }

  void PropsAnimatedNode::UpdateView()
  {
    if (m_connectedViewTag == s_connectedViewTagUnset)
    {
      return;
    }

    if (const auto manager = std::shared_ptr<NativeAnimatedNodeManager>(m_manager))
    {
      for (const auto& entry : m_propMapping)
      {
        if (const auto& styleNode = &manager->GetStyleAnimatedNode(entry.second))
        {
          for (const auto& styleEntry : styleNode->GetMapping())
          {
            MakeAnimation(styleEntry.second, styleEntry.first);
          }
        }
        else if (const auto& valueNode = &manager->GetValueAnimatedNode(entry.second))
        {
          MakeAnimation(entry.second, StringToFacadeType(entry.first));
        }
      }
    }

    StartAnimations();
  }

  void PropsAnimatedNode::StartAnimations()
  {
    if (m_expressionAnimations.size())
    {
      if (const auto uiElement = GetUIElement())
      {
        uiElement.RotationAxis(m_rotationAxis);
        for (const auto anim : m_expressionAnimations)
        {
          uiElement.StartAnimation(anim.second);
        }
        if (m_needsCenterPointAnimation)
        {
          if (!m_centerPointAnimation)
          {
            m_centerPointAnimation = winrt::Window::Current().Compositor().CreateExpressionAnimation();
            m_centerPointAnimation.Target(L"CenterPoint");
            m_centerPointAnimation.SetReferenceParameter(L"centerPointPropertySet", GetShadowNodeBase()->EnsureTransformPS());
            m_centerPointAnimation.Expression(L"centerPointPropertySet.center");
          }

          uiElement.StartAnimation(m_centerPointAnimation);
        }
      }
      else
      {
        if (const auto instance = m_instance.lock())
        {
          if (const auto manager = m_manager.lock())
          {
            manager->AddDelayedPropsNode(Tag(), instance);
          }
        }
      }
    }
  }

  void PropsAnimatedNode::DisposeCompletedAnimation(int64_t valueTag)
  {
    if (m_expressionAnimations.count(valueTag))
    {
      if (const auto target = GetUIElement())
      {
        target.StopAnimation(m_expressionAnimations.at(valueTag));
      }
      m_expressionAnimations.erase(valueTag);
    }
  }

  void PropsAnimatedNode::MakeAnimation(int64_t valueNodeTag, FacadeType facadeType)
  {
    if (const auto manager = m_manager.lock())
    {
      if (const auto valueNode = &manager->GetValueAnimatedNode(valueNodeTag))
      {
        const auto animation = winrt::Window::Current().Compositor().CreateExpressionAnimation();
        animation.SetReferenceParameter(L"ValuePropSet", valueNode->PropertySet());
        animation.Expression(L"ValuePropSet.value + ValuePropSet.offset");
        switch (facadeType)
        {
        case FacadeType::Opacity:
          animation.Target(L"Opacity");
          break;
        case FacadeType::Rotation:
          m_rotationAxis = { 0,0,1 };
          animation.Expression(L"(ValuePropSet.value + ValuePropSet.offset) * 180 / PI");
          animation.Target(L"Rotation");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::RotationX:
          animation.Expression(L"(ValuePropSet.value + ValuePropSet.offset) * 180 / PI");
          m_rotationAxis = { 1,0,0 };
          animation.Target(L"Rotation");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::RotationY:
          animation.Expression(L"(ValuePropSet.value + ValuePropSet.offset) * 180 / PI");
          m_rotationAxis = { 0,1,0 };
          animation.Target(L"Rotation");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::Scale:
          animation.Expression(L"Vector3(ValuePropSet.value + ValuePropSet.offset, ValuePropSet.value + ValuePropSet.offset, 0)");
          animation.Target(L"Scale");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::ScaleX:
          animation.Target(L"Scale.X");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::ScaleY:
          animation.Target(L"Scale.Y");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::TranslateX:
          animation.Target(L"Translation.X");
          break;
        case FacadeType::TranslateY:
          animation.Target(L"Translation.Y");
          break;
        case FacadeType::Perspective:
          // TODO: implement perspective animations, tracked by issue #2680
          return;
        default:
          assert(false);
        }
        m_expressionAnimations.insert({ valueNode->Tag(), animation });
        valueNode->AddDependentPropsNode(Tag());
      }
    }
  }


  ShadowNodeBase* PropsAnimatedNode::GetShadowNodeBase()
  {
    if (const auto instance = m_instance.lock())
    {
      if (const auto nativeUIManagerHost = static_cast<NativeUIManager*>(instance->NativeUIManager())->getHost())
      {
        return static_cast<ShadowNodeBase*>(nativeUIManagerHost->FindShadowNodeForTag(m_connectedViewTag));
      }
    }
    return nullptr;
  }

  winrt::UIElement PropsAnimatedNode::GetUIElement()
  {
    if(const auto shadowNodeBase = GetShadowNodeBase())
    {
      if (const auto shadowNodeView = shadowNodeBase->GetView())
      {
        return shadowNodeView.as<winrt::UIElement>();
      }
    }
    return nullptr;
  }
} }
