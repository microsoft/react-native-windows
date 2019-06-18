#include "pch.h"
#include "PropsAnimatedNode.h"
#include <ReactUWP\Modules\NativeUIManager.h>
#include <Views/ShadowNodeBase.h>
#include "StyleAnimatedNode.h"

namespace react {
  namespace uwp {
    PropsAnimatedNode::PropsAnimatedNode(int64_t tag, const folly::dynamic& config, const std::weak_ptr<IReactInstance>& instance, const std::shared_ptr<NativeAnimatedNodesManager>& manager) :
      AnimatedNode(tag), m_instance(instance), m_manager(manager)
    {
      for (auto entry : config.find("props").dereference().second.items())
      {
        m_propMapping.insert({ entry.first.getString(), entry.second.getInt() });
      }
    }

    void PropsAnimatedNode::ConnectToView(int64_t viewTag)
    {
      if (m_connectedViewTag != -1)
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
      m_connectedViewTag = -1;
    }

    void PropsAnimatedNode::RestoreDefaultValues()
    {

    }

    void PropsAnimatedNode::UpdateView()
    {
      if (m_connectedViewTag == -1)
      {
        return;
      }

      auto const uiElement = [instance = m_instance.lock(), connectedViewTag = m_connectedViewTag]()
      {
        if (instance)
        {
          auto pNativeUIManagerHost = static_cast<NativeUIManager*>(instance->NativeUIManager())->getHost();
          auto pShadowNodeChild = static_cast<ShadowNodeBase*>(pNativeUIManagerHost->FindShadowNodeForTag(connectedViewTag));
          return pShadowNodeChild->GetView().as<winrt::UIElement>();
        }
        return static_cast<winrt::UIElement>(nullptr);
      }();

      if (const auto manager = m_manager.lock())
      {
        for (auto entry : m_propMapping)
        {
          if (manager->m_styleNodes.count(entry.second))
          {
            for (auto entry : manager->m_styleNodes.at(entry.second)->GetMapping())
            {
              auto valueNode = manager->m_valueNodes.at(entry.second);

              const auto animation = [valueNode, entry, uiElement]()
              {
                auto animation = winrt::Window::Current().Compositor().CreateExpressionAnimation();
                animation.SetReferenceParameter(L"ValuePropSet", valueNode->PropertySet());
                animation.Expression(L"ValuePropSet.value + ValuePropSet.offset");
                switch (entry.first)
                {
                case FacadeType::opacity:
                  animation.Target(L"Opacity");
                  break;
                case FacadeType::rotation:
                  animation.Expression(L"(ValuePropSet.value + ValuePropSet.offset) * 180 / PI");
                  animation.Target(L"Rotation");
                  break;
                case FacadeType::rotationX:
                  animation.Expression(L"(ValuePropSet.value + ValuePropSet.offset) * 180 / PI");
                  uiElement.RotationAxis({ 1, 0, 0 });
                  animation.Target(L"Rotation");
                  break;
                case FacadeType::rotationY:
                  animation.Expression(L"(ValuePropSet.value + ValuePropSet.offset) * 180 / PI");
                  uiElement.RotationAxis({ 0, 1, 0 });
                  animation.Target(L"Rotation");
                  break;
                case FacadeType::scale:
                  animation.Expression(L"Vector3(ValuePropSet.value + ValuePropSet.offset, ValuePropSet.value + ValuePropSet.offset, 0)");
                  animation.Target(L"Scale");
                  break;
                case FacadeType::scaleX:
                  animation.Target(L"Scale.X");
                  break;
                case FacadeType::scaleY:
                  animation.Target(L"Scale.Y");
                  break;
                case FacadeType::translateX:
                  animation.Target(L"Translation.X");
                  break;
                case FacadeType::translateY:
                  animation.Target(L"Translation.Y");
                  break;
                case FacadeType::perspective:
                  // We don't have an easy way of doing this..
                  return static_cast<winrt::ExpressionAnimation>(nullptr);
                default:
                  assert(false);
                }
                return animation;
              }();

              if (animation)
              {
                m_expressionAnimations.insert({ valueNode->Tag(), { animation, uiElement } });
                uiElement.StartAnimation(animation);
              }
            }
          }
          else if (manager->m_valueNodes.count(entry.second))
          {

          }
        }
      }
    }

    void PropsAnimatedNode::DisposeCompletedAnimation(int64_t valueTag)
    {
      auto animationAndTarget = m_expressionAnimations.at(valueTag);
      animationAndTarget.target.StopAnimation(animationAndTarget.animation);
      m_expressionAnimations.erase(valueTag);
    }
  }
}
