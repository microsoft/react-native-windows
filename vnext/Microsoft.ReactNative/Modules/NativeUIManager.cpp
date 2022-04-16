// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Input.h>
#include <UI.Xaml.Media.h>
#include <Views/ShadowNodeBase.h>
#include "Modules/I18nManagerModule.h"
#include "NativeUIManager.h"

#include "CppWinRTIncludes.h"
#include "IXamlRootView.h"
#include "QuirkSettings.h"
#include "ReactRootViewTagGenerator.h"
#include "Unicode.h"
#include "fixed_string.h"

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace xaml;
using namespace xaml::Controls;
using namespace xaml::Media;
} // namespace winrt

namespace Microsoft::ReactNative {

static YogaNodePtr make_yoga_node(YGConfigRef config) {
  YogaNodePtr result(YGNodeNewWithConfig(config));
  return result;
}

#if defined(_DEBUG)
static int YogaLog(
    const YGConfigRef /*config*/,
    const YGNodeRef /*node*/,
    YGLogLevel /*level*/,
    const char *format,
    va_list args) {
  int len = _scprintf(format, args);
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

  // TODO: call UpdateRootNodeSize when ReactRootView size changes
  /*var resizeCount = 0;
  rootView.SetOnSizeChangedListener((sender, args) =>
  {
  var currentCount = ++resizeCount;
  var newWidth = args.NewSize.Width;
  var newHeight = args.NewSize.Height;

  Context.RunOnNativeModulesQueueThread(() =>
  {
  if (currentCount == resizeCount)
  {
  Context.AssertOnNativeModulesQueueThread();
  _uiImplementation.UpdateRootNodeSize(tag, newWidth, newHeight,
  _eventDispatcher);
  }
  });
  });*/

  return tag;
}

void NativeUIManager::AddRootView(ShadowNode &shadowNode, facebook::react::IReactRootView *pReactRootView) {
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
  m_tagsToXamlReactControl.erase(shadow.m_tag);
  RemoveView(shadow, true);
}

void NativeUIManager::onBatchComplete() {
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
    const auto &key = pair.first;
    const auto key_hash = details::hash(pair.first.c_str());
    const auto &value = pair.second;
    const auto value_hash = value.Type() == winrt::Microsoft::ReactNative::JSValueType::String
        ? details::hash(value.TryGetString()->c_str())
        : 0ull;

    switch (key_hash) {
      case HASH_STRING("flexDirection"): {
        YGFlexDirection direction;

        switch (value_hash) {
          case HASH_STRING("column"):
            direction = YGFlexDirectionColumn;
            break;
          case HASH_STRING("row"):
            direction = YGFlexDirectionRow;
            break;
          case HASH_STRING("column-reverse"):
            direction = YGFlexDirectionColumnReverse;
            break;
          case HASH_STRING("row-reverse"):
            direction = YGFlexDirectionRowReverse;
            break;
          default:
            direction = YGFlexDirectionColumn;
            assert(value.IsNull());
        }
        YGNodeStyleSetFlexDirection(yogaNode, direction);
        break;
      }
      case HASH_STRING("justifyContent"): {
        YGJustify justify;

        switch (value_hash) {
          case HASH_STRING("flex-start"):
            justify = YGJustifyFlexStart;
            break;
          case HASH_STRING("flex-end"):
            justify = YGJustifyFlexEnd;
            break;
          case HASH_STRING("center"):
            justify = YGJustifyCenter;
            break;
          case HASH_STRING("space-between"):
            justify = YGJustifySpaceBetween;
            break;
          case HASH_STRING("space-around"):
            justify = YGJustifySpaceAround;
            break;
          case HASH_STRING("space-evenly"):
            justify = YGJustifySpaceEvenly;
            break;
          default:
            justify = YGJustifyFlexStart;
            assert(value.IsNull());
        }
        YGNodeStyleSetJustifyContent(yogaNode, justify);
        break;
      }
      case HASH_STRING("flexWrap"): {
        YGWrap wrap;

        switch (value_hash) {
          case HASH_STRING("nowrap"):
            wrap = YGWrapNoWrap;
            break;
          case HASH_STRING("wrap"):
            wrap = YGWrapWrap;
            break;
          case HASH_STRING("wrap-reverse"):
            wrap = YGWrapWrapReverse;
            break;
          default:
            wrap = YGWrapNoWrap;
            assert(value.IsNull());

            break;
        }
        YGNodeStyleSetFlexWrap(yogaNode, wrap);
        break;
      }
      case HASH_STRING("alignItems"): {
        YGAlign align;

        switch (value_hash) {
          case HASH_STRING("stretch"):
            align = YGAlignStretch;
            break;
          case HASH_STRING("flex-start"):
            align = YGAlignFlexStart;
            break;
          case HASH_STRING("flex-end"):
            align = YGAlignFlexEnd;
            break;
          case HASH_STRING("center"):
            align = YGAlignCenter;
            break;
          case HASH_STRING("baseline"):
            align = YGAlignBaseline;
            break;
          default:
            align = YGAlignStretch;
            assert(value.IsNull());
            break;
        }
        YGNodeStyleSetAlignItems(yogaNode, align);
        break;
      }
      case HASH_STRING("alignSelf"): {
        YGAlign align;

        switch (value_hash) {
          case HASH_STRING("auto"):
            align = YGAlignAuto;
            break;
          case HASH_STRING("stretch"):
            align = YGAlignStretch;
            break;
          case HASH_STRING("flex-start"):
            align = YGAlignFlexStart;
            break;
          case HASH_STRING("flex-end"):
            align = YGAlignFlexEnd;
            break;
          case HASH_STRING("center"):
            align = YGAlignCenter;
            break;
          case HASH_STRING("baseline"):
            align = YGAlignBaseline;
            break;
          default:
            align = YGAlignAuto;
            assert(value.IsNull());

            break;
        }

        YGNodeStyleSetAlignSelf(yogaNode, align);
        break;
      }
      case HASH_STRING("alignContent"): {
        YGAlign align;

        switch (value_hash) {
          case HASH_STRING("stretch"):
            align = YGAlignStretch;
            break;
          case HASH_STRING("flex-start"):
            align = YGAlignFlexStart;
            break;
          case HASH_STRING("flex-end"):
            align = YGAlignFlexEnd;
            break;
          case HASH_STRING("center"):
            align = YGAlignCenter;
            break;
          case HASH_STRING("space-between"):
            align = YGAlignSpaceBetween;
            break;
          case HASH_STRING("space-around"):
            align = YGAlignSpaceAround;
            break;
          default:
            align = YGAlignFlexStart;
            assert(value.IsNull());
            break;
        }
        YGNodeStyleSetAlignContent(yogaNode, align);
        break;
      }
      case HASH_STRING("flex"): {
        float result = NumberOrDefault(value, 0.0f /*default*/);

        YGNodeStyleSetFlex(yogaNode, result);
        break;
      }
      case HASH_STRING("flexGrow"): {
        float result = NumberOrDefault(value, 0.0f /*default*/);

        YGNodeStyleSetFlexGrow(yogaNode, result);
        break;
      }
      case HASH_STRING("flexShrink"): {
        float result = NumberOrDefault(value, 0.0f /*default*/);

        YGNodeStyleSetFlexShrink(yogaNode, result);
        break;
      }
      case HASH_STRING("flexBasis"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaUnitValueAutoHelper(
            yogaNode, result, YGNodeStyleSetFlexBasis, YGNodeStyleSetFlexBasisPercent, YGNodeStyleSetFlexBasisAuto);
        break;
      }
      case HASH_STRING("position"): {
        YGPositionType position;

        switch (value_hash) {
          case HASH_STRING("relative"):
            position = YGPositionTypeRelative;
            break;
          case HASH_STRING("absolute"):
            position = YGPositionTypeAbsolute;
            break;
          case HASH_STRING("static"):
            position = YGPositionTypeStatic;
            break;
          default:
            position = YGPositionTypeRelative;
            assert(value.IsNull());
            break;
        }
        YGNodeStyleSetPositionType(yogaNode, position);
        break;
      }
      case HASH_STRING("overflow"): {
        YGOverflow overflow;
        switch (value_hash) {
          case HASH_STRING("visible"):
            overflow = YGOverflowVisible;
            break;
          case HASH_STRING("hidden"):
            overflow = YGOverflowHidden;
            break;
          case HASH_STRING("scroll"):
            overflow = YGOverflowScroll;
            break;
          default:
            overflow = YGOverflowVisible;
            assert(value.IsNull());
            break;
        }
        YGNodeStyleSetOverflow(yogaNode, overflow);
        break;
      }
      case HASH_STRING("display"): {
        YGDisplay display;
        switch (value_hash) {
          case HASH_STRING("flex"):
            display = YGDisplayFlex;
            break;
          case HASH_STRING("none"):
            display = YGDisplayNone;
            break;
          default:
            display = YGDisplayFlex;
            assert(value.IsNull());
            break;
        }

        YGNodeStyleSetDisplay(yogaNode, display);
        break;
      }
      case HASH_STRING("direction"): {
        // https://github.com/microsoft/react-native-windows/issues/4668
        // In order to support the direction property, we tell yoga to always layout
        // in LTR direction, then push the appropriate FlowDirection into XAML.
        // This way XAML handles flipping in RTL mode, which works both for RN components
        // as well as native components that have purely XAML sub-trees (eg ComboBox).
        YGDirection direction = YGDirectionLTR;

        YGNodeStyleSetDirection(yogaNode, direction);
        break;
      }
      case HASH_STRING("aspectRatio"): {
        float result = NumberOrDefault(value, 1.0f /*default*/);

        YGNodeStyleSetAspectRatio(yogaNode, result);
        break;
      }
      case HASH_STRING("left"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueHelper(yogaNode, YGEdgeLeft, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
        break;
      }
      case HASH_STRING("top"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueHelper(yogaNode, YGEdgeTop, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
        break;
      }
      case HASH_STRING("right"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueHelper(yogaNode, YGEdgeRight, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
        break;
      }
      case HASH_STRING("bottom"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueHelper(yogaNode, YGEdgeBottom, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
        break;
      }
      case HASH_STRING("end"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueHelper(yogaNode, YGEdgeEnd, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
        break;
      }
      case HASH_STRING("start"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueHelper(yogaNode, YGEdgeStart, result, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
        break;
      }
      case HASH_STRING("width"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaUnitValueAutoHelper(
            yogaNode, result, YGNodeStyleSetWidth, YGNodeStyleSetWidthPercent, YGNodeStyleSetWidthAuto);
        break;
      }
      case HASH_STRING("minWidth"): {
        YGValue result = YGValueOrDefault(value, YGValue{0.0f, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaUnitValueHelper(yogaNode, result, YGNodeStyleSetMinWidth, YGNodeStyleSetMinWidthPercent);
        break;
      }
      case HASH_STRING("maxWidth"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaUnitValueHelper(yogaNode, result, YGNodeStyleSetMaxWidth, YGNodeStyleSetMaxWidthPercent);
        break;
      }
      case HASH_STRING("height"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaUnitValueAutoHelper(
            yogaNode, result, YGNodeStyleSetHeight, YGNodeStyleSetHeightPercent, YGNodeStyleSetHeightAuto);
        break;
      }
      case HASH_STRING("minHeight"): {
        YGValue result = YGValueOrDefault(value, YGValue{0.0f, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaUnitValueHelper(yogaNode, result, YGNodeStyleSetMinHeight, YGNodeStyleSetMinHeightPercent);
        break;
      }
      case HASH_STRING("maxHeight"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaUnitValueHelper(yogaNode, result, YGNodeStyleSetMaxHeight, YGNodeStyleSetMaxHeightPercent);
        break;
      }
      case HASH_STRING("margin"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueAutoHelper(
            yogaNode, YGEdgeAll, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
        break;
      }
      case HASH_STRING("marginLeft"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueAutoHelper(
            yogaNode, YGEdgeLeft, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
        break;
      }
      case HASH_STRING("marginStart"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueAutoHelper(
            yogaNode, YGEdgeStart, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
        break;
      }
      case HASH_STRING("marginTop"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueAutoHelper(
            yogaNode, YGEdgeTop, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
        break;
      }
      case HASH_STRING("marginRight"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueAutoHelper(
            yogaNode, YGEdgeRight, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
        break;
      }
      case HASH_STRING("marginEnd"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueAutoHelper(
            yogaNode, YGEdgeEnd, result, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
        break;
      }
      case HASH_STRING("marginBottom"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueAutoHelper(
            yogaNode,
            YGEdgeBottom,
            result,
            YGNodeStyleSetMargin,
            YGNodeStyleSetMarginPercent,
            YGNodeStyleSetMarginAuto);
        break;
      }
      case HASH_STRING("marginHorizontal"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueAutoHelper(
            yogaNode,
            YGEdgeHorizontal,
            result,
            YGNodeStyleSetMargin,
            YGNodeStyleSetMarginPercent,
            YGNodeStyleSetMarginAuto);
        break;
      }
      case HASH_STRING("marginVertical"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueAutoHelper(
            yogaNode,
            YGEdgeVertical,
            result,
            YGNodeStyleSetMargin,
            YGNodeStyleSetMarginPercent,
            YGNodeStyleSetMarginAuto);
        break;
      }
      case HASH_STRING("padding"): {
        if (!shadowNode.ImplementsPadding()) {
          YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

          SetYogaValueHelper(yogaNode, YGEdgeAll, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
        }
        break;
      }
      case HASH_STRING("paddingLeft"): {
        if (!shadowNode.ImplementsPadding()) {
          YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

          SetYogaValueHelper(yogaNode, YGEdgeLeft, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
        }
        break;
      }
      case HASH_STRING("paddingStart"): {
        if (!shadowNode.ImplementsPadding()) {
          YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

          SetYogaValueHelper(yogaNode, YGEdgeStart, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
        }
        break;
      }
      case HASH_STRING("paddingTop"): {
        if (!shadowNode.ImplementsPadding()) {
          YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

          SetYogaValueHelper(yogaNode, YGEdgeTop, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
        }
        break;
      }
      case HASH_STRING("paddingRight"): {
        YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

        SetYogaValueHelper(yogaNode, YGEdgeRight, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
        break;
      }
      case HASH_STRING("paddingEnd"): {
        if (!shadowNode.ImplementsPadding()) {
          YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

          SetYogaValueHelper(yogaNode, YGEdgeEnd, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
        }
        break;
      }
      case HASH_STRING("paddingBottom"): {
        if (!shadowNode.ImplementsPadding()) {
          YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

          SetYogaValueHelper(yogaNode, YGEdgeBottom, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
        }
        break;
      }
      case HASH_STRING("paddingHorizontal"): {
        if (!shadowNode.ImplementsPadding()) {
          YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

          SetYogaValueHelper(yogaNode, YGEdgeHorizontal, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
        }
        break;
      }
      case HASH_STRING("paddingVertical"): {
        if (!shadowNode.ImplementsPadding()) {
          YGValue result = YGValueOrDefault(value, YGValue{YGUndefined, YGUnitPoint} /*default*/, shadowNode, key);

          SetYogaValueHelper(yogaNode, YGEdgeVertical, result, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
        }
        break;
      }
      case HASH_STRING("borderWidth"): {
        float result = NumberOrDefault(value, 0.0f /*default*/);

        YGNodeStyleSetBorder(yogaNode, YGEdgeAll, result);
        break;
      }
      case HASH_STRING("borderLeftWidth"): {
        float result = NumberOrDefault(value, 0.0f /*default*/);

        YGNodeStyleSetBorder(yogaNode, YGEdgeLeft, result);
        break;
      }
      case HASH_STRING("borderStartWidth"): {
        float result = NumberOrDefault(value, 0.0f /*default*/);

        YGNodeStyleSetBorder(yogaNode, YGEdgeStart, result);
        break;
      }
      case HASH_STRING("borderTopWidth"): {
        float result = NumberOrDefault(value, 0.0f /*default*/);

        YGNodeStyleSetBorder(yogaNode, YGEdgeTop, result);
        break;
      }
      case HASH_STRING("borderRightWidth"): {
        float result = NumberOrDefault(value, 0.0f /*default*/);

        YGNodeStyleSetBorder(yogaNode, YGEdgeRight, result);
        break;
      }
      case HASH_STRING("borderEndWidth"): {
        float result = NumberOrDefault(value, 0.0f /*default*/);

        YGNodeStyleSetBorder(yogaNode, YGEdgeEnd, result);
        break;
      }
      case HASH_STRING("borderBottomWidth"): {
        float result = NumberOrDefault(value, 0.0f /*default*/);

        YGNodeStyleSetBorder(yogaNode, YGEdgeBottom, result);
      }
    }
  }
}

void NativeUIManager::CreateView(ShadowNode &shadowNode, React::JSValueObject &props) {
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

      YGMeasureFunc func = pViewManager->GetYogaCustomMeasureFunc();
      if (func != nullptr) {
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
  ShadowNodeBase &node = static_cast<ShadowNodeBase &>(shadowNode);
  auto *pViewManager = node.GetViewManager();

  if (pViewManager->RequiresYogaNode()) {
    YGNodeRef yogaNode = GetYogaNode(node.m_tag);
    StyleYogaNode(node, yogaNode, props);
  }
}

void NativeUIManager::UpdateExtraLayout(int64_t tag) {
  // For nodes that are not self-measure, there may be styles applied that are
  // applying padding. Here we make sure Yoga knows about that padding so yoga
  // layout is aware of what rendering intends to do with it.  (net: buttons
  // with padding shouldn't have clipped content anymore)
  ShadowNodeBase *shadowNode = static_cast<ShadowNodeBase *>(m_host->FindShadowNodeForTag(tag));
  if (shadowNode == nullptr)
    return;

  if (shadowNode->IsExternalLayoutDirty()) {
    YGNodeRef yogaNode = GetYogaNode(tag);
    if (yogaNode)
      shadowNode->DoExtraLayoutPrep(yogaNode);
  }

  for (int64_t child : shadowNode->m_children) {
    UpdateExtraLayout(child);
  }
}

void NativeUIManager::DoLayout() {
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
  auto &rootTags = m_host->GetAllRootTags();
  for (int64_t rootTag : rootTags) {
    UpdateExtraLayout(rootTag);

    ShadowNodeBase &rootShadowNode = static_cast<ShadowNodeBase &>(m_host->GetShadowNodeForTag(rootTag));
    YGNodeRef rootNode = GetYogaNode(rootTag);
    auto rootElement = rootShadowNode.GetView().as<xaml::FrameworkElement>();

    float actualWidth = static_cast<float>(rootElement.ActualWidth());
    float actualHeight = static_cast<float>(rootElement.ActualHeight());

    // We must always run layout in LTR mode, which might seem unintuitive.
    // We will flip the root of the tree into RTL by forcing the root XAML node's FlowDirection to RightToLeft
    // which will inherit down the XAML tree, allowing all native controls to pick it up.
    YGNodeCalculateLayout(rootNode, actualWidth, actualHeight, YGDirectionLTR);
  }

  for (auto &tagToYogaNode : m_tagsToYogaNodes) {
    int64_t tag = tagToYogaNode.first;
    YGNodeRef yogaNode = tagToYogaNode.second.get();

    if (!YGNodeGetHasNewLayout(yogaNode))
      continue;
    YGNodeSetHasNewLayout(yogaNode, false);

    float left = YGNodeLayoutGetLeft(yogaNode);
    float top = YGNodeLayoutGetTop(yogaNode);
    float width = YGNodeLayoutGetWidth(yogaNode);
    float height = YGNodeLayoutGetHeight(yogaNode);

    ShadowNodeBase &shadowNode = static_cast<ShadowNodeBase &>(m_host->GetShadowNodeForTag(tag));
    auto view = shadowNode.GetView();
    auto pViewManager = shadowNode.GetViewManager();
    pViewManager->SetLayoutProps(shadowNode, view, left, top, width, height);
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
    auto windowTransform = view.TransformToVisual(xaml::Window::Current().Content());
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
    xaml::Input::FocusManager::TryFocusAsync(shadowNode->GetView(), winrt::FocusState::Keyboard);
  }
}

// Note: It's a known issue that blur on flyout/popup would dismiss them.
void NativeUIManager::blur(int64_t reactTag) {
  if (auto shadowNode = static_cast<ShadowNodeBase *>(m_host->FindShadowNodeForTag(reactTag))) {
    auto view = shadowNode->GetView();
    // Only blur if current UI is focused to avoid problem described in PR #2687
    if (view == xaml::Input::FocusManager::GetFocusedElement().try_as<xaml::DependencyObject>()) {
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
  if (auto shadowNode = static_cast<ShadowNodeBase *>(m_host->FindParentRootShadowNode(tag))) {
    auto it = m_tagsToXamlReactControl.find(shadowNode->m_tag);
    if (it != m_tagsToXamlReactControl.end()) {
      return it->second;
    }
  }
  return {};
}

} // namespace Microsoft::ReactNative
