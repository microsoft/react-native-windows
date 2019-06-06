// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "NativeUIManager.h"

#include <ReactRootView.h>
#include <Views/ShadowNodeBase.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
}

namespace react { namespace uwp {

static YogaNodePtr make_yoga_node()
{
  YogaNodePtr result(YGNodeNew());
  return result;
}

static int YogaLog(
  const YGConfigRef config,
  const YGNodeRef node,
  YGLogLevel level,
  const char* format,
  va_list args)
{
  int len = _scprintf(format, args);
  std::string buffer(len + 1, '\0');
  int c = vsnprintf_s(&buffer[0], len + 1, _TRUNCATE, format, args);
  buffer.resize(len);

  // OutputDebugString will truncate output around 4k, here
  // we output a 1000 characters at a time.
  int start = 0;
  while (len - start > 1000)
  {
    char tmp = buffer[start + 1000];
    buffer[start + 1000] = '\0';
    OutputDebugStringA(&buffer[start]);
    buffer[start + 1000] = tmp;
    start += 1000;
  }
  OutputDebugStringA(&buffer[start]);
  OutputDebugStringA("\n");

  return 0;
}

YGNodeRef NativeUIManager::GetYogaNode(int64_t tag) const
{
  auto iter = m_tagsToYogaNodes.find(tag);
  if (iter == m_tagsToYogaNodes.end())
    return nullptr;
  return iter->second.get();
}

void NativeUIManager::DirtyYogaNode(int64_t tag)
{
  ShadowNodeBase* pShadowNodeChild = static_cast<ShadowNodeBase*>(m_host->FindShadowNodeForTag(tag));
  if (pShadowNodeChild != nullptr)
  {
    auto* pViewManager = pShadowNodeChild->GetViewManager();
    YGMeasureFunc func = pViewManager->GetYogaCustomMeasureFunc();
    if (func != nullptr)
    {
      // If there is a yoga node for this tag mark it as dirty
      YGNodeRef yogaNodeChild = GetYogaNode(tag);
      if (yogaNodeChild != nullptr)
      {
        // Retrieve and dirty the yoga node
        YGNodeMarkDirty(yogaNodeChild);

        // Once we mark a node dirty we can stop because the yoga code will mark all parents anyway
        return;
      }
    }

    // Since this node didn't meet the criteria, jump to parent in case it does
    int64_t parentTag = pShadowNodeChild->GetParent();
    if (parentTag >= 0)
      DirtyYogaNode(parentTag);
  }
}

winrt::XamlRoot NativeUIManager::tryGetXamlRoot()
{
  if (m_host)
  {
    for (auto const tag : m_host->GetAllRootTags())
    {
      if (auto shadowNode = static_cast<ShadowNodeBase*>(m_host->FindShadowNodeForTag(tag)))
      {
        if (auto uiElement10 = shadowNode->GetView().try_as<winrt::IUIElement10>())
        {
          if (auto xamlRoot = uiElement10.XamlRoot())
            return xamlRoot;
        }
      }
    }
  }
  return nullptr;
}

NativeUIManager::NativeUIManager()
{
#if defined(_DEBUG)
  YGConfigSetLogger(YGConfigGetDefault(), &YogaLog);

  // To Debug Yoga layout, uncomment the following line.
  // YGConfigSetPrintTreeFlag(YGConfigGetDefault(), true);

  // Additional logging can be enabled editing yoga.cpp (e.g. gPrintChanges, gPrintSkips)
#endif
}

struct RootShadowNode final : public ShadowNodeBase
{
  RootShadowNode(const RootShadowNode& that) = delete;
  RootShadowNode& operator=(RootShadowNode const&) = delete;
  RootShadowNode() = delete;

  RootShadowNode(facebook::react::IReactRootView* rootView, facebook::react::INativeUIManagerHost* host)
    : m_host(host)
  {
    auto reactRootView = static_cast<react::uwp::ReactRootView*>(rootView);
    m_view = reactRootView->GetXamlView();
  }

  void createView() override
  {
    // ASSERT: The root view is created before react, so react should never tell the root view to be created.
    assert(false);
  }

