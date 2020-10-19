// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <INativeUIManager.h>
#include <IReactRootView.h>
#include <Views/ViewManagerBase.h>

#include <folly/dynamic.h>
#include <yoga/yoga.h>

#include <ReactHost/React.h>
#include <map>
#include <memory>
#include <vector>

namespace react::uwp {
struct IXamlReactControl;
}

namespace Microsoft::ReactNative {

struct YogaNodeDeleter {
  void operator()(YGNodeRef node) {
    YGNodeFree(node);
  }
};

typedef std::unique_ptr<YGNode, YogaNodeDeleter> YogaNodePtr;

class NativeUIManager final : public INativeUIManager {
 public:
  NativeUIManager(Mso::React::IReactContext *reactContext);

  // INativeUIManager
  ShadowNode *createRootShadowNode(facebook::react::IReactRootView *rootView) override;
  void configureNextLayoutAnimation(
      winrt::Microsoft::ReactNative::JSValueObject && /*config*/,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const & /*callback*/,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const & /*errorCallback*/) override{};
  void destroy() override;
  void destroyRootShadowNode(ShadowNode *) override;
  void removeRootView(ShadowNode &rootshadow) override;
  void setHost(INativeUIManagerHost *host) override;
  INativeUIManagerHost *getHost() override {
    return m_host;
  }
  void AddRootView(ShadowNode &shadowNode, facebook::react::IReactRootView *pReactRootView) override;
  void CreateView(ShadowNode &shadowNode, winrt::Microsoft::ReactNative::JSValueObject &props) override;
  void AddView(ShadowNode &parentShadowNode, ShadowNode &childShadowNode, uint64_t index) override;
  void RemoveView(ShadowNode &shadowNode, bool removeChildren = true) override;
  void ReplaceView(ShadowNode &shadowNode) override;
  void UpdateView(ShadowNode &shadowNode, winrt::Microsoft::ReactNative::JSValueObject &props) override;
  void onBatchComplete() override;
  void ensureInBatch() override;
  void measure(
      ShadowNode &shadowNode,
      ShadowNode &shadowRoot,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &callback) override;
  void measureInWindow(
      Microsoft::ReactNative::ShadowNode &shadowNode,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &callback) override;
  void measureLayout(
      ShadowNode &shadowNode,
      ShadowNode &ancestorNode,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &errorCallback,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &callback) override;
  void findSubviewIn(
      ShadowNode &shadowNode,
      float x,
      float y,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &callback) override;

  void focus(int64_t reactTag) override;
  void blur(int64_t reactTag) override;

  // Other public functions
  void DirtyYogaNode(int64_t tag);
  void AddBatchCompletedCallback(std::function<void()> callback);

  // For unparented node like Flyout, XamlRoot should be set to handle
  // XamlIsland/AppWindow scenarios. Since it doesn't have parent, and all nodes
  // in the tree should have the same XamlRoot, this function iterates all roots
  // and try to get a valid XamlRoot.
  xaml::XamlRoot tryGetXamlRoot();

  // Searches itself and its parent to get a valid XamlView.
  // Like Mouse/Keyboard, the event source may not have matched XamlView.
  XamlView reactPeerOrContainerFrom(xaml::FrameworkElement fe);

  int64_t AddMeasuredRootView(facebook::react::IReactRootView *rootView);

 private:
  void DoLayout();
  void UpdateExtraLayout(int64_t tag);
  YGNodeRef GetYogaNode(int64_t tag) const;

  std::weak_ptr<react::uwp::IXamlReactControl> GetParentXamlReactControl(int64_t tag) const;

 private:
  INativeUIManagerHost *m_host = nullptr;
  Mso::CntPtr<Mso::React::IReactContext> m_context;
  YGConfigRef m_yogaConfig;
  bool m_inBatch = false;
  int64_t m_nextRootTag = 101;
  static const int64_t RootViewTagIncrement = 10;

  std::map<int64_t, YogaNodePtr> m_tagsToYogaNodes;
  std::map<int64_t, std::unique_ptr<YogaContext>> m_tagsToYogaContext;
  std::vector<xaml::FrameworkElement::SizeChanged_revoker> m_sizeChangedVector;
  std::vector<std::function<void()>> m_batchCompletedCallbacks;
  std::vector<int64_t> m_extraLayoutNodes;

  std::map<int64_t, std::weak_ptr<react::uwp::IXamlReactControl>> m_tagsToXamlReactControl;
};

} // namespace Microsoft::ReactNative
