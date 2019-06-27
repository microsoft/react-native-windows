// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <INativeUIManager.h>
#include <IReactRootView.h>
#include <Views/ViewManagerBase.h>

#include <winrt/Windows.UI.Xaml.h>

#include <folly/dynamic.h>
#include <yoga/yoga.h>

#include <memory>
#include <vector>
#include <map>

namespace react { namespace uwp {

struct YogaNodeDeleter
{
  void operator()(YGNodeRef node) {
    YGNodeFree(node);
  }
};

typedef std::unique_ptr<YGNode, YogaNodeDeleter> YogaNodePtr;

class NativeUIManager : public facebook::react::INativeUIManager
{
public:
  NativeUIManager();

  // INativeUIManager
  facebook::react::ShadowNode* createRootShadowNode(facebook::react::IReactRootView* rootView) override;
  void configureNextLayoutAnimation(folly::dynamic&& config, facebook::xplat::module::CxxModule::Callback success, facebook::xplat::module::CxxModule::Callback error) override {};
  void destroy() override;
  void destroyRootShadowNode(facebook::react::ShadowNode*) override;
  void removeRootView(facebook::react::ShadowNode& rootshadow) override;
  void setHost(facebook::react::INativeUIManagerHost* host) override;
  facebook::react::INativeUIManagerHost* getHost() override { return m_host; }
  void AddRootView(facebook::react::ShadowNode& shadowNode, facebook::react::IReactRootView* pReactRootView) override;
  void CreateView(facebook::react::ShadowNode& shadowNode, folly::dynamic /*ReadableMap*/ props) override;
  void AddView(facebook::react::ShadowNode& parentShadowNode, facebook::react::ShadowNode& childShadowNode, uint64_t index) override;
  void RemoveView(facebook::react::ShadowNode& shadowNode, bool removeChildren = true) override;
  void ReplaceView(facebook::react::ShadowNode& shadowNode) override;
  void UpdateView(facebook::react::ShadowNode& shadowNode, folly::dynamic /*ReadableMap*/ props) override;
  void onBatchComplete() override;
  void ensureInBatch() override;
  void measure(facebook::react::ShadowNode& shadowNode, facebook::react::ShadowNode& shadowRoot, facebook::xplat::module::CxxModule::Callback callback) override;

  // Other public functions
  void DirtyYogaNode(int64_t tag);

  // For unparented node like Flyout, XamlRoot should be set to handle XamlIsland/AppWindow scenarios.
  // Since it doesn't have parent, and all nodes in the tree should have the same XamlRoot,
  // this function iterates all roots and try to get a valid XamlRoot.
  winrt::XamlRoot tryGetXamlRoot();

  // Searches itself and its parent to get a valid XamlView.
  // Like Mouse/Keyboard, the event source may not have matched XamlView.
  XamlView reactPeerOrContainerFrom(winrt::FrameworkElement fe);

private:
  void DoLayout();
  void UpdateExtraLayout(int64_t tag);
  YGNodeRef GetYogaNode(int64_t tag) const;

private:
  facebook::react::INativeUIManagerHost* m_host = nullptr;
  bool m_inBatch = false;

  std::map<int64_t, YogaNodePtr> m_tagsToYogaNodes;
  std::map<int64_t, std::unique_ptr<YogaContext>> m_tagsToYogaContext;
  std::vector<winrt::Windows::UI::Xaml::FrameworkElement::SizeChanged_revoker> m_sizeChangedVector;
};

} }