  void AddView(facebook::react::ShadowNode& child, int64_t index) override
  {
    auto panel(GetView().as<winrt::Panel>());
    if (panel != nullptr)
    {
      auto childView = static_cast<ShadowNodeBase&>(child).GetView().as<winrt::UIElement>();
      panel.Children().InsertAt(static_cast<uint32_t>(index), childView);
    }
  }

private:
  facebook::react::INativeUIManagerHost* m_host;
};

void NativeUIManager::setHost(facebook::react::INativeUIManagerHost* host)
{
  m_host = host;
}

facebook::react::ShadowNode* NativeUIManager::createRootShadowNode(facebook::react::IReactRootView* pReactRootView)
{
  return new RootShadowNode(pReactRootView, m_host);
}

void NativeUIManager::destroyRootShadowNode(facebook::react::ShadowNode* node)
{
  delete node;
}

void NativeUIManager::AddRootView(facebook::react::ShadowNode& shadowNode, facebook::react::IReactRootView* pReactRootView)
{
  ShadowNodeBase& node = static_cast<ShadowNodeBase&>(shadowNode);

  XamlView view = static_cast<IXamlRootView*>(pReactRootView)->GetXamlView();

  m_tagsToYogaNodes.emplace(shadowNode.m_tag, make_yoga_node());

  auto element = view.as<winrt::FrameworkElement>();
  element.Tag(winrt::PropertyValue::CreateInt64(shadowNode.m_tag));

  // Add listener to size change so we can redo the layout when that happens
  m_sizeChangedVector.push_back(
    view.as<winrt::FrameworkElement>().SizeChanged(winrt::auto_revoke, [this](auto&&, auto&&) {
    DoLayout();
  }));
}

void NativeUIManager::destroy()
{
  delete this;
}

void NativeUIManager::removeRootView(facebook::react::ShadowNode& shadow)
{
  RemoveView(shadow, true);
}

void NativeUIManager::onBatchComplete()
{
  if (m_inBatch)
  {
    DoLayout();
    m_inBatch = false;
  }
}

void NativeUIManager::ensureInBatch()
{
  if (!m_inBatch)
    m_inBatch = true;
}

static float NumberOrDefault(const folly::dynamic& value, float defaultValue)
{
  float result = defaultValue;

  if (value.isNumber())
    result = static_cast<float>(value.asDouble());
  else if (value.isNull())
    result = defaultValue;
  else
    assert(false);

  return result;
}

static YGValue YGValueOrDefault(const folly::dynamic& value, YGValue defaultValue)
{
  YGValue result = defaultValue;

  if (value.isNumber())
    return YGValue{ static_cast<float>(value.asDouble()) , YGUnitPoint };

  if (value.isNull())
    return defaultValue;

  if (value.isString())
  {
    std::string str = value.getString();
    if (str == "auto")
      return YGValue{ YGUndefined, YGUnitAuto };
    if (str.length() > 0 && str.back() == '%')
    {
      str.pop_back();
      folly::dynamic pct(str);
      return YGValue{ static_cast<float>(pct.asDouble()), YGUnitPercent };
    }
  }

  // Unexpected format, using default
  assert(false);
  return defaultValue;
}

typedef void (*YogaSetterFunc)(const YGNodeRef yogaNode, const YGEdge edge, const float value);
static void SetYogaValueHelper(const YGNodeRef yogaNode, const YGEdge edge, const YGValue& value, YogaSetterFunc normalSetter, YogaSetterFunc percentSetter)
{
  switch (value.unit)
  {
    case YGUnitAuto:
    case YGUnitUndefined:
      normalSetter(yogaNode, edge, YGUndefined);
      break;
    case YGUnitPoint:
      normalSetter(yogaNode, edge, value.value);
      break;
    case YGUnitPercent:
      percentSetter(yogaNode, edge, value.value);
      break;
  }
}

typedef void (*YogaUnitSetterFunc)(const YGNodeRef yogaNode, const float value);
static void SetYogaUnitValueHelper(const YGNodeRef yogaNode, const YGValue& value, YogaUnitSetterFunc normalSetter, YogaUnitSetterFunc percentSetter)
{
  switch (value.unit)
  {
    case YGUnitAuto:
    case YGUnitUndefined:
      normalSetter(yogaNode, YGUndefined);
      break;
    case YGUnitPoint:
      normalSetter(yogaNode, value.value);
      break;
    case YGUnitPercent:
      percentSetter(yogaNode, value.value);
      break;
  }
}

typedef void (*YogaAutoUnitSetterFunc)(const YGNodeRef yogaNode);
static void SetYogaUnitValueAutoHelper(const YGNodeRef yogaNode, const YGValue& value, YogaUnitSetterFunc normalSetter, YogaUnitSetterFunc percentSetter, YogaAutoUnitSetterFunc autoSetter)
{
  switch (value.unit)
  {
    case YGUnitAuto:
      autoSetter(yogaNode);
      break;
    case YGUnitUndefined:
      normalSetter(yogaNode, YGUndefined);
      break;
    case YGUnitPoint:
      normalSetter(yogaNode, value.value);
      break;
    case YGUnitPercent:
      percentSetter(yogaNode, value.value);
      break;
  }
}

typedef void (*YogaAutoSetterFunc)(const YGNodeRef yogaNode, const YGEdge edge);
static void SetYogaValueAutoHelper(const YGNodeRef yogaNode, const YGEdge edge, const YGValue& value, YogaSetterFunc normalSetter, YogaSetterFunc percentSetter, YogaAutoSetterFunc autoSetter)
{
  switch (value.unit)
  {
    case YGUnitAuto:
      autoSetter(yogaNode, edge);
      break;
    case YGUnitUndefined:
      normalSetter(yogaNode, edge, YGUndefined);
      break;
    case YGUnitPoint:
      normalSetter(yogaNode, edge, value.value);
      break;
    case YGUnitPercent:
      percentSetter(yogaNode, edge, value.value);
      break;
  }
}

static void StyleYogaNode(ShadowNodeBase& shadowNode, const YGNodeRef yogaNode, const folly::dynamic& props)
{
  if (props.empty())
    return;

  for (const auto& pair : props.items()) {
    const std::string& key = pair.first.getString();
    const auto& value = pair.second;

    if (key == "flexDirection") {
      YGFlexDirection direction = YGFlexDirectionColumn;

      if (value == "column" || value.isNull())
        direction = YGFlexDirectionColumn;
      else if (value == "row")
        direction = YGFlexDirectionRow;
      else if (value == "column-reverse")
        direction = YGFlexDirectionColumnReverse;
      else if (value == "row-reverse")
        direction = YGFlexDirectionRowReverse;
      else
        assert(false);

      YGNodeStyleSetFlexDirection(yogaNode, direction);
    }
    else if (key == "justifyContent") {
      YGJustify justify = YGJustifyFlexStart;

      if (value == "flex-start" || value.isNull())
        justify = YGJustifyFlexStart;
      else if (value == "flex-end")
        justify = YGJustifyFlexEnd;
      else if (value == "center")
        justify = YGJustifyCenter;
      else if (value == "space-between")
        justify = YGJustifySpaceBetween;
      else if (value == "space-around")
        justify = YGJustifySpaceAround;
      else if (value == "space-evenly")
        justify = YGJustifySpaceEvenly;
      else
        assert(false);

      YGNodeStyleSetJustifyContent(yogaNode, justify);
    }
    else if (key == "flexWrap") {
      YGWrap wrap = YGWrapNoWrap;

      if (value == "nowrap" || value.isNull())
        wrap = YGWrapNoWrap;
      else if (value == "wrap")
        wrap = YGWrapWrap;
      else
        assert(false);

      YGNodeStyleSetFlexWrap(yogaNode, wrap);
    }
    else if (key == "alignItems") {
      YGAlign align = YGAlignStretch;

      if (value == "stretch" || value.isNull())
        align = YGAlignStretch;
      else if (value == "flex-start")
        align = YGAlignFlexStart;
      else if (value == "flex-end")
        align = YGAlignFlexEnd;
      else if (value == "center")
        align = YGAlignCenter;
      else if (value == "baseline")
        align = YGAlignBaseline;
      else
        assert(false);

      YGNodeStyleSetAlignItems(yogaNode, align);
    }
    else if (key == "alignSelf") {
      YGAlign align = YGAlignAuto;

      if (value == "auto" || value.isNull())
        align = YGAlignAuto;
      if (value == "stretch")
        align = YGAlignStretch;
      else if (value == "flex-start")
        align = YGAlignFlexStart;
      else if (value == "flex-end")
        align = YGAlignFlexEnd;
      else if (value == "center")
        align = YGAlignCenter;
      else if (value == "baseline")
        align = YGAlignBaseline;
      else
        assert(false);

      YGNodeStyleSetAlignSelf(yogaNode, align);
    }
    else if (key == "alignContent") {
      YGAlign align = YGAlignFlexStart;

      if (value == "stretch")
        align = YGAlignStretch;
      else if (value == "flex-start" || value.isNull())
        align = YGAlignFlexStart;
      else if (value == "flex-end")
        align = YGAlignFlexEnd;
      else if (value == "center")
        align = YGAlignCenter;
      else if (value == "space-between")
        align = YGAlignSpaceBetween;
      else if (value == "space-around")
        align = YGAlignSpaceAround;
      else
        assert(false);

      YGNodeStyleSetAlignContent(yogaNode, align);
    }
    else if (key == "flex")
    {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetFlex(yogaNode, result);
    }
    else if (key == "flexGrow") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetFlexGrow(yogaNode, result);
    }
    else if (key == "flexShrink") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetFlexShrink(yogaNode, result);
    }
    else if (key == "flexBasis") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaUnitValueAutoHelper(yogaNode, result, YGNodeStyleSetFlexBasis, YGNodeStyleSetFlexBasisPercent, YGNodeStyleSetFlexBasisAuto);
    }
    else if (key == "position") {
      YGPositionType position = YGPositionTypeRelative;

      if (value == "relative" || value.isNull())
        position = YGPositionTypeRelative;
      else if (value == "absolute")
        position = YGPositionTypeAbsolute;
      else
        assert(false);

      YGNodeStyleSetPositionType(yogaNode, position);
    }
    else if (key == "overflow")
    {
      YGOverflow overflow = YGOverflowVisible;
      if (value == "visible" || value.isNull())
        overflow = YGOverflowVisible;
      else if (value == "hidden")
        overflow = YGOverflowHidden;
      else if (value == "scroll")
        overflow = YGOverflowScroll;

      YGNodeStyleSetOverflow(yogaNode, overflow);
    }
    else if (key == "display")
    {
      YGDisplay display = YGDisplayFlex;
      if (value == "flex" || value.isNull())
        display = YGDisplayFlex;
      else if (value == "none")
        display = YGDisplayNone;

      YGNodeStyleSetDisplay(yogaNode, display);
    }
    else if (key == "direction")
    {
      YGDirection direction = YGDirectionInherit;
      if (value == "inherit" || value.isNull())
        direction = YGDirectionInherit;
      else if (value == "ltr" || value.isNull())
        direction = YGDirectionLTR;
      else if (value == "rtl")
        direction = YGDirectionRTL;

      YGNodeStyleSetDirection(yogaNode, direction);
    }
    else if (key == "aspectRatio")
    {
      float result = NumberOrDefault(value, 1.0f /*default*/);

      YGNodeStyleSetAspectRatio(yogaNode, result);
    }
    else if (key == "left") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaValueHelper(yogaNode, YGEdgeLeft, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
    }
    else if (key == "top") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaValueHelper(yogaNode, YGEdgeTop, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
    }
    else if (key == "right") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaValueHelper(yogaNode, YGEdgeRight, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
    }
    else if (key == "bottom") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaValueHelper(yogaNode, YGEdgeBottom, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
    }
    else if (key == "end") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaValueHelper(yogaNode, YGEdgeEnd, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
    }
    else if (key == "start") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaValueHelper(yogaNode, YGEdgeStart, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
    }
    else if (key == "width") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaUnitValueAutoHelper(yogaNode, result, YGNodeStyleSetWidth, YGNodeStyleSetWidthPercent, YGNodeStyleSetWidthAuto);
    }
    else if (key == "minWidth") {
      YGValue result = YGValueOrDefault(value, YGValue{ 0.0f, YGUnitPoint } /*default*/);

      SetYogaUnitValueHelper(yogaNode, result, YGNodeStyleSetMinWidth, YGNodeStyleSetMinWidthPercent);
    }
    else if (key == "maxWidth") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaUnitValueHelper(yogaNode, result, YGNodeStyleSetMaxWidth, YGNodeStyleSetMaxWidthPercent);
    }
    else if (key == "height") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaUnitValueAutoHelper(yogaNode, result, YGNodeStyleSetHeight, YGNodeStyleSetHeightPercent, YGNodeStyleSetHeightAuto);
    }
    else if (key == "minHeight") {
      YGValue result = YGValueOrDefault(value, YGValue{ 0.0f, YGUnitPoint } /*default*/);

      SetYogaUnitValueHelper(yogaNode, result, YGNodeStyleSetMinHeight, YGNodeStyleSetMinHeightPercent);
    }
    else if (key == "maxHeight") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaUnitValueHelper(yogaNode, result, YGNodeStyleSetMaxHeight, YGNodeStyleSetMaxHeightPercent);
    }
    else if (key == "margin") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaValueAutoHelper(yogaNode, YGEdgeAll, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    }
    else if (key == "marginLeft") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaValueAutoHelper(yogaNode, YGEdgeLeft, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    }
    else if (key == "marginStart") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaValueAutoHelper(yogaNode, YGEdgeStart, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    }
    else if (key == "marginTop") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaValueAutoHelper(yogaNode, YGEdgeTop, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    }
    else if (key == "marginRight") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaValueAutoHelper(yogaNode, YGEdgeRight, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    }
    else if (key == "marginEnd") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaValueAutoHelper(yogaNode, YGEdgeEnd, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    }
    else if (key == "marginBottom") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaValueAutoHelper(yogaNode, YGEdgeBottom, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    }
    else if (key == "marginHorizontal") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaValueAutoHelper(yogaNode, YGEdgeHorizontal, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    }
    else if (key == "marginVertical") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaValueAutoHelper(yogaNode, YGEdgeVertical, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    }
    else if (key == "padding") {
      if (!shadowNode.ImplementsPadding()) {
        YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

        SetYogaValueHelper(yogaNode, YGEdgeAll, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
      }
    }
    else if (key == "paddingLeft") {
      if (!shadowNode.ImplementsPadding()) {
        YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

        SetYogaValueHelper(yogaNode, YGEdgeLeft, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
      }
    }
    else if (key == "paddingStart") {
      if (!shadowNode.ImplementsPadding()) {
        YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

        SetYogaValueHelper(yogaNode, YGEdgeStart, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
      }
    }
    else if (key == "paddingTop") {
      if (!shadowNode.ImplementsPadding()) {
        YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

        SetYogaValueHelper(yogaNode, YGEdgeTop, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
      }
    }
    else if (key == "paddingRight") {
      YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

      SetYogaValueHelper(yogaNode, YGEdgeRight, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
    }
    else if (key == "paddingEnd") {
      if (!shadowNode.ImplementsPadding()) {
        YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

        SetYogaValueHelper(yogaNode, YGEdgeEnd, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
      }
    }
    else if (key == "paddingBottom") {
      if (!shadowNode.ImplementsPadding()) {
        YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

        SetYogaValueHelper(yogaNode, YGEdgeBottom, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
      }
    }
    else if (key == "paddingHorizontal") {
      if (!shadowNode.ImplementsPadding()) {
        YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

        SetYogaValueHelper(yogaNode, YGEdgeHorizontal, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
      }
    }
    else if (key == "paddingVertical") {
      if (!shadowNode.ImplementsPadding()) {
        YGValue result = YGValueOrDefault(value, YGValue{ YGUndefined, YGUnitPoint } /*default*/);

        SetYogaValueHelper(yogaNode, YGEdgeVertical, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
      }
    }
    else if (key == "borderWidth") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetBorder(yogaNode, YGEdgeAll, result);
    }
    else if (key == "borderLeftWidth") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetBorder(yogaNode, YGEdgeLeft, result);
    }
    else if (key == "borderStartWidth") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetBorder(yogaNode, YGEdgeStart, result);
    }
    else if (key == "borderTopWidth") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetBorder(yogaNode, YGEdgeTop, result);
    }
    else if (key == "borderRightWidth") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetBorder(yogaNode, YGEdgeRight, result);
    }
    else if (key == "borderEndWidth") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetBorder(yogaNode, YGEdgeEnd, result);
    }
    else if (key == "borderBottomWidth") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetBorder(yogaNode, YGEdgeBottom, result);
    }
  }
}

