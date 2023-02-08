// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Input.h>
#include <UI.Xaml.Media.h>
#include <Views/ShadowNodeBase.h>
#include <cxxreact/SystraceSection.h>
#include "Modules/I18nManagerModule.h"
#include "NativeUIManager.h"

#include "CppWinRTIncludes.h"
#include "IXamlRootView.h"
#include "QuirkSettings.h"
#include "ReactRootViewTagGenerator.h"
#include "Unicode.h"

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace xaml;
using namespace xaml::Controls;
using namespace xaml::Media;
} // namespace winrt

using namespace facebook::react;

namespace Microsoft::ReactNative {

static YogaNodePtr make_yoga_node(YGConfigRef config) {
  YogaNodePtr result(YGNodeNewWithConfig(config));
  return result;
}

static inline bool YogaFloatEquals(float x, float y) {
  // Epsilon value of 0.0001f is taken from the YGFloatsEqual method in Yoga.
  return std::fabs(x - y) < 0.0001f;
};

#if defined(_DEBUG)
static int YogaLog(
    const YGConfigRef /*config*/,
    const YGNodeRef /*node*/,
    YGLogLevel /*level*/,
    const char *format,
    va_list args) {
  const int len = _vscprintf(format, args);
  std::string buffer(len + 1, '\0');
  vsnprintf_s(&buffer[0], len + 1, _TRUNCATE, format, args);
  buffer.resize(len);

  // OutputDebugString will truncate output around 4k, here
  // we output a 1000 characters at a time.
  int start = 0;
  while (len - start > 1000) {
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
#endif

YGNodeRef NativeUIManager::GetYogaNode(int64_t tag) const {
  auto iter = m_tagsToYogaNodes.find(tag);
  if (iter == m_tagsToYogaNodes.end())
    return nullptr;
  return iter->second.get();
}

void NativeUIManager::DirtyYogaNode(int64_t tag) {
  ShadowNodeBase *pShadowNodeChild = static_cast<ShadowNodeBase *>(m_host->FindShadowNodeForTag(tag));
  if (pShadowNodeChild != nullptr) {
    auto *pViewManager = pShadowNodeChild->GetViewManager();
    YGMeasureFunc func = pViewManager->GetYogaCustomMeasureFunc();
    if (func != nullptr) {
      // If there is a yoga node for this tag mark it as dirty
      YGNodeRef yogaNodeChild = GetYogaNode(tag);
      if (yogaNodeChild != nullptr) {
        // Retrieve and dirty the yoga node
        YGNodeMarkDirty(yogaNodeChild);

        // Once we mark a node dirty we can stop because the yoga code will mark
        // all parents anyway
        return;
      }
    }

    // Since this node didn't meet the criteria, jump to parent in case it does
    int64_t parentTag = pShadowNodeChild->GetParent();
    if (parentTag >= 0)
      DirtyYogaNode(parentTag);
  }
}

void NativeUIManager::AddBatchCompletedCallback(std::function<void()> callback) {
  m_batchCompletedCallbacks.push_back(std::move(callback));
}

winrt::XamlRoot NativeUIManager::tryGetXamlRoot() {
  if (m_host) {
    for (auto const tag : m_host->GetAllRootTags()) {
      if (auto shadowNode = static_cast<ShadowNodeBase *>(m_host->FindShadowNodeForTag(tag))) {
        if (auto uiElement10 = shadowNode->GetView().try_as<xaml::IUIElement10>()) {
          return uiElement10.XamlRoot();
        }
      }
    }
  }
  return nullptr;
}

winrt::XamlRoot NativeUIManager::tryGetXamlRoot(int64_t rootTag) {
  if (m_host) {
    if (auto shadowNode = static_cast<ShadowNodeBase *>(m_host->FindShadowNodeForTag(rootTag))) {
      if (auto uiElement10 = shadowNode->GetView().try_as<xaml::IUIElement10>()) {
        return uiElement10.XamlRoot();
      }
    }
  }
  return nullptr;
}

XamlView NativeUIManager::reactPeerOrContainerFrom(xaml::FrameworkElement fe) {
  if (m_host) {
    while (fe) {
      if (auto value = GetTagAsPropertyValue(fe)) {
        auto tag = GetTag(value);
        if (auto shadowNode = static_cast<ShadowNodeBase *>(m_host->FindShadowNodeForTag(tag))) {
          if (auto xamlView = shadowNode->GetView()) {
            if (xamlView == fe) {
              return xamlView;
            }
          }
        }
      }
      fe = fe.Parent().try_as<xaml::FrameworkElement>();
    }
  }
  return nullptr;
}

NativeUIManager::NativeUIManager(winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : m_context(reactContext) {
  m_yogaConfig = YGConfigNew();
  if (React::implementation::QuirkSettings::GetUseWebFlexBasisBehavior(m_context.Properties()))
    YGConfigSetExperimentalFeatureEnabled(m_yogaConfig, YGExperimentalFeatureWebFlexBasis, true);
  if (React::implementation::QuirkSettings::GetMatchAndroidAndIOSStretchBehavior(m_context.Properties()))
    YGConfigSetUseLegacyStretchBehaviour(m_yogaConfig, true);

#if defined(_DEBUG)
  YGConfigSetLogger(m_yogaConfig, &YogaLog);

  // To Debug Yoga layout, uncomment the following line.
  // YGConfigSetPrintTreeFlag(m_yogaConfig, true);

  // Additional logging can be enabled editing yoga.cpp (e.g. gPrintChanges,
  // gPrintSkips)
#endif
}

struct RootShadowNode final : public ShadowNodeBase {
  RootShadowNode(const RootShadowNode &that) = delete;
  RootShadowNode &operator=(RootShadowNode const &) = delete;
  RootShadowNode() = delete;

  RootShadowNode(facebook::react::IReactRootView *rootView, INativeUIManagerHost *host) {
    auto reactRootView = static_cast<IXamlRootView *>(rootView);
    m_view = reactRootView->GetXamlView();
  }

  void createView(const winrt::Microsoft::ReactNative::JSValueObject &) override {
    // ASSERT: The root view is created before react, so react should never tell
    // the root view to be created.
    assert(false);
  }

  void AddView(ShadowNode &child, int64_t index) override {
    auto panel(GetView().as<winrt::Microsoft::ReactNative::ReactRootView>());
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactRootView>(panel)->AddView(
        static_cast<uint32_t>(index), static_cast<ShadowNodeBase &>(child).GetView().as<xaml::UIElement>());
  }
};

void NativeUIManager::setHost(INativeUIManagerHost *host) {
  m_host = host;
}

ShadowNode *NativeUIManager::createRootShadowNode(facebook::react::IReactRootView *pReactRootView) {
  return new RootShadowNode(pReactRootView, m_host);
}

void NativeUIManager::destroyRootShadowNode(ShadowNode *node) {
  delete node;
}

int64_t NativeUIManager::AddMeasuredRootView(facebook::react::IReactRootView *rootView) {
  auto tag = getNextRootViewTag();

  int64_t width = rootView->GetActualWidth();
  int64_t height = rootView->GetActualHeight();

  m_host->RegisterRootView(rootView, tag, width, height);

  return tag;
}

void NativeUIManager::AddRootView(ShadowNode &shadowNode, facebook::react::IReactRootView *pReactRootView) {
  SystraceSection s("NativeUIManager::AddRootView");
  auto xamlRootView = static_cast<IXamlRootView *>(pReactRootView);
  XamlView view = xamlRootView->GetXamlView();
  m_tagsToXamlReactControl.emplace(
      shadowNode.m_tag,
      winrt::weak_ref<winrt::Microsoft::ReactNative::ReactRootView>(
          view.as<winrt::Microsoft::ReactNative::ReactRootView>()));

  // Push the appropriate FlowDirection into the root view.
  view.as<xaml::FrameworkElement>().FlowDirection(
      I18nManager::IsRTL(m_context.Properties()) ? xaml::FlowDirection::RightToLeft : xaml::FlowDirection::LeftToRight);

  m_tagsToYogaNodes.emplace(shadowNode.m_tag, make_yoga_node(m_yogaConfig));

  auto element = view.as<xaml::FrameworkElement>();
  Microsoft::ReactNative::SetTag(element, shadowNode.m_tag);

  // Add listener to size change so we can redo the layout when that happens
  m_sizeChangedVector.push_back(
      view.as<xaml::FrameworkElement>().SizeChanged(winrt::auto_revoke, [this](auto &&, auto &&) { DoLayout(); }));
}

void NativeUIManager::removeRootView(Microsoft::ReactNative::ShadowNode &shadow) {
  SystraceSection s("NativeUIManager::removeRootView");
  m_tagsToXamlReactControl.erase(shadow.m_tag);
  RemoveView(shadow, true);
}

void NativeUIManager::onBatchComplete() {
  SystraceSection s("NativeUIManager::onBatchComplete");
  if (m_inBatch) {
    DoLayout();
    m_inBatch = false;

    const auto callbacks = m_batchCompletedCallbacks;
    m_batchCompletedCallbacks.clear();
    for (const auto &callback : callbacks) {
      callback.operator()();
    }
  }
}

void NativeUIManager::ensureInBatch() {
  if (!m_inBatch)
    m_inBatch = true;
}

bool NativeUIManager::isInBatch() {
  return m_inBatch;
}

static float NumberOrDefault(const winrt::Microsoft::ReactNative::JSValue &value, float defaultValue) {
  float result = defaultValue;

  if (value.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
      value.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
    result = value.AsSingle();
  else if (value.IsNull())
    result = defaultValue;
  else if (value.Type() == winrt::Microsoft::ReactNative::JSValueType::String)
    result = std::stof(value.AsString());
  else
    assert(false);

  return result;
}

static YGValue YGValueOrDefault(
    const winrt::Microsoft::ReactNative::JSValue &value,
    YGValue defaultValue,
    ShadowNodeBase &shadowNode,
    const std::string &key) {
  YGValue result = defaultValue;

  if (value.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
      value.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
    return YGValue{value.AsSingle(), YGUnitPoint};

  if (value.IsNull())
    return defaultValue;

  if (value.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
    std::string str = value.AsString();
    if (str == "auto")
      return YGValue{YGUndefined, YGUnitAuto};
    if (str.length() > 0 && str.back() == '%') {
      str.pop_back();
      folly::dynamic pct(str);
      return YGValue{static_cast<float>(pct.asDouble()), YGUnitPercent};
    }
    if (str.length() > 2 && (str.compare(str.length() - 2, 2, "pt") || str.compare(str.length() - 2, 2, "px"))) {
      shadowNode.RedBox(
          "Value '" + value.AsString() + "' for " + key + " is invalid. Cannot be converted to YGValue. '" +
          str.substr((str.length() - 2), 2) + "' unit not needed. Simply use integer value.");
      return defaultValue;
    }
  }

  shadowNode.RedBox(
      "Value '" + value.AsString() + "' for " + key +
      " is invalid. Cannot be converted to YGValue. Did you forget the %? Otherwise, simply use integer value.");
  return defaultValue;
}

typedef void (*YogaSetterFunc)(const YGNodeRef yogaNode, const YGEdge edge, const float value);
static void SetYogaValueHelper(
    const YGNodeRef yogaNode,
    const YGEdge edge,
    const YGValue &value,
    YogaSetterFunc normalSetter,
    YogaSetterFunc percentSetter) {
  switch (value.unit) {
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
static void SetYogaUnitValueHelper(
    const YGNodeRef yogaNode,
    const YGValue &value,
    YogaUnitSetterFunc normalSetter,
    YogaUnitSetterFunc percentSetter) {
  switch (value.unit) {
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
static void SetYogaUnitValueAutoHelper(
    const YGNodeRef yogaNode,
    const YGValue &value,
    YogaUnitSetterFunc normalSetter,
    YogaUnitSetterFunc percentSetter,
    YogaAutoUnitSetterFunc autoSetter) {
  switch (value.unit) {
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
static void SetYogaValueAutoHelper(
    const YGNodeRef yogaNode,
    const YGEdge edge,
    const YGValue &value,
    YogaSetterFunc normalSetter,
    YogaSetterFunc percentSetter,
    YogaAutoSetterFunc autoSetter) {
  switch (value.unit) {
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

static void StyleYogaNode(
    ShadowNodeBase &shadowNode,
    const YGNodeRef yogaNode,
    const winrt::Microsoft::ReactNative::JSValueObject &props) {
  for (const auto &pair : props) {
    const std::string &key = pair.first;
    const auto &value = pair.second;

    if (key == "flexDirection") {
      YGFlexDirection direction = YGFlexDirectionColumn;

      if (value == "column" || value.IsNull())
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
    } else if (key == "justifyContent") {
      YGJustify justify = YGJustifyFlexStart;

      if (value == "flex-start" || value.IsNull())
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
    } else if (key == "flexWrap") {
      YGWrap wrap = YGWrapNoWrap;

      if (value == "nowrap" || value.IsNull())
        wrap = YGWrapNoWrap;
      else if (value == "wrap")
        wrap = YGWrapWrap;
      else if (value == "wrap-reverse")
        wrap = YGWrapWrapReverse;
      else
        assert(false);

      YGNodeStyleSetFlexWrap(yogaNode, wrap);
    } else if (key == "alignItems") {
      YGAlign align = YGAlignStretch;

      if (value == "stretch" || value.IsNull())
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
    } else if (key == "alignSelf") {
      YGAlign align = YGAlignAuto;

      if (value == "auto" || value.IsNull())
        align = YGAlignAuto;
      else if (value == "stretch")
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
    } else if (key == "alignContent") {
      YGAlign align = YGAlignFlexStart;

      if (value == "stretch")
        align = YGAlignStretch;
      else if (value == "flex-start" || value.IsNull())
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
    } else if (key == "flex") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetFlex(yogaNode, result);
    } else if (key == "flexGrow") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetFlexGrow(yogaNode, result);
    } else if (key == "flexShrink") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetFlexShrink(yogaNode, result);
    } else if (key == "flexBasis") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaUnitValueAutoHelper(
          yogaNode, result, YGNodeStyleSetFlexBasis, YGNodeStyleSetFlexBasisPercent, YGNodeStyleSetFlexBasisAuto);
    } else if (key == "position") {
      YGPositionType position = YGPositionTypeRelative;

      if (value == "relative" || value.IsNull())
        position = YGPositionTypeRelative;
      else if (value == "absolute")
        position = YGPositionTypeAbsolute;
      else if (value == "static")
        position = YGPositionTypeStatic;
      else
        assert(false);

      YGNodeStyleSetPositionType(yogaNode, position);
    } else if (key == "overflow") {
      YGOverflow overflow = YGOverflowVisible;
      if (value == "visible" || value.IsNull())
        overflow = YGOverflowVisible;
      else if (value == "hidden")
        overflow = YGOverflowHidden;
      else if (value == "scroll")
        overflow = YGOverflowScroll;

      YGNodeStyleSetOverflow(yogaNode, overflow);
    } else if (key == "display") {
      YGDisplay display = YGDisplayFlex;
      if (value == "flex" || value.IsNull())
        display = YGDisplayFlex;
      else if (value == "none")
        display = YGDisplayNone;

      YGNodeStyleSetDisplay(yogaNode, display);
    } else if (key == "direction") {
      // https://github.com/microsoft/react-native-windows/issues/4668
      // In order to support the direction property, we tell yoga to always layout
      // in LTR direction, then push the appropriate FlowDirection into XAML.
      // This way XAML handles flipping in RTL mode, which works both for RN components
      // as well as native components that have purely XAML sub-trees (eg ComboBox).
      YGDirection direction = YGDirectionLTR;

      YGNodeStyleSetDirection(yogaNode, direction);
    } else if (key == "aspectRatio") {
      float result = NumberOrDefault(value, 1.0f /*default*/);

      YGNodeStyleSetAspectRatio(yogaNode, result);
    } else if (key == "left") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaValueHelper(yogaNode, YGEdgeLeft, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
    } else if (key == "top") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaValueHelper(yogaNode, YGEdgeTop, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
    } else if (key == "right") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaValueHelper(yogaNode, YGEdgeRight, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
    } else if (key == "bottom") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaValueHelper(yogaNode, YGEdgeBottom, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
    } else if (key == "end") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaValueHelper(yogaNode, YGEdgeEnd, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
    } else if (key == "start") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaValueHelper(yogaNode, YGEdgeStart, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
    } else if (key == "width") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaUnitValueAutoHelper(
          yogaNode, result, YGNodeStyleSetWidth, YGNodeStyleSetWidthPercent, YGNodeStyleSetWidthAuto);
    } else if (key == "minWidth") {
      YGValue result = YGValueOrDefault(value, YGValue{0.0f, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaUnitValueHelper(yogaNode, result, YGNodeStyleSetMinWidth, YGNodeStyleSetMinWidthPercent);
    } else if (key == "maxWidth") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaUnitValueHelper(yogaNode, result, YGNodeStyleSetMaxWidth, YGNodeStyleSetMaxWidthPercent);
    } else if (key == "height") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaUnitValueAutoHelper(
          yogaNode, result, YGNodeStyleSetHeight, YGNodeStyleSetHeightPercent, YGNodeStyleSetHeightAuto);
    } else if (key == "minHeight") {
      YGValue result = YGValueOrDefault(value, YGValue{0.0f, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaUnitValueHelper(yogaNode, result, YGNodeStyleSetMinHeight, YGNodeStyleSetMinHeightPercent);
    } else if (key == "maxHeight") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaUnitValueHelper(yogaNode, result, YGNodeStyleSetMaxHeight, YGNodeStyleSetMaxHeightPercent);
    } else if (key == "rowGap") {
      float result = NumberOrDefault(value, 0.0f /*default*/);
      YGNodeStyleSetGap(yogaNode, YGGutterRow, result);
    } else if (key == "columnGap") {
      float result = NumberOrDefault(value, 0.0f /*default*/);
      YGNodeStyleSetGap(yogaNode, YGGutterColumn, result);
    } else if (key == "gap") {
      float result = NumberOrDefault(value, 0.0f /*default*/);
      YGNodeStyleSetGap(yogaNode, YGGutterAll, result);
    } else if (key == "margin") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaValueAutoHelper(
          yogaNode, YGEdgeAll, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    } else if (key == "marginLeft") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaValueAutoHelper(
          yogaNode, YGEdgeLeft, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    } else if (key == "marginStart") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaValueAutoHelper(
          yogaNode, YGEdgeStart, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    } else if (key == "marginTop") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaValueAutoHelper(
          yogaNode, YGEdgeTop, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    } else if (key == "marginRight") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaValueAutoHelper(
          yogaNode, YGEdgeRight, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    } else if (key == "marginEnd") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaValueAutoHelper(
          yogaNode, YGEdgeEnd, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    } else if (key == "marginBottom") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaValueAutoHelper(
          yogaNode, YGEdgeBottom, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    } else if (key == "marginHorizontal") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaValueAutoHelper(
          yogaNode,
          YGEdgeHorizontal,
          result,
          YGNodeStyleSetMargin,
          YGNodeStyleSetMarginPercent,
          YGNodeStyleSetMarginAuto);
    } else if (key == "marginVertical") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaValueAutoHelper(
          yogaNode,
          YGEdgeVertical,
          result,
          YGNodeStyleSetMargin,
          YGNodeStyleSetMarginPercent,
          YGNodeStyleSetMarginAuto);
    } else if (key == "padding") {
      if (!shadowNode.ImplementsPadding()) {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueHelper(yogaNode, YGEdgeAll, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
      }
    } else if (key == "paddingLeft") {
      if (!shadowNode.ImplementsPadding()) {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueHelper(yogaNode, YGEdgeLeft, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
      }
    } else if (key == "paddingStart") {
      if (!shadowNode.ImplementsPadding()) {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueHelper(yogaNode, YGEdgeStart, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
      }
    } else if (key == "paddingTop") {
      if (!shadowNode.ImplementsPadding()) {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueHelper(yogaNode, YGEdgeTop, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
      }
    } else if (key == "paddingRight") {
      YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

      SetYogaValueHelper(yogaNode, YGEdgeRight, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
    } else if (key == "paddingEnd") {
      if (!shadowNode.ImplementsPadding()) {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueHelper(yogaNode, YGEdgeEnd, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
      }
    } else if (key == "paddingBottom") {
      if (!shadowNode.ImplementsPadding()) {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueHelper(yogaNode, YGEdgeBottom, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
      }
    } else if (key == "paddingHorizontal") {
      if (!shadowNode.ImplementsPadding()) {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueHelper(yogaNode, YGEdgeHorizontal, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
      }
    } else if (key == "paddingVertical") {
      if (!shadowNode.ImplementsPadding()) {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueHelper(yogaNode, YGEdgeVertical, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
      }
    } else if (key == "borderWidth") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetBorder(yogaNode, YGEdgeAll, result);
    } else if (key == "borderLeftWidth") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetBorder(yogaNode, YGEdgeLeft, result);
    } else if (key == "borderStartWidth") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetBorder(yogaNode, YGEdgeStart, result);
    } else if (key == "borderTopWidth") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetBorder(yogaNode, YGEdgeTop, result);
    } else if (key == "borderRightWidth") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetBorder(yogaNode, YGEdgeRight, result);
    } else if (key == "borderEndWidth") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetBorder(yogaNode, YGEdgeEnd, result);
    } else if (key == "borderBottomWidth") {
      float result = NumberOrDefault(value, 0.0f /*default*/);

      YGNodeStyleSetBorder(yogaNode, YGEdgeBottom, result);
    }
  }
}

void NativeUIManager::CreateView(ShadowNode &shadowNode, React::JSValueObject &props) {
  SystraceSection s("NativeUIManager::CreateView");
  ShadowNodeBase &node = static_cast<ShadowNodeBase &>(shadowNode);
  auto *pViewManager = node.GetViewManager();

  if (pViewManager->RequiresYogaNode()) {
    // Generate list of RN controls that need to have layout rerun on them.
    if (node.NeedsForceLayout()) {
      m_extraLayoutNodes.push_back(node.m_tag);
    }

    auto result = m_tagsToYogaNodes.emplace(node.m_tag, make_yoga_node(m_yogaConfig));
    if (result.second == true) {
      YGNodeRef yogaNode = result.first->second.get();
      StyleYogaNode(node, yogaNode, props);

      YGMeasureFunc func = pViewManager->GetYogaCustomMeasureFunc();
      if (func != nullptr) {
        YGNodeSetMeasureFunc(yogaNode, func);

        auto context = std::make_unique<Microsoft::ReactNative::YogaContext>(node.GetView());
        YGNodeSetContext(yogaNode, reinterpret_cast<void *>(context.get()));

        m_tagsToYogaContext.emplace(node.m_tag, std::move(context));
      }
    }
  }
}

void NativeUIManager::AddView(ShadowNode &parentShadowNode, ShadowNode &childShadowNode, uint64_t index) {
  ShadowNodeBase &parentNode = static_cast<ShadowNodeBase &>(parentShadowNode);
  auto *pViewManager = parentNode.GetViewManager();

  if (pViewManager->RequiresYogaNode() && !pViewManager->IsNativeControlWithSelfLayout()) {
    YGNodeRef yogaNodeToManage = GetYogaNode(parentNode.m_tag);
    ShadowNodeBase &childNode = static_cast<ShadowNodeBase &>(childShadowNode);
    YGNodeRef yogaNodeToAdd = GetYogaNode(childNode.m_tag);

    int64_t oldParentTag = childNode.GetParent();
    YGNodeRef yogaOldParent = GetYogaNode(oldParentTag);
    if (yogaOldParent != nullptr) {
      YGNodeRemoveChild(yogaOldParent, yogaNodeToAdd);
    }

    YGNodeInsertChild(yogaNodeToManage, yogaNodeToAdd, static_cast<uint32_t>(index));
  }
}

void NativeUIManager::RemoveView(ShadowNode &shadowNode, bool removeChildren /*= true*/) {
  ShadowNodeBase &node = static_cast<ShadowNodeBase &>(shadowNode);

  if (removeChildren) {
    auto *pViewManager = node.GetViewManager();

    YGNodeRef yogaNode = pViewManager->RequiresYogaNode() ? GetYogaNode(node.m_tag) : nullptr;
    if (yogaNode != nullptr && !pViewManager->IsNativeControlWithSelfLayout()) {
      uint32_t childCount = YGNodeGetChildCount(yogaNode);
      for (uint32_t i = childCount; i > 0; --i) {
        YGNodeRef yogaNodeToRemove = YGNodeGetChild(yogaNode, i - 1);
        YGNodeRemoveChild(yogaNode, yogaNodeToRemove);
      }
    }
  }

  m_tagsToYogaNodes.erase(node.m_tag);
  m_tagsToYogaContext.erase(node.m_tag);
}

void NativeUIManager::ReplaceView(ShadowNode &shadowNode) {
  ShadowNodeBase &node = static_cast<ShadowNodeBase &>(shadowNode);
  auto *pViewManager = node.GetViewManager();

  if (pViewManager->RequiresYogaNode()) {
    auto it = m_tagsToYogaNodes.find(node.m_tag);
    if (it != m_tagsToYogaNodes.end()) {
      YGNodeRef yogaNode = it->second.get();

      if (pViewManager->IsNativeControlWithSelfLayout()) {
        auto context = std::make_unique<YogaContext>(node.GetView());
        YGNodeSetContext(yogaNode, reinterpret_cast<void *>(context.get()));

        m_tagsToYogaContext.erase(node.m_tag);
        m_tagsToYogaContext.emplace(node.m_tag, std::move(context));
      }
    } else {
      assert(false);
      return;
    }
  }
}

void NativeUIManager::UpdateView(ShadowNode &shadowNode, winrt::Microsoft::ReactNative::JSValueObject &props) {
  SystraceSection s("NativeUIManager::UpdateView");
  ShadowNodeBase &node = static_cast<ShadowNodeBase &>(shadowNode);
  auto *pViewManager = node.GetViewManager();

  if (pViewManager->RequiresYogaNode()) {
    YGNodeRef yogaNode = GetYogaNode(node.m_tag);
    StyleYogaNode(node, yogaNode, props);
  }
}

void NativeUIManager::DoLayout() {
  SystraceSection s("NativeUIManager::DoLayout");

  {
    SystraceSection s("NativeUIManager::DoLayout::UpdateLayout");
    // Process vector of RN controls needing extra layout here.
    const auto extraLayoutNodes = m_extraLayoutNodes;
    for (const int64_t tag : extraLayoutNodes) {
      ShadowNodeBase *node = static_cast<ShadowNodeBase *>(m_host->FindShadowNodeForTag(tag));
      if (node) {
        auto element = node->GetView().as<xaml::FrameworkElement>();
        element.UpdateLayout();
      }
    }
    // Values need to be cleared from the vector before next call to DoLayout.
    m_extraLayoutNodes.clear();
  }

  auto &rootTags = m_host->GetAllRootTags();
  for (int64_t rootTag : rootTags) {
    ShadowNodeBase &rootShadowNode = static_cast<ShadowNodeBase &>(m_host->GetShadowNodeForTag(rootTag));
    const auto rootElement = rootShadowNode.GetView().as<xaml::FrameworkElement>();
    float actualWidth = static_cast<float>(rootElement.ActualWidth());
    float actualHeight = static_cast<float>(rootElement.ActualHeight());
    ApplyLayout(rootTag, actualWidth, actualHeight);
  }
}

void NativeUIManager::ApplyLayout(int64_t tag, float width, float height) {
  if (YGNodeRef rootNode = GetYogaNode(tag)) {
    SystraceSection s("NativeUIManager::DoLayout::YGNodeCalculateLayout");
    // We must always run layout in LTR mode, which might seem unintuitive.
    // We will flip the root of the tree into RTL by forcing the root XAML node's FlowDirection to RightToLeft
    // which will inherit down the XAML tree, allowing all native controls to pick it up.
    YGNodeCalculateLayout(rootNode, width, height, YGDirectionLTR);
  } else {
    assert(false);
    return;
  }

  {
    SystraceSection s("NativeUIManager::DoLayout::SetLayoutProps");
    SetLayoutPropsRecursive(tag);
  }
}

void NativeUIManager::SetLayoutPropsRecursive(int64_t tag) {
  ShadowNodeBase &shadowNode = static_cast<ShadowNodeBase &>(m_host->GetShadowNodeForTag(tag));
  auto *pViewManager = shadowNode.GetViewManager();
  if (!pViewManager->IsNativeControlWithSelfLayout()) {
    for (const auto child : shadowNode.m_children) {
      SetLayoutPropsRecursive(child);
    }
  }

  const auto tagToYogaNode = m_tagsToYogaNodes.find(tag);
  if (auto yogaNode = GetYogaNode(tag)) {
    if (!YGNodeGetHasNewLayout(yogaNode))
      return;
    YGNodeSetHasNewLayout(yogaNode, false);

    float left = YGNodeLayoutGetLeft(yogaNode);
    float top = YGNodeLayoutGetTop(yogaNode);
    float width = YGNodeLayoutGetWidth(yogaNode);
    float height = YGNodeLayoutGetHeight(yogaNode);
    auto view = shadowNode.GetView();
    auto pViewManager = shadowNode.GetViewManager();
    pViewManager->SetLayoutProps(shadowNode, view, left, top, width, height);
    if (shadowNode.m_onLayoutRegistered) {
      const auto hasLayoutChanged = !YogaFloatEquals(left, shadowNode.m_layout.Left) ||
          !YogaFloatEquals(top, shadowNode.m_layout.Top) || !YogaFloatEquals(width, shadowNode.m_layout.Width) ||
          !YogaFloatEquals(height, shadowNode.m_layout.Height);
      if (hasLayoutChanged) {
        React::JSValueObject layout{{"x", left}, {"y", top}, {"height", height}, {"width", width}};
        React::JSValueObject eventData{{"target", tag}, {"layout", std::move(layout)}};
        pViewManager->DispatchCoalescingEvent(tag, L"topLayout", MakeJSValueWriter(std::move(eventData)));
      }
    }
    shadowNode.m_layout = {left, top, width, height};
  }
}

winrt::Windows::Foundation::Rect GetRectOfElementInParentCoords(
    xaml::FrameworkElement element,
    xaml::UIElement parent) {
  if (parent == nullptr) {
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

void NativeUIManager::measure(
    ShadowNode &shadowNode,
    ShadowNode &shadowRoot,
    std::function<void(double left, double top, double width, double height, double pageX, double pageY)> &&callback) {
  std::vector<folly::dynamic> args;
  ShadowNodeBase &node = static_cast<ShadowNodeBase &>(shadowNode);
  auto view = node.GetView();

  auto feView = view.try_as<xaml::FrameworkElement>();
  if (feView == nullptr) {
    m_context.JSDispatcher().Post([callback = std::move(callback)]() { callback(0, 0, 0, 0, 0, 0); });
    return;
  }

  // Traverse up the react node tree to find any windowed popups.
  // If there are none, then we use the top-level root provided by our caller.
  xaml::FrameworkElement feRootView = nullptr;
  int64_t rootTag = shadowNode.m_tag;
  int64_t childTag = rootTag;
  while (true) {
    auto &currNode = m_host->GetShadowNodeForTag(rootTag);
    if (currNode.m_parent == InvalidTag)
      break;
    ShadowNodeBase &rootNode = static_cast<ShadowNodeBase &>(currNode);
    if (rootNode.IsWindowed()) {
      ShadowNodeBase &childNode = static_cast<ShadowNodeBase &>(m_host->GetShadowNodeForTag(childTag));
      feRootView = childNode.GetView().try_as<xaml::FrameworkElement>();
      break;
    }
    childTag = currNode.m_tag;
    rootTag = currNode.m_parent;
  }

  if (feRootView == nullptr) {
    // Retrieve the XAML element for the root view containing this view
    if (auto xamlRootView = static_cast<ShadowNodeBase &>(shadowRoot).GetView()) {
      feRootView = xamlRootView.as<xaml::FrameworkElement>();
    }
    if (feRootView == nullptr) {
      m_context.JSDispatcher().Post([callback = std::move(callback)]() { callback(0, 0, 0, 0, 0, 0); });
      return;
    }
  }

  winrt::Rect rectInParentCoords = GetRectOfElementInParentCoords(feView, feRootView);

  // TODO: The first two params are for the local position. It's unclear what
  // this is exactly, but it is not used anyway.
  //  Either codify this non-use or determine if and how we can send the needed
  //  data.
  m_context.JSDispatcher().Post([callback = std::move(callback), react = rectInParentCoords]() {
    callback(0, 0, react.Width, react.Height, react.X, react.Y);
  });
}

void NativeUIManager::measureInWindow(
    ShadowNode &shadowNode,
    std::function<void(double x, double y, double width, double height)> &&callback) {
  std::vector<folly::dynamic> args;

  ShadowNodeBase &node = static_cast<ShadowNodeBase &>(shadowNode);

  if (auto view = node.GetView().try_as<xaml::FrameworkElement>()) {
    // When supplied with nullptr, TransformToVisual will return the position
    // relative to the root XAML element.
    auto windowTransform = view.TransformToVisual(nullptr);
    auto positionInWindow = windowTransform.TransformPoint({0, 0});

    m_context.JSDispatcher().Post(
        [callback = std::move(callback), pos = positionInWindow, w = view.ActualWidth(), h = view.ActualHeight()]() {
          callback(pos.X, pos.Y, w, h);
        });
    return;
  }

  m_context.JSDispatcher().Post([callback = std::move(callback)]() { callback(0, 0, 0, 0); });
}

void NativeUIManager::measureLayout(
    ShadowNode &shadowNode,
    ShadowNode &ancestorNode,
    std::function<void(React::JSValue const &)> &&errorCallback,
    std::function<void(double left, double top, double width, double height)> &&callback) {
  std::vector<folly::dynamic> args;
  try {
    const auto &target = static_cast<ShadowNodeBase &>(shadowNode);
    const auto &ancestor = static_cast<ShadowNodeBase &>(ancestorNode);
    const auto targetElement = target.GetView().as<xaml::FrameworkElement>();
    const auto ancenstorElement = ancestor.GetView().as<xaml::FrameworkElement>();

    const auto ancestorTransform = targetElement.TransformToVisual(ancenstorElement);
    const auto width = static_cast<float>(targetElement.ActualWidth());
    const auto height = static_cast<float>(targetElement.ActualHeight());
    const auto transformedBounds = ancestorTransform.TransformBounds(winrt::Rect(0, 0, width, height));

    m_context.JSDispatcher().Post([callback = std::move(callback), rect = transformedBounds]() {
      callback(rect.X, rect.Y, rect.Width, rect.Height);
    });
  } catch (winrt::hresult_error const &e) {
    m_context.JSDispatcher().Post([errorCallback = std::move(errorCallback), msg = e.message()]() {
      auto writer = React::MakeJSValueTreeWriter();
      writer.WriteString(msg);
      errorCallback(React::TakeJSValue(writer));
    });
  }
}

void NativeUIManager::findSubviewIn(
    ShadowNode &shadowNode,
    float x,
    float y,
    std::function<void(double nativeViewTag, double left, double top, double width, double height)> &&callback) {
  ShadowNodeBase &node = static_cast<ShadowNodeBase &>(shadowNode);
  auto view = node.GetView();

  auto rootUIView = view.try_as<xaml::UIElement>();
  if (rootUIView == nullptr) {
    m_context.JSDispatcher().Post([callback = std::move(callback)]() { callback(0, 0, 0, 0, 0); });
    return;
  }

  winrt::Point point{x, y};

  // Transform the point to app window coordinates because
  // FindElementsInHostCoordinate() work in that metric
  auto appWindowTransform = rootUIView.TransformToVisual(nullptr);
  auto pointInAppWindow = appWindowTransform.TransformPoint(point);

  // Perform hit test to find the top most z-index element that intersects with
  // the queried point
  auto hitTestElements = winrt::VisualTreeHelper::FindElementsInHostCoordinates(pointInAppWindow, rootUIView);

  int64_t foundTag{};
  xaml::FrameworkElement foundElement = nullptr;

  for (const auto &elem : hitTestElements) {
    if (foundElement = elem.try_as<xaml::FrameworkElement>()) {
      auto tag = GetTag(foundElement);
      if (tag != InvalidTag) {
        foundTag = tag;
        break;
      }
    }
  }

  if (foundElement == nullptr) {
    m_context.JSDispatcher().Post([callback = std::move(callback)]() { callback(0, 0, 0, 0, 0); });
    return;
  }

  m_context.JSDispatcher().Post(
      [callback = std::move(callback), foundTag, box = GetRectOfElementInParentCoords(foundElement, rootUIView)]() {
        callback(static_cast<double>(foundTag), box.X, box.Y, box.Width, box.Height);
      });
}

void NativeUIManager::focus(int64_t reactTag) {
  if (auto shadowNode = static_cast<ShadowNodeBase *>(m_host->FindShadowNodeForTag(reactTag))) {
    xaml::Input::FocusManager::TryFocusAsync(shadowNode->GetView(), winrt::FocusState::Programmatic);
  }
}

// Note: It's a known issue that blur on flyout/popup would dismiss them.
void NativeUIManager::blur(int64_t reactTag) {
  if (auto shadowNode = static_cast<ShadowNodeBase *>(m_host->FindShadowNodeForTag(reactTag))) {
    // Only blur if current UI is focused to avoid problem described in PR #2687
    const auto xamlRoot = tryGetXamlRoot(shadowNode->m_rootTag);
    if (shadowNode->GetView() == xaml::Input::FocusManager::GetFocusedElement(xamlRoot)) {
      if (auto reactControl = GetParentXamlReactControl(reactTag).get()) {
        reactControl.as<winrt::Microsoft::ReactNative::implementation::ReactRootView>()->blur(shadowNode->GetView());
      } else {
        assert(false);
      }
    }
  }
}

// The same react instance can be shared by multiple ReactControls.
// To reduce the dependency between modules, IXamlReactControl other than
// ReactControl is used here. To get the IXamlReactControl for any node, we
// first iterate its parent until reaching the root node. Then look up
// m_tagsToXamlReactControl to get the IXamlReactControl
winrt::weak_ref<winrt::Microsoft::ReactNative::ReactRootView> NativeUIManager::GetParentXamlReactControl(
    int64_t tag) const {
  if (auto shadowNode = m_host->FindShadowNodeForTag(tag)) {
    auto it = m_tagsToXamlReactControl.find(shadowNode->m_rootTag);
    if (it != m_tagsToXamlReactControl.end()) {
      return it->second;
    }
  }
  return {};
}

} // namespace Microsoft::ReactNative
