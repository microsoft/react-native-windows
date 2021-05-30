// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "EmptyUIManagerModule.h"
#include <cxxreact/JsArgumentHelpers.h>

using namespace std;
using namespace facebook::react;
using namespace facebook::xplat;

namespace Microsoft::React::Test {

std::string NodeRegistry::PrintTree() {
  std::stringstream ss;
  ss << "** Node Tree **\n";
  for (auto &&root : m_roots)
    PrintNode(ss, 0, root.second.get());
  ss << "** End Tree **\n";
  return ss.str();
}

void NodeRegistry::PrintNode(std::stringstream &ss, int indent, EmptyUINode *node) {
  for (int i = 0; i < indent; i++)
    ss << '\t';

  ss << '{' << node->m_tag << " : " << node->m_className << "}\n";
  for (auto &&child : node->m_children)
    PrintNode(ss, indent + 1, m_allNodes[child].get());
}

EmptyUINode::EmptyUINode(int64_t tag, const std::string &className) : m_tag(tag), m_className(className) {}

EmptyUIManager::EmptyUIManager(
    std::unique_ptr<std::vector<std::unique_ptr<IViewManager>>> viewManagers,
    std::shared_ptr<NodeRegistry> nodeRegistry)
    : m_viewManagers(std::move(viewManagers)), m_nodeRegistry(nodeRegistry) {}

void EmptyUIManager::PopulateViewManagerConstants(std::map<std::string, folly::dynamic> &constants) {
  for (auto &&vm : *m_viewManagers)
    constants.emplace(std::make_pair(std::string(vm->GetName()), vm->GetConstants()));
}

std::shared_ptr<EmptyUINode> EmptyUIManager::addRootView(int64_t rootViewTag) {
  auto root = std::make_shared<EmptyUINode>(rootViewTag, std::string("ROOT"));
  m_nodeRegistry->m_roots[rootViewTag] = root;
  m_nodeRegistry->m_allNodes[rootViewTag] = root;
  return root;
}

void EmptyUIManager::removeRootView(int64_t rootViewTag) {
  m_nodeRegistry->m_roots.erase(rootViewTag);
}

void EmptyUIManager::createView(
    int64_t tag,
    const std::string &className,
    int64_t rootViewTag,
    folly::dynamic /*ReadableMap*/ props) {
  auto it = m_nodeRegistry->m_roots.find(rootViewTag);
  // Create root if this is the first time we're seeing it
  if (it == m_nodeRegistry->m_roots.end()) {
    addRootView(rootViewTag);
  }

  m_nodeRegistry->m_allNodes[tag] = std::make_shared<EmptyUINode>(tag, className);
}

void EmptyUIManager::setChildren(int64_t viewTag, folly::dynamic /*ReadableMap*/ childrenTags) {
  auto &parent = m_nodeRegistry->m_allNodes[viewTag];
  for (auto &&childTag : childrenTags)
    parent->m_children.push_back(childTag.asInt());
}

EmptyUIManagerModule::EmptyUIManagerModule(std::unique_ptr<EmptyUIManager> sample) : m_manager(std::move(sample)) {}

std::string EmptyUIManagerModule::getName() {
  return "UIManager";
}

std::map<std::string, folly::dynamic> EmptyUIManagerModule::getConstants() {
  std::map<std::string, folly::dynamic> constants{
      {"Dimensions",
       folly::dynamic::object(
           "windowPhysicalPixels",
           folly::dynamic::object("width", 100)("height", 100)("scale", 1.0)("fontScale", 1.0)("densityDpi", 72))(
           "screenPhysicalPixels",
           folly::dynamic::object("width", 100)("height", 100)("scale", 1.0)("fontScale", 1.0)("densityDpi", 72))}};

  // TODO merge in customBubblingEventTypes and customDirectEventTypes from
  // ViewManagers

  m_manager->PopulateViewManagerConstants(constants);

  return std::move(constants);
}

auto EmptyUIManagerModule::getMethods() -> std::vector<Method> {
  return {
      Method("removeRootView", [this](folly::dynamic args) { m_manager->removeRootView(jsArgAsInt(args, 0)); }),
      Method(
          "createView",
          [this](folly::dynamic args) {
            m_manager->createView(
                jsArgAsInt(args, 0), jsArgAsString(args, 1), jsArgAsInt(args, 2), jsArgAsDynamic(args, 3));
          }),
      Method(
          "setChildren",
          [this](folly::dynamic args) { m_manager->setChildren(jsArgAsInt(args, 0), jsArgAsArray(args, 1)); }),
  };
}

} // namespace Microsoft::React::Test