void NativeUIManager::CreateView(facebook::react::ShadowNode& shadowNode, folly::dynamic /*ReadableMap*/ props)
{
  ShadowNodeBase& node = static_cast<ShadowNodeBase&>(shadowNode);
  auto* pViewManager = node.GetViewManager();

  if (pViewManager->RequiresYogaNode())
  {
    auto result = m_tagsToYogaNodes.emplace(node.m_tag, make_yoga_node());
    if (result.second == true)
    {
      YGNodeRef yogaNode = result.first->second.get();
      StyleYogaNode(node, yogaNode, props);

      YGMeasureFunc func = pViewManager->GetYogaCustomMeasureFunc();
      if (func != nullptr)
      {
        YGNodeSetMeasureFunc(yogaNode, func);

        auto context = std::make_unique<YogaContext>(node.GetView());
        YGNodeSetContext(yogaNode, reinterpret_cast<void*>(context.get()));

        m_tagsToYogaContext.emplace(node.m_tag, std::move(context));
      }
    }
  }
}

void NativeUIManager::AddView(facebook::react::ShadowNode& parentShadowNode, facebook::react::ShadowNode& childShadowNode, uint64_t index)
{
  ShadowNodeBase& parentNode = static_cast<ShadowNodeBase&>(parentShadowNode);
  auto* pViewManager = parentNode.GetViewManager();

  if (pViewManager->RequiresYogaNode() && !pViewManager->IsNativeControlWithSelfLayout())
  {
    YGNodeRef yogaNodeToManage = GetYogaNode(parentNode.m_tag);
    ShadowNodeBase& childNode = static_cast<ShadowNodeBase&>(childShadowNode);
    YGNodeRef yogaNodeToAdd = GetYogaNode(childNode.m_tag);

    int64_t oldParentTag = childNode.GetParent();
    YGNodeRef yogaOldParent = GetYogaNode(oldParentTag);
    if (yogaOldParent != nullptr)
    {
      YGNodeRemoveChild(yogaOldParent, yogaNodeToAdd);
    }

    YGNodeInsertChild(yogaNodeToManage, yogaNodeToAdd, static_cast<uint32_t>(index));
  }
}

void NativeUIManager::RemoveView(facebook::react::ShadowNode& shadowNode, bool removeChildren /*= true*/)
{
  ShadowNodeBase& node = static_cast<ShadowNodeBase&>(shadowNode);

  if (removeChildren)
  {
    auto* pViewManager = node.GetViewManager();

    YGNodeRef yogaNode = pViewManager->RequiresYogaNode() ? GetYogaNode(node.m_tag) : nullptr;
    if (yogaNode != nullptr && !pViewManager->IsNativeControlWithSelfLayout())
    {
      uint32_t childCount = YGNodeGetChildCount(yogaNode);
      for (uint32_t i = childCount; i > 0; --i)
      {
        YGNodeRef yogaNodeToRemove = YGNodeGetChild(yogaNode, i - 1);
        YGNodeRemoveChild(yogaNode, yogaNodeToRemove);
      }
    }
  }

  m_tagsToYogaNodes.erase(node.m_tag);
  m_tagsToYogaContext.erase(node.m_tag);
}

void NativeUIManager::ReplaceView(facebook::react::ShadowNode& shadowNode)
{
  ShadowNodeBase& node = static_cast<ShadowNodeBase&>(shadowNode);
  auto* pViewManager = node.GetViewManager();

  if (pViewManager->RequiresYogaNode())
  {
    auto it = m_tagsToYogaNodes.find(node.m_tag);
    if (it != m_tagsToYogaNodes.end())
    {
      YGNodeRef yogaNode = it->second.get();

      YGMeasureFunc func = pViewManager->GetYogaCustomMeasureFunc();
      if (func != nullptr)
      {
        auto context = std::make_unique<YogaContext>(node.GetView());
        YGNodeSetContext(yogaNode, reinterpret_cast<void*>(context.get()));

        m_tagsToYogaContext.emplace(node.m_tag, std::move(context));
      }
    }
    else
    {
      assert(false);
      return;
    }
  }
}

