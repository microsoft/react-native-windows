// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ViewManager.h>
#include <cxxreact/CxxModule.h>
#include <vector>

namespace Microsoft::React::Test {

struct EmptyUINode {
  EmptyUINode(int64_t tag, const std::string &className);
  int64_t m_tag;
  std::string m_className;
  std::vector<int64_t> m_children;
};

struct NodeRegistry {
  std::map<int64_t, std::shared_ptr<EmptyUINode>> m_roots;
  std::map<int64_t, std::shared_ptr<EmptyUINode>> m_allNodes;

  std::string PrintTree();

 private:
  void PrintNode(std::stringstream &ss, int indent, EmptyUINode *node);
};

class EmptyUIManager {
 public:
  EmptyUIManager(
      std::unique_ptr<std::vector<
          std::unique_ptr<facebook::react::IViewManager>>> viewManagers,
      std::shared_ptr<NodeRegistry> nodeRegistry);

  void removeRootView(int64_t rootViewTag);
  void createView(
      int64_t tag,
      const std::string &className,
      int64_t rootViewTag,
      folly::dynamic /*ReadableMap*/ props);
  void setChildren(
      int64_t viewTag,
      folly::dynamic /*ReadableMap*/ childrenTags);
#ifdef NOT_DEF
  void updateView(
      int64_t tag,
      const std::string &className,
      folly::dynamic /*ReadableMap*/ props);
  void manageChildren(
      int64_t viewTag,
      folly::dynamic * /*@Nullable ReadableArray*/ moveFrom,
      folly::dynamic * /*@Nullable ReadableArray*/ moveTo,
      folly::dynamic * /*@Nullable ReadableArray*/ addChildTags,
      folly::dynamic * /*@Nullable ReadableArray*/ addAtIndices,
      folly::dynamic * /*@Nullable ReadableArray*/ removeFrom);
  void replaceExistingNonRootView(int64_t oldTag, int64_t newTag);
  void removeSubviewsFromContainerWithID(int64_t containerTag);
  void measure(int64_t reactTag, std::function<void()> /*Callback*/ callback);
  void measureInWindow(
      int64_t reactTag,
      std::function<void()> /*Callback*/ callback);
  void measureLayout(
      int64_t tag,
      int64_t ancestorTag,
      std::function<void()> /*Callback*/ errorCallback,
      std::function<void()> /*Callback*/ successCallback);
  void measureLayoutRelativeToParent(
      int64_t tag,
      std::function<void()> /*Callback*/ errorCallback,
      std::function<void()> /*Callback*/ successCallback);
  void findSubviewIn(
      int64_t reactTag,
      folly::dynamic /*final ReadableArray*/ point,
      std::function<void()> /*final Callback*/ callback);
  void setJSResponder(int64_t reactTag, bool blockNativeResponder);
  void clearJSResponder();
  void dispatchViewManagerCommand(
      int64_t reactTag,
      int64_t commandId,
      folly::dynamic /*ReadableMap*/ commandArgs);
  void showPopupMenu(
      int64_t reactTag,
      folly::dynamic /*ReadableMap*/ items,
      std::function<void()> /*Callback*/ error,
      std::function<void()> /*Callback*/ success);
  void setLayoutAnimationEnabledExperimental(bool enabled);
  void configureNextLayoutAnimation(
      folly::dynamic /*ReadableMap*/ config,
      std::function<void()> /*Callback*/ success,
      std::function<void()> /*Callback*/ error);
  void sendAccessibilityEvent(int64_t tag, int64_t eventType);
#endif

  void PopulateViewManagerConstants(
      std::map<std::string, folly::dynamic> &constants);
  std::shared_ptr<EmptyUINode> addRootView(int64_t rootViewTag);

 private:
  std::unique_ptr<std::vector<std::unique_ptr<facebook::react::IViewManager>>>
      m_viewManagers;
  std::shared_ptr<NodeRegistry> m_nodeRegistry;
};

class EmptyUIManagerModule : public facebook::xplat::module::CxxModule {
 public:
  EmptyUIManagerModule(std::unique_ptr<EmptyUIManager> sample);
  std::string getName();
  virtual std::map<std::string, folly::dynamic> getConstants();
  virtual auto getMethods() -> std::vector<Method>;

 private:
  std::unique_ptr<EmptyUIManager> m_manager;
};

} // namespace Microsoft::React::Test
