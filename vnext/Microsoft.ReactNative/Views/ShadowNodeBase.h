// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/PaperShadowNode.h>
#include <XamlView.h>

#include <folly/dynamic.h>
#include <yoga/yoga.h>

#include <Shared/ReactWindowsAPI.h>
#include "KeyboardEventHandler.h"

namespace Microsoft::ReactNative {

class ViewManagerBase;

enum class ShadowEdges : uint8_t {
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

enum class ShadowCorners : uint8_t {
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
#define INIT_UNDEFINED_EDGES                                                                              \
  {                                                                                                       \
    c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, \
        c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge                                                 \
  }
#define INIT_UNDEFINED_CORNERS                                                                            \
  {                                                                                                       \
    c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge, \
        c_UndefinedEdge, c_UndefinedEdge, c_UndefinedEdge                                                 \
  }

#pragma warning(push)
#pragma warning(disable : 4275) // base is not DLL exported
#pragma warning(disable : 4251) // member is not DLL exported
struct REACTWINDOWS_EXPORT ShadowNodeBase : public ShadowNode {
  ShadowNodeBase(const ShadowNodeBase &) = delete;
  ShadowNodeBase &operator=(ShadowNodeBase const &) = delete;
  ShadowNodeBase();
  virtual ~ShadowNodeBase() {}

  virtual void onDropViewInstance() override;
  virtual void dispatchCommand(const std::string &commandId, winrt::Microsoft::ReactNative::JSValueArray &&commandArgs)
      override;
  virtual void removeAllChildren() override;
  virtual void AddView(ShadowNode &child, int64_t index) override;
  virtual void RemoveChildAt(int64_t indexToRemove) override;
  virtual void createView() override;
  virtual bool NeedsForceLayout();

  virtual void updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) override;

  virtual void ReplaceChild(const XamlView &oldChildView, const XamlView &newChildView);
  virtual bool ImplementsPadding() {
    return false;
  }

  void YellowBox(const std::string &message) const noexcept;
  void RedBox(const std::string &message) const noexcept;

  ViewManagerBase *GetViewManager() const;
  XamlView GetView() const {
    return m_view;
  }
  int64_t GetParent() const {
    return m_parent;
  }
  virtual bool IsWindowed() {
    return false;
  }

  void ReplaceView(XamlView view);

  void ReparentView(XamlView view);

  // Extra layout handling
  virtual bool IsExternalLayoutDirty() const {
    return false;
  }
  virtual void DoExtraLayoutPrep(YGNodeRef /*yogaNode*/) {}

  bool HasTransformPS() const {
    return m_transformPS != nullptr;
  }
  comp::CompositionPropertySet EnsureTransformPS();
  void UpdateTransformPS();

  bool IsRegisteredForMouseEvents() const {
    return m_onMouseEnterRegistered || m_onMouseLeaveRegistered;
  }

 protected:
  XamlView m_view;
  bool m_updating = false;
  comp::CompositionPropertySet m_transformPS{nullptr};

 public:
  double m_padding[(int)ShadowEdges::CountEdges] = INIT_UNDEFINED_EDGES;
  double m_border[(int)ShadowEdges::CountEdges] = INIT_UNDEFINED_EDGES;
  double m_cornerRadius[(int)ShadowCorners::CountCorners] = INIT_UNDEFINED_CORNERS;

  // Bound event types
  bool m_onLayoutRegistered = false;
  bool m_onMouseEnterRegistered = false;
  bool m_onMouseLeaveRegistered = false;

  // Support Keyboard
 public:
  void UpdateHandledKeyboardEvents(
      std::string const &propertyName,
      winrt::Microsoft::ReactNative::JSValue const &value);

 private:
  void EnsureHandledKeyboardEventHandler();
  std::unique_ptr<HandledKeyboardEventHandler> m_handledKeyboardEventHandler;
};
#pragma warning(pop)

} // namespace Microsoft::ReactNative