void NativeUIManager::UpdateView(facebook::react::ShadowNode& shadowNode, folly::dynamic /*ReadableMap*/ props)
{
  ShadowNodeBase& node = static_cast<ShadowNodeBase&>(shadowNode);
  auto* pViewManager = node.GetViewManager();

  if (pViewManager->RequiresYogaNode())
  {
    YGNodeRef yogaNode = GetYogaNode(node.m_tag);
    StyleYogaNode(node, yogaNode, props);
  }
}

void NativeUIManager::UpdateExtraLayout(int64_t tag)
{
  // For nodes that are not self-measure, there may be styles applied that are applying padding.
  // Here we make sure Yoga knows about that padding so yoga layout is aware of what rendering
  // intends to do with it.  (net: buttons with padding shouldn't have clipped content anymore)
  ShadowNodeBase* shadowNode = static_cast<ShadowNodeBase*>(m_host->FindShadowNodeForTag(tag));
  if (shadowNode == nullptr)
    return;

  if (shadowNode->IsExternalLayoutDirty())
  {
    YGNodeRef yogaNode = GetYogaNode(tag);
    if (yogaNode)
      shadowNode->DoExtraLayoutPrep(yogaNode);
  }

  for (int64_t child : shadowNode->m_children)
    UpdateExtraLayout(child);
}

