// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ShadowNodeRegistry.h"
#include "ShadowNode.h"
#include "ViewManager.h"

namespace facebook {
namespace react {

void ShadowNodeDeleter::operator()(ShadowNode *node) {
  if (node->m_viewManager)
    node->m_viewManager->destroyShadow(node);
}

void ShadowNodeRegistry::addRootView(
    std::unique_ptr<ShadowNode, ShadowNodeDeleter> &&root,
    int64_t rootViewTag) {
  m_roots.insert(rootViewTag);
  m_allNodes[rootViewTag] = std::move(root);
}

ShadowNode &ShadowNodeRegistry::getRoot(int64_t rootViewTag) {
  CHECK(m_roots.find(rootViewTag) != m_roots.end());
  return getNode(rootViewTag);
}

void ShadowNodeRegistry::removeRootView(int64_t rootViewTag) {
  m_roots.erase(rootViewTag);
  removeNode(rootViewTag);
}

void ShadowNodeRegistry::addNode(
    std::unique_ptr<ShadowNode, ShadowNodeDeleter> &&node,
    int64_t tag) {
  m_allNodes[tag] = std::move(node);
}

ShadowNode *ShadowNodeRegistry::findNode(int64_t tag) {
  auto iter = m_allNodes.find(tag);
  return (iter != m_allNodes.end()) ? iter->second.get() : nullptr;
}

ShadowNode &ShadowNodeRegistry::getNode(int64_t tag) {
  return *m_allNodes.at(tag);
}

void ShadowNodeRegistry::removeNode(int64_t tag) {
  m_allNodes.erase(tag);
}

void ShadowNodeRegistry::removeAllRootViews(
    const std::function<void(int64_t rootViewTag)> &fn) {
  while (!m_roots.empty())
    fn(*m_roots.begin());
}

std::unordered_set<int64_t> &ShadowNodeRegistry::getAllRoots() {
  return m_roots;
}

// iterate its parent to get the root shadow node
ShadowNode *ShadowNodeRegistry::getParentRootShadowNode(int64_t nodeTag) {
  auto node = findNode(nodeTag);
  while (node) {
    if (m_roots.find(node->m_tag) != m_roots.end()) {
      return node;
    }
    node = findNode(node->m_parent);
  }
  return nullptr;
}

} // namespace react
} // namespace facebook