#include "pch.h"
#include "StyleAnimatedNode.h"

namespace react {
  namespace uwp {
    StyleAnimatedNode::StyleAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodesManager>& manager) : AnimatedNode(tag), m_manager(manager)
    {
      for (auto entry : config.find("style").dereference().second.items())
      {
        m_propMapping.insert({ entry.first.getString(), entry.second.getInt() });
      }
    }

    void StyleAnimatedNode::CollectViewUpdates(const folly::dynamic& propsMap)
    {

    }

    std::unordered_map<FacadeType, int64_t> StyleAnimatedNode::GetMapping()
    {
      std::unordered_map<FacadeType, int64_t> mapping;
      for (auto prop : m_propMapping)
      {
        if (auto manager = m_manager.lock())
        {
          if (manager->m_transformNodes.count(prop.second))
          {
            if (auto transformNode = manager->m_transformNodes.at(prop.second))
            {
              auto transformMapping = transformNode->GetMapping();
              mapping.insert(transformMapping.begin(), transformMapping.end());
              break;
            }
          }
        }
        mapping.insert({ NativeAnimatedNodesManager::StringToFacadeType(prop.first), prop.second });
      }
      return mapping;
    }
  }
}
