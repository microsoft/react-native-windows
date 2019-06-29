// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ShadowNode.h>
#include <XamlView.h>

#include <folly/dynamic.h>
#include <yoga/yoga.h>

#include <ReactWindowsCore/ReactWindowsAPI.h>
#include "KeyboardEventHandler.h"

namespace react { namespace uwp {

class ViewManagerBase;

enum ShadowEdges : uint8_t
{
  Left = 0,
  Top,
  Right,
  Bottom,
  Start,
  End,
  Horizontal,
  Vertical,
  AllEdges,
  CountEdges
};

enum ShadowCorners : uint8_t
{
  TopLeft = 0,
  TopRight,
  BottomRight,
  BottomLeft,
  TopStart,
  TopEnd,
  BottomStart,
  BottomEnd,
  AllCorners,
  CountCorners
};

extern const DECLSPEC_SELECTANY double c_UndefinedEdge = -1;
#define INIT_UNDEFINED_EDGES { c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge }
#define INIT_UNDEFINED_CORNERS { c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge }

#pragma warning(push)
#pragma warning(disable: 4275) // base is not DLL exported
#pragma warning(disable: 4251) // member is not DLL exported
struct REACTWINDOWS_EXPORT ShadowNodeBase : public facebook::react::ShadowNode
{
  ShadowNodeBase(const ShadowNodeBase&) = delete;
  ShadowNodeBase& operator=(ShadowNodeBase const&) = delete;
  ShadowNodeBase();
  virtual ~ShadowNodeBase() {}

  virtual void onDropViewInstance() override;
  virtual void dispatchCommand(int64_t commandId, const folly::dynamic& commandArgs) override;
  virtual void removeAllChildren() override;
  virtual void AddView(ShadowNode& child, int64_t index) override;
  virtual void RemoveChildAt(int64_t indexToRemove) override;
  virtual void createView() override;
  virtual void focus() override;
  virtual void blur() override;

  virtual void updateProperties(const folly::dynamic&& props) override;

  virtual void ReplaceChild(XamlView oldChildView, XamlView newChildView);
  virtual bool ImplementsPadding() { return false; }

  ViewManagerBase* GetViewManager() const;
  XamlView GetView() const { return m_view; }
  int64_t GetParent() const { return m_parent; }

  void ReplaceView(XamlView view);

  // Extra layout handling
  virtual bool IsExternalLayoutDirty() const { return false; }
  virtual void DoExtraLayoutPrep(YGNodeRef /*yogaNode*/) {}

  bool HasTransformPS() const { return m_transformPS != nullptr; }
  winrt::Windows::UI::Composition::CompositionPropertySet EnsureTransformPS();
  void UpdateTransformPS();

protected:
  XamlView m_view;
  bool m_updating = false;
  winrt::Windows::UI::Composition::CompositionPropertySet m_transformPS{ nullptr };

public:
  double m_padding[ShadowEdges::CountEdges] = INIT_UNDEFINED_EDGES;
  double m_border[ShadowEdges::CountEdges] = INIT_UNDEFINED_EDGES;
  double m_cornerRadius[ShadowCorners::CountCorners] = INIT_UNDEFINED_CORNERS;

  // Bound event types
  bool m_onLayout = false;
  bool m_onMouseEnter = false;
  bool m_onMouseLeave = false;
  bool m_onMouseMove = false;

  // Support Keyboard
public:
  void UpdateHandledKeyboardEvents(std::string const& propertyName, folly::dynamic const& value);

private:
  void EnsureHandledKeyboardEventHandler();
  std::unique_ptr<HandledKeyboardEventHandler> m_handledKeyboardEventHandler;
};
#pragma warning(pop)

}}
