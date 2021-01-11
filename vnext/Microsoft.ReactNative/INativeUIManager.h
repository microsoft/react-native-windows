// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <JSValue.h>
#include <cxxreact/CxxModule.h>
#include <unordered_set>
#include <vector>

namespace facebook::react {
struct IReactRootView;
}

namespace Microsoft::ReactNative {

class IUIManager;
class IViewManager;
struct ShadowNode;

struct INativeUIManagerHost {
  // Allows the NativeUIManager to notify the UIManager that the UI has been
  // destroyed, even before react told it to be The UI went away from some
  // system event, etc.
  virtual void zombieView(int64_t tag) = 0;

  virtual void
  RegisterRootView(facebook::react::IReactRootView *rootView, int64_t rootViewTag, int64_t width, int64_t height) = 0;
  virtual std::unordered_set<int64_t> &GetAllRootTags() = 0;
  virtual ShadowNode &GetShadowNodeForTag(int64_t tag) = 0;
  virtual ShadowNode *FindShadowNodeForTag(int64_t tag) = 0;
  virtual ShadowNode *FindParentRootShadowNode(int64_t tag) = 0;
};

struct INativeUIManager {
  virtual ShadowNode *createRootShadowNode(facebook::react::IReactRootView *rootView) = 0;
  virtual void configureNextLayoutAnimation(
      winrt::Microsoft::ReactNative::JSValueObject &&config,
      std::function<void()> &&callback,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> &&errorCallback) = 0;
  virtual void destroyRootShadowNode(ShadowNode *) = 0;
  virtual void removeRootView(ShadowNode &rootNode) = 0;
  virtual void setHost(INativeUIManagerHost *host) = 0;
  virtual INativeUIManagerHost *getHost() = 0;
  virtual void AddRootView(ShadowNode &shadowNode, facebook::react::IReactRootView *pReactRootView) = 0;
  virtual void CreateView(ShadowNode &shadowNode, winrt::Microsoft::ReactNative::JSValueObject &props) = 0;
  virtual void AddView(ShadowNode &parentShadowNode, ShadowNode &childShadowNode, uint64_t index) = 0;
  virtual void RemoveView(ShadowNode &shadowNode, bool removeChildren = true) = 0;
  virtual void ReplaceView(ShadowNode &shadowNode) = 0;
  virtual void UpdateView(ShadowNode &shadowNode, winrt::Microsoft::ReactNative::JSValueObject &props) = 0;
  virtual void onBatchComplete() = 0;
  virtual void ensureInBatch() = 0;
  virtual void measure(
      ShadowNode &shadowNode,
      ShadowNode &shadowRoot,
      std::function<void(double left, double top, double width, double height, double pageX, double pageY)>
          &&callback) = 0;
  virtual void measureInWindow(
      ShadowNode &shadowNode,
      std::function<void(double x, double y, double width, double height)> &&callback) = 0;
  virtual void measureLayout(
      ShadowNode &shadowNode,
      ShadowNode &ancestorShadowNode,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> &&errorCallback,
      std::function<void(double left, double top, double width, double height)> &&callback) = 0;
  virtual void focus(int64_t reactTag) = 0;
  virtual void blur(int64_t reactTag) = 0;
  virtual void findSubviewIn(
      ShadowNode &shadowNode,
      float x,
      float y,
      std::function<void(double nativeViewTag, double left, double top, double width, double height)> &&callback) = 0;
};

} // namespace Microsoft::ReactNative
