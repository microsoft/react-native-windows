// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ABIViewManager.h"
#include "DynamicReader.h"
#include "DynamicWriter.h"

#include "IReactContext.h"

#include <JSValueWriter.h>
#include <Utils/ValueUtils.h>
#include <Views/ShadowNodeBase.h>
#include "ReactHost/MsoUtils.h"

#include "NativeMeasuringPanel.g.h"
#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>

namespace winrt::Microsoft::ReactNative::implementation {
struct NativeMeasuringPanel : NativeMeasuringPanelT<NativeMeasuringPanel> {
  using super = xaml::Controls::ContentControl;
  NativeMeasuringPanel() = default;
  Size MeasureOverride(Size const &available) {
    auto parent = this->Parent();
    auto parentType = winrt::get_class_name(parent);
    if (auto child = Content().as<xaml::FrameworkElement>()) {
      auto w = child.Width();
      auto h = child.Height();
      child.ClearValue(xaml::FrameworkElement::WidthProperty());
      child.ClearValue(xaml::FrameworkElement::HeightProperty());

      child.Measure({10000, 10000});
      auto ret = child.DesiredSize();
      this->Width(ret.Width);
      this->Height(ret.Height);
      child.Width(w);
      child.Height(h);

      if (ret != m_last && !m_isMeasuring) {
        m_isMeasuring = true;
        auto x = 0;
        auto &ctx = m_shadowNode->GetViewManager()->GetReactContext();
        if (auto nativeUIManager = ::Microsoft::ReactNative::GetNativeUIManager(ctx).lock()) {
          nativeUIManager->DirtyYogaNode(m_shadowNode->m_tag);
          nativeUIManager->ensureInBatch();
          nativeUIManager->onBatchComplete();
        }
        m_isMeasuring = false;
      }
      m_last = ret;
    }
    return m_last;
  }

  Size m_last{};
  ::Microsoft::ReactNative::ShadowNodeBase *m_shadowNode{nullptr};
  bool m_isMeasuring{false};
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct NativeMeasuringPanel : NativeMeasuringPanelT<NativeMeasuringPanel, implementation::NativeMeasuringPanel> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation

#if __has_include("NativeMeasuringPanel.g.cpp")
#include "NativeMeasuringPanel.g.cpp"
#endif

namespace winrt::Microsoft::ReactNative {

class ABIShadowNode : public ::Microsoft::ReactNative::ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  ABIShadowNode(bool needsForceLayout) : m_needsForceLayout(needsForceLayout) {}
  bool NeedsForceLayout() override {
    return m_needsForceLayout;
  }

  void createView(const winrt::Microsoft::ReactNative::JSValueObject &props) override {
    Super::createView(props);
    auto panel = winrt::get_self<implementation::NativeMeasuringPanel>(m_view.as<NativeMeasuringPanel>());
    panel->m_shadowNode = this;
  }

