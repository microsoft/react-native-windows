#include "pch.h"
#include "DiffClampAnimatedNode.h"

namespace react {
  namespace uwp {
    DiffClampAnimatedNode::DiffClampAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodesManager>& manager) : ValueAnimatedNode(tag, config, manager)
    {
      m_inputNodeTag = config.find("input").dereference().second.asInt();
      m_min = config.find("min").dereference().second.asDouble();
      m_max = config.find("max").dereference().second.asDouble();

      m_propertySet.StartAnimation(L"value",
        [node = m_inputNodeTag, min = m_min, max = m_max,  manager]()
        {
          const auto anim = winrt::Window::Current().Compositor().CreateExpressionAnimation();
          anim.SetReferenceParameter(L"input", manager->m_valueNodes.at(node)->PropertySet());
          anim.SetScalarParameter(L"min", static_cast<float>(min));
          anim.SetScalarParameter(L"max", static_cast<float>(max));
          anim.Expression(L"Clamp(input.value + input.offset, min, max)");
          return anim;
        }()
      );
    }
  }
}
