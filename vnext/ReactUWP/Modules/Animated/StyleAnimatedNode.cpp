#include "pch.h"
#include "StyleAnimatedNode.h"

namespace react {
  namespace uwp {
    StyleAnimatedNode::StyleAnimatedNode(int64_t tag, const folly::dynamic& config/*, const std::shared_ptr<NativeAnimatedNodesManager>& manager*/) : AnimatedNode(tag)
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
        mapping.insert({ StringToFacadeType(prop.first), prop.second });
      }
      return mapping;
    }

    FacadeType StyleAnimatedNode::StringToFacadeType(const std::string& string)
    {
      if (string == "opacity") return FacadeType::opacity;
      else return FacadeType::opacity;
    }
  }
}