void NativeUIManager::DoLayout()
{
  auto& rootTags = m_host->GetAllRootTags();
  for (int64_t rootTag : rootTags) {
    UpdateExtraLayout(rootTag);

    ShadowNodeBase& rootShadowNode = static_cast<ShadowNodeBase&>(m_host->GetShadowNodeForTag(rootTag));
    YGNodeRef rootNode = GetYogaNode(rootTag);
    auto rootElement = rootShadowNode.GetView().as<winrt::FrameworkElement>();

    float actualWidth = static_cast<float>(rootElement.ActualWidth());
    float actualHeight = static_cast<float>(rootElement.ActualHeight());

    // TODO: Real direction (VSO 1697992: RTL Layout)
    YGNodeCalculateLayout(rootNode, actualWidth, actualHeight, YGDirectionLTR);

    for (auto& tagToYogaNode : m_tagsToYogaNodes) {
      int64_t tag = tagToYogaNode.first;
      YGNodeRef yogaNode = tagToYogaNode.second.get();

      if (!YGNodeGetHasNewLayout(yogaNode))
        continue;
      YGNodeSetHasNewLayout(yogaNode, false);

      float left = YGNodeLayoutGetLeft(yogaNode);
      float top = YGNodeLayoutGetTop(yogaNode);
      float width = YGNodeLayoutGetWidth(yogaNode);
      float height = YGNodeLayoutGetHeight(yogaNode);

      ShadowNodeBase& shadowNode = static_cast<ShadowNodeBase&>(m_host->GetShadowNodeForTag(tag));
      auto view = shadowNode.GetView();
      auto pViewManager = shadowNode.GetViewManager();
      pViewManager->SetLayoutProps(shadowNode, view, left, top, width, height);
    }
  }
}

