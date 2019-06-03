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
            auto mapping = manager->m_styleNodes.at(entry.second)->GetMapping();
            for (auto entry : mapping)
            {
              auto valueNode = manager->m_valueNodes.at(entry.second);
              switch (entry.first)
              {
              case FacadeType::opacity:
                auto propSet = valueNode->PropertySet();
                auto animation = winrt::Window::Current().Compositor().CreateExpressionAnimation();
                animation.SetReferenceParameter(L"ValuePropSet", propSet);
                animation.Expression(L"ValuePropSet.value + ValuePropSet.Offset");
                animation.Target(L"opacity");
                uiElement.StartAnimation(animation);
                break;
              }
            }
          }
          else if (manager->m_valueNodes.count(entry.second))
          {

          }
        }
      }
    }
  }
}
