// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#if !defined(OSS_RN)
#include <cxxreact/Platform.h>
#endif
#include <INativeUIManager.h>
#include <InstanceManager.h>
#include <ShadowNode.h>
#include <ViewManager.h>
#include <folly/dynamic.h>

#include <queue>

namespace Microsoft::React::Test {

struct ITestInstance {
  virtual void AttachMeasuredRootView(std::string &&appName) noexcept = 0;
  virtual void DetachRootView() noexcept = 0;
  virtual std::shared_ptr<facebook::react::Instance> GetInnerInstance() const
      noexcept = 0;
};

class TestViewManager : public facebook::react::IViewManager {
  const std::string m_name;

 public:
  TestViewManager(const char *name);

#pragma region IViewManager members

  const char *GetName() const override;
  folly::dynamic GetExportedViewConstants() const override;
  folly::dynamic GetCommands() const override;
  folly::dynamic GetNativeProps() const override;
  facebook::react::ShadowNode *createShadow() const override;
  void destroyShadow(facebook::react::ShadowNode *) const override;

  folly::dynamic GetConstants() const override;
  folly::dynamic GetExportedCustomBubblingEventTypeConstants() const override;
  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;

#pragma endregion IViewManager members
};

class TestNativeUIManager : public facebook::react::INativeUIManager {
 public:
  void destroy() override;
  void configureNextLayoutAnimation(
      folly::dynamic &&config,
      facebook::xplat::module::CxxModule::Callback success,
      facebook::xplat::module::CxxModule::Callback error) override;
  facebook::react::ShadowNode *createRootShadowNode(
      facebook::react::IReactRootView *rootView) override;
  void destroyRootShadowNode(facebook::react::ShadowNode *) override;
  void removeRootView(facebook::react::ShadowNode &rootNode) override;
  void setHost(facebook::react::INativeUIManagerHost *host) override;
  facebook::react::INativeUIManagerHost *getHost() override {
    return nullptr;
  }
  void AddRootView(
      facebook::react::ShadowNode &shadowNode,
      facebook::react::IReactRootView *pReactRootView) override;
  void CreateView(
      facebook::react::ShadowNode &shadowNode,
      folly::dynamic /*ReadableMap*/ props) override;
  void AddView(
      facebook::react::ShadowNode &parentShadowNode,
      facebook::react::ShadowNode &childShadowNode,
      uint64_t index) override;
  void RemoveView(
      facebook::react::ShadowNode &shadowNode,
      bool removeChildren = true) override;
  void ReplaceView(facebook::react::ShadowNode &shadowNode) override;
  void UpdateView(
      facebook::react::ShadowNode &shadowNode,
      folly::dynamic /*ReadableMap*/ props) override;
  void onBatchComplete() override;
  void ensureInBatch() override;
  void measure(
      facebook::react::ShadowNode &shadowNode,
      facebook::react::ShadowNode &shadowRoot,
      facebook::xplat::module::CxxModule::Callback callback) override;
  void findSubviewIn(
      facebook::react::ShadowNode &shadowNode,
      int64_t x,
      int64_t y,
      facebook::xplat::module::CxxModule::Callback callback) override;
  void focus(int64_t reactTag) override;
  void blur(int64_t reactTag) override;
};

class TestShadowNode : public facebook::react::ShadowNode {
 public:
#pragma region ShadowNode overrides

  void onDropViewInstance() override;
  void removeAllChildren() override;
  void AddView(ShadowNode &child, int64_t index) override;
  void RemoveChildAt(int64_t indexToRemove) override;
  void createView() override;

#pragma endregion ShadowNode overrides
};

} // namespace Microsoft::React::Test
