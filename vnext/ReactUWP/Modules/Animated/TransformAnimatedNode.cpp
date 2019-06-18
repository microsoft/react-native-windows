#include "pch.h"
#include "TransformAnimatedNode.h"

namespace react {
  namespace uwp {
    TransformAnimatedNode::TransformAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodesManager>& manager) : AnimatedNode(tag), m_manager(manager)
    {
      for (auto transform : config.find("transforms").dereference().second)
      {
        auto property = transform.find("property").dereference().second.asString();
        if (transform.find("type").dereference().second.asString() == "animated")
        {
          m_transformConfigs.push_back(TransformConfig{ property, transform.find("nodeTag").dereference().second.asInt(), 0 });
        }
        else
        {
          m_transformConfigs.push_back(TransformConfig{ property, -1, transform.find("value").dereference().second.asDouble() });
        }
      }
    }

    std::unordered_map<FacadeType, int64_t> TransformAnimatedNode::GetMapping()
    {
      std::unordered_map<FacadeType, int64_t> mapping;
      for (auto config : m_transformConfigs)
      {
        if (config.nodeTag != -1)
        {
          mapping.insert({ NativeAnimatedNodesManager::StringToFacadeType(config.property), config.nodeTag });
        }
      }
      return mapping;
    }
  }
}