 private:
  bool m_needsForceLayout;
};

ABIViewManager::ABIViewManager(
    Mso::CntPtr<Mso::React::IReactContext> const &reactContext,
    ReactNative::IViewManager const &viewManager)
    : Super(*reactContext),
      m_name{viewManager.Name()},
      m_viewManager{viewManager},
      m_viewManagerWithReactContext{viewManager.try_as<IViewManagerWithReactContext>()},
      m_viewManagerWithExportedViewConstants{viewManager.try_as<IViewManagerWithExportedViewConstants>()},
      m_viewManagerWithNativeProperties{viewManager.try_as<IViewManagerWithNativeProperties>()},
      m_viewManagerWithCommands{viewManager.try_as<IViewManagerWithCommands>()},
      m_viewManagerWithExportedEventTypeConstants{viewManager.try_as<IViewManagerWithExportedEventTypeConstants>()},
      m_viewManagerRequiresNativeLayout{viewManager.try_as<IViewManagerRequiresNativeLayout>()},
      m_viewManagerWithChildren{viewManager.try_as<IViewManagerWithChildren>()},
      m_viewManagerWithPointerEvents{viewManager.try_as<IViewManagerWithPointerEvents>()},
      m_viewManagerWithDropViewInstance{viewManager.try_as<IViewManagerWithDropViewInstance>()} {
    if (m_viewManagerWithReactContext) {
    m_viewManagerWithReactContext.ReactContext(winrt::make<implementation::ReactContext>(Mso::Copy(reactContext)));
  }
  if (m_viewManagerWithNativeProperties) {
    m_nativeProps = m_viewManagerWithNativeProperties.NativeProps();
  }
}

const wchar_t *ABIViewManager::GetName() const {
  return m_name.c_str();
}

xaml::DependencyObject ABIViewManager::CreateViewCore(
    int64_t tag,
    const winrt::Microsoft::ReactNative::JSValueObject &props) {
  xaml::DependencyObject created{nullptr};
  if (auto viewCreateProps = m_viewManager.try_as<IViewManagerCreateWithProperties>()) {
    auto view = viewCreateProps.CreateViewWithProperties(
        MakeJSValueTreeReader(winrt::Microsoft::ReactNative::JSValue(props.Copy())));
    created = view.as<xaml::DependencyObject>();
  } else {
    created = m_viewManager.CreateView();
  }

  auto panel = NativeMeasuringPanel();

  panel.Content(created);
  return panel;
}

void ABIViewManager::GetExportedViewConstants(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetExportedViewConstants(writer);

  if (m_viewManagerWithExportedViewConstants) {
    if (auto constantProvider = m_viewManagerWithExportedViewConstants.ExportedViewConstants()) {
      IJSValueWriter argWriter = winrt::make<DynamicWriter>();
      constantProvider(writer);
    }
  }
}

void ABIViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);

  if (m_nativeProps) {
    for (const auto &pair : m_nativeProps) {
      writer.WritePropertyName(pair.Key());
      switch (pair.Value()) {
        case ViewManagerPropertyType::Boolean:
          writer.WriteString(L"boolean");
          break;
        case ViewManagerPropertyType::Number:
          writer.WriteString(L"number");
          break;
        case ViewManagerPropertyType::String:
          writer.WriteString(L"string");
          break;
        case ViewManagerPropertyType::Array:
          writer.WriteString(L"array");
          break;
        case ViewManagerPropertyType::Map:
          writer.WriteString(L"Map");
          break;
        case ViewManagerPropertyType::Color:
          writer.WriteString(L"Color");
          break;
        case ViewManagerPropertyType::Function:
          writer.WriteString(L"function");
          break;
      }
    }
  }
}

void ABIViewManager::UpdateProperties(
    ::Microsoft::ReactNative::ShadowNodeBase *nodeToUpdate,
    winrt::Microsoft::ReactNative::JSValueObject &props) {
  if (m_viewManagerRequiresNativeLayout && m_viewManagerRequiresNativeLayout.RequiresNativeLayout()) {
    MarkDirty(nodeToUpdate->m_tag);
  }

  if (m_viewManagerWithNativeProperties) {
    auto view = nodeToUpdate->GetView().as<NativeMeasuringPanel>().Content().as<xaml::FrameworkElement>();

    if (props.size() > 0) {
      m_viewManagerWithNativeProperties.UpdateProperties(
          view, MakeJSValueTreeReader(winrt::Microsoft::ReactNative::JSValue(props.Copy())));
    }
  }

  Super::UpdateProperties(nodeToUpdate, props);
}

void ABIViewManager::GetCommands(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  // Why are we providing commands with the same key and value? React Native 0.61 internally introduced string command
  // IDs which can be dispatched directly without querying the ViewManager for commands. Integer command IDs are
  // internally deprecated, but querying for command ID is still the documented path. Returning constants as their
  // string lets us internally only support the string path.
  if (m_viewManagerWithCommands) {
    for (const auto &commandName : m_viewManagerWithCommands.Commands()) {
      winrt::Microsoft::ReactNative::WriteProperty(writer, commandName, commandName);
    }
  }
}

void ABIViewManager::DispatchCommand(
    const xaml::DependencyObject &viewToUpdate,
    const std::string &commandId,
    winrt::Microsoft::ReactNative::JSValueArray &&commandArgs) {
  if (m_viewManagerWithCommands) {
    auto view = viewToUpdate.as<xaml::FrameworkElement>();
    m_viewManagerWithCommands.DispatchCommand(
        view,
        to_hstring(commandId),
        MakeJSValueTreeReader(winrt::Microsoft::ReactNative::JSValue(std::move(commandArgs))));
  }
}

