#pragma once
#include "pch.h"
#include "AnimatedNode.h"
#include <folly/dynamic.h>
#include "NativeAnimatedNodesManager.h"

namespace react {
  namespace uwp {

    class StyleAnimatedNode : public AnimatedNode
    {
    public:
      StyleAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodesManager>& manager);
      void CollectViewUpdates(const folly::dynamic& propsMap);

      std::unordered_map<FacadeType, int64_t> GetMapping();
    private:
      std::weak_ptr<NativeAnimatedNodesManager> m_manager{};
      std::map<std::string, int64_t> m_propMapping{};
    };
  }
}