winrt::Windows::Foundation::Rect GetRectOfElementInParentCoords(winrt::FrameworkElement element, winrt::UIElement parent)
{
  if (parent == nullptr)
  {
    assert(false);
    return winrt::Windows::Foundation::Rect();
  }

  winrt::Rect anchorRect;
  winrt::Point anchorTopLeft = winrt::Point(0, 0);

  winrt::GeneralTransform transform = element.TransformToVisual(parent);
  winrt::Point anchorTopLeftConverted = transform.TransformPoint(anchorTopLeft);

  anchorRect.X = anchorTopLeftConverted.X;
  anchorRect.Y = anchorTopLeftConverted.Y;
  anchorRect.Width = (float)element.ActualWidth();
  anchorRect.Height = (float)element.ActualHeight();

  return anchorRect;
}

void NativeUIManager::measure(facebook::react::ShadowNode& shadowNode, facebook::react::ShadowNode& shadowRoot, facebook::xplat::module::CxxModule::Callback callback)
{
  std::vector<folly::dynamic> args;
  ShadowNodeBase& node = static_cast<ShadowNodeBase&>(shadowNode);
  auto view = node.GetView();

  auto feView = view.as<winrt::FrameworkElement>();
  if (feView == nullptr)
  {
    callback(args);
    return;
  }

  // Retrieve the XAML element for the root view containing this view
  ShadowNodeBase& nodeRoot = static_cast<ShadowNodeBase&>(shadowRoot);
  XamlView xamlRootView = nodeRoot.GetView();
  if (xamlRootView == nullptr)
  {
    callback(args);
    return;
  }

  auto feRootView = xamlRootView.as<winrt::FrameworkElement>();

  winrt::Rect rectInParentCoords =
    GetRectOfElementInParentCoords(feView, feRootView);

  // TODO: The first two params are for the local position. It's unclear what this is exactly, but it is not used anyway.
  //  Either codify this non-use or determine if and how we can send the needed data.
  args.push_back(0.0f);
  args.push_back(0.0f);

  // Size
  args.push_back(rectInParentCoords.Width);
  args.push_back(rectInParentCoords.Height);

  // Global Position
  args.push_back(rectInParentCoords.X);
  args.push_back(rectInParentCoords.Y);

  callback(args);
}

}
}