void ABIViewManager::GetExportedCustomBubblingEventTypeConstants(
    const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetExportedCustomBubblingEventTypeConstants(writer);

  if (m_viewManagerWithExportedEventTypeConstants) {
    if (auto constantProvider =
            m_viewManagerWithExportedEventTypeConstants.ExportedCustomBubblingEventTypeConstants()) {
      constantProvider(writer);
    }
  }
}

void ABIViewManager::GetExportedCustomDirectEventTypeConstants(
    const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetExportedCustomDirectEventTypeConstants(writer);

  if (m_viewManagerWithExportedEventTypeConstants) {
    if (auto constantProvider = m_viewManagerWithExportedEventTypeConstants.ExportedCustomDirectEventTypeConstants()) {
      constantProvider(writer);
    }
  }
}

void ABIViewManager::AddView(const xaml::DependencyObject &parent, const xaml::DependencyObject &child, int64_t index) {
  auto parentNMP = parent.as<NativeMeasuringPanel>();
  auto realParent = parentNMP.Content().as<xaml::FrameworkElement>();
  auto childNMP = child.as<NativeMeasuringPanel>();
  auto realChild = childNMP.Content().as<xaml::FrameworkElement>();
  childNMP.Content(nullptr);
  if (m_viewManagerWithChildren) {
    m_viewManagerWithChildren.AddView(realParent, realChild, index);
  } else {
    Super::AddView(realParent, realChild, index);
  }
}

void ABIViewManager::RemoveAllChildren(const xaml::DependencyObject &parent) {
  if (m_viewManagerWithChildren) {
    m_viewManagerWithChildren.RemoveAllChildren(parent.as<xaml::FrameworkElement>());
  } else {
    Super::RemoveAllChildren(parent);
  }
}

void ABIViewManager::RemoveChildAt(const xaml::DependencyObject &parent, int64_t index) {
  if (m_viewManagerWithChildren) {
    m_viewManagerWithChildren.RemoveChildAt(parent.as<xaml::FrameworkElement>(), index);
  } else {
    Super::RemoveChildAt(parent, index);
  }
}

void ABIViewManager::ReplaceChild(
    const xaml::DependencyObject &parent,
    const xaml::DependencyObject &oldChild,
    const xaml::DependencyObject &newChild) {
  if (m_viewManagerWithChildren) {
    m_viewManagerWithChildren.ReplaceChild(
        parent.as<xaml::FrameworkElement>(), oldChild.as<xaml::UIElement>(), newChild.as<xaml::UIElement>());
  } else {
    Super::ReplaceChild(parent, oldChild, newChild);
  }
}

YGMeasureFunc ABIViewManager::GetYogaCustomMeasureFunc() const {
  if (m_viewManagerRequiresNativeLayout && m_viewManagerRequiresNativeLayout.RequiresNativeLayout()) {
    return ::Microsoft::ReactNative::DefaultYogaSelfMeasureFunc;
  } else {
    return nullptr;
  }
}

void ABIViewManager::OnPointerEvent(::Microsoft::ReactNative::ShadowNodeBase *node, const ReactPointerEventArgs &args) {
  if (m_viewManagerWithPointerEvents) {
    m_viewManagerWithPointerEvents.OnPointerEvent(node->GetView(), args);
  }
  // Call the base method to handle `pointerEvents` behavior
  Super::OnPointerEvent(node, args);
}

void ABIViewManager::OnDropViewInstance(const ::Microsoft::ReactNative::XamlView &view) {
  if (m_viewManagerWithDropViewInstance) {
    m_viewManagerWithDropViewInstance.OnDropViewInstance(view.try_as<xaml::FrameworkElement>());
  } else {
    Super::OnDropViewInstance(view);
  }
}

::Microsoft::ReactNative::ShadowNode *ABIViewManager::createShadow() const {
  return new ABIShadowNode(
      m_viewManagerRequiresNativeLayout && m_viewManagerRequiresNativeLayout.RequiresNativeLayout());
}

} // namespace winrt::Microsoft::ReactNative
