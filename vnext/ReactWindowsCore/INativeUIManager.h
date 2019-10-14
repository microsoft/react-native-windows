// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include <unordered_set>
#include <vector>

namespace facebook {
namespace react {

struct IReactRootView;
class IUIManager;
class IViewManager;
struct ShadowNode;

struct INativeUIManagerHost {
  // Allows the NativeUIManager to notify the UIManager that the UI has been
  // destroyed, even before react told it to be The UI went away from some
  // system event, etc.
  virtual void zombieView(int64_t tag) = 0;

  virtual std::unordered_set<int64_t> &GetAllRootTags() = 0;
  virtual ShadowNode &GetShadowNodeForTag(int64_t tag) = 0;
  virtual ShadowNode *FindShadowNodeForTag(int64_t tag) = 0;
  virtual ShadowNode *FindParentRootShadowNode(int64_t tag) = 0;
};

struct INativeUIManager {
  virtual void destroy() = 0;
  virtual ShadowNode *createRootShadowNode(IReactRootView *rootView) = 0;
  virtual void configureNextLayoutAnimation(
      folly::dynamic &&config,
      facebook::xplat::module::CxxModule::Callback success,
      facebook::xplat::module::CxxModule::Callback error) = 0;
  virtual void destroyRootShadowNode(ShadowNode *) = 0;
  virtual void removeRootView(ShadowNode &rootNode) = 0;
  virtual void setHost(INativeUIManagerHost *host) = 0;
  virtual INativeUIManagerHost *getHost() = 0;
  virtual void AddRootView(
      facebook::react::ShadowNode &shadowNode,
      facebook::react::IReactRootView *pReactRootView) = 0;
  virtual void CreateView(
      facebook::react::ShadowNode &shadowNode,
      folly::dynamic /*ReadableMap*/ props) = 0;
  virtual void AddView(
      facebook::react::ShadowNode &parentShadowNode,
      facebook::react::ShadowNode &childShadowNode,
      uint64_t index) = 0;
  virtual void RemoveView(
      facebook::react::ShadowNode &shadowNode,
      bool removeChildren = true) = 0;
  virtual void ReplaceView(facebook::react::ShadowNode &shadowNode) = 0;
  virtual void UpdateView(
      facebook::react::ShadowNode &shadowNode,
      folly::dynamic /*ReadableMap*/ props) = 0;
  virtual void onBatchComplete() = 0;
  virtual void ensureInBatch() = 0;
  virtual void measure(
      facebook::react::ShadowNode &shadowNode,
      facebook::react::ShadowNode &shadowRoot,
      facebook::xplat::module::CxxModule::Callback callback) = 0;
  virtual void measureInWindow(
      facebook::react::ShadowNode &shadowNode,
      facebook::xplat::module::CxxModule::Callback callback) = 0;
  virtual void focus(int64_t reactTag) = 0;
  virtual void blur(int64_t reactTag) = 0;
  virtual void findSubviewIn(
      facebook::react::ShadowNode &shadowNode,
      float x,
      float y,
      facebook::xplat::module::CxxModule::Callback callback) = 0;
};

} // namespace react
} // namespace facebook
