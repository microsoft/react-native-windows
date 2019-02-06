// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ViewManager.h>

#include <IReactInstance.h>
#include <XamlView.h>

#include <folly/dynamic.h>
#include <yoga/yoga.h>

#include <map>

namespace facebook { namespace react {
struct ShadowNode;
} } // facebook::react

namespace react { namespace uwp {

struct IReactInstance;
struct ShadowNodeBase;

struct YogaContext
{
  YogaContext(const XamlView& view_) : view(view_) {}

  XamlView view;
};

YGSize DefaultYogaSelfMeasureFunc(YGNodeRef node, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode);

class ViewManagerBase : public facebook::react::IViewManager
{
public:
  ViewManagerBase(const std::shared_ptr<IReactInstance>& reactInstance);
  virtual ~ViewManagerBase() {}

  virtual XamlView CreateView(int64_t tag);

  folly::dynamic GetExportedViewConstants() const override;
  folly::dynamic GetCommands() const override;
  folly::dynamic GetNativeProps() const override;

  facebook::react::ShadowNode* createShadow() const override;
  void destroyShadow(facebook::react::ShadowNode* node) const override;

  folly::dynamic GetConstants() const override;
  folly::dynamic GetExportedCustomBubblingEventTypeConstants() const override;
  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;

  virtual void AddView(XamlView parent, XamlView child, int64_t index);
  virtual XamlView GetChildAt(XamlView parent, int64_t index);
  virtual int64_t GetChildCount(XamlView parent);
  virtual void RemoveAllChildren(XamlView parent);
  virtual void RemoveChildAt(XamlView parent, int64_t index);

  virtual void UpdateProperties(ShadowNodeBase* nodeToUpdate, XamlView viewToUpdate, folly::dynamic reactDiffMap);

  virtual void DispatchCommand(XamlView viewToUpdate, int64_t commandId, const folly::dynamic& commandArgs);

  // Yoga Layout
  virtual void SetLayoutProps(ShadowNodeBase& nodeToUpdate, XamlView viewToUpdate, float left, float top, float width, float height);
  virtual YGMeasureFunc GetYogaCustomMeasureFunc() const;
  virtual bool RequiresYogaNode() const;
  bool IsNativeControlWithSelfLayout() const;

  std::weak_ptr<IReactInstance> GetReactInstance() const { return m_wkReactInstance; }

protected:
  virtual XamlView CreateViewCore(int64_t tag) = 0;

protected:
  std::weak_ptr<IReactInstance> m_wkReactInstance;
};

} }
