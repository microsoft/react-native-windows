// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <folly/dynamic.h>
#include <string>
#include <vector>
#include "JSValue.h"

namespace Microsoft::ReactNative {

class IViewManager;

struct ShadowNode {
  ShadowNode(const ShadowNode &that) = delete;
  ShadowNode &operator=(ShadowNode const &) = delete;
  ShadowNode() = default;
  virtual ~ShadowNode();

  virtual void updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props);
  virtual void onDropViewInstance() = 0;
  virtual void dispatchCommand(const std::string &commandId, winrt::Microsoft::ReactNative::JSValueArray &&commandArgs);
  virtual void removeAllChildren() = 0;
  virtual void AddView(ShadowNode &child, int64_t index) = 0;
  virtual void RemoveChildAt(int64_t indexToRemove) = 0;
  virtual void createView(const winrt::Microsoft::ReactNative::JSValueObject &props) = 0;

  int64_t m_tag{0};
  int64_t m_rootTag{0};
  std::string m_className;
  std::vector<int64_t> m_children;
  int64_t m_parent = -1;
  IViewManager *m_viewManager = nullptr;
  bool m_zombie = false;
};

} // namespace Microsoft::ReactNative
