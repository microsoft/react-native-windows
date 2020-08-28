// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <ShadowNode.h>
#include <functional/functorref.h>
#include <map>

namespace facebook {
namespace react {

class IViewManager;

struct ShadowNodeDeleter {
  void operator()(ShadowNode *node);
};

using shadow_ptr = std::unique_ptr<ShadowNode, ShadowNodeDeleter>;

struct ShadowNodeRegistry {
  void addRootView(std::unique_ptr<ShadowNode, ShadowNodeDeleter> &&root, int64_t rootViewTag);
  ShadowNode &getRoot(int64_t rootViewTag);
  void removeRootView(int64_t rootViewTag);

  void addNode(std::unique_ptr<ShadowNode, ShadowNodeDeleter> &&node, int64_t tag);
  ShadowNode &getNode(int64_t tag);
  ShadowNode *findNode(int64_t tag);
  void removeNode(int64_t tag);

  void removeAllRootViews(const std::function<void(int64_t rootViewTag)> &);

  std::unordered_set<int64_t> &getAllRoots();

  ShadowNode *getParentRootShadowNode(int64_t nodeTag);

  void ForAllNodes(const Mso::FunctorRef<void(int64_t, shadow_ptr const &) noexcept> &fnDo) noexcept;

 private:
  std::unordered_set<int64_t> m_roots;
  std::map<int64_t, std::unique_ptr<ShadowNode, ShadowNodeDeleter>> m_allNodes;
};

} // namespace react
} // namespace facebook
