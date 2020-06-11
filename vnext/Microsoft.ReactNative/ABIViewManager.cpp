// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ABIViewManager.h"
#include "DynamicReader.h"
#include "DynamicWriter.h"

#include "IReactContext.h"

#include <ReactUWP/Utils/ValueUtils.h>
#include "ReactHost/MsoUtils.h"

namespace winrt::Microsoft::ReactNative {

ABIViewManager::ABIViewManager(
    std::shared_ptr<react::uwp::IReactInstance> const &reactInstance,
    Mso::CntPtr<Mso::React::IReactContext> const &reactContext,
    ReactNative::IViewManager const &viewManager)
    : Super(reactInstance),
      m_viewManager{viewManager},
      m_viewManagerWithReactContext{viewManager.try_as<IViewManagerWithReactContext>()},
      m_viewManagerWithExportedViewConstants{viewManager.try_as<IViewManagerWithExportedViewConstants>()},
      m_viewManagerWithNativeProperties{viewManager.try_as<IViewManagerWithNativeProperties>()},
      m_viewManagerWithCommands{viewManager.try_as<IViewManagerWithCommands>()},
      m_viewManagerWithExportedEventTypeConstants{viewManager.try_as<IViewManagerWithExportedEventTypeConstants>()},
      m_viewManagerWithChildren{viewManager.try_as<IViewManagerWithChildren>()},
      m_name{to_string(viewManager.Name())} {
  if (m_viewManagerWithReactContext) {
    m_viewManagerWithReactContext.ReactContext(winrt::make<implementation::ReactContext>(Mso::Copy(reactContext)));
  }
  if (m_viewManagerWithNativeProperties) {
    m_nativeProps = m_viewManagerWithNativeProperties.NativeProps();
  }
}

const char *ABIViewManager::GetName() const {
  return m_name.c_str();
}

winrt::Windows::UI::Xaml::DependencyObject ABIViewManager::CreateViewCore(int64_t) {
  auto view = m_viewManager.CreateView();
  return view;
}

folly::dynamic ABIViewManager::GetExportedViewConstants() const {
  folly::dynamic parent = Super::GetExportedViewConstants();

  if (m_viewManagerWithExportedViewConstants) {
    if (auto constantProvider = m_viewManagerWithExportedViewConstants.ExportedViewConstants()) {
      IJSValueWriter argWriter = winrt::make<DynamicWriter>();
      argWriter.WriteObjectBegin();
      constantProvider(argWriter);
      argWriter.WriteObjectEnd();

      auto outerChild = argWriter.as<DynamicWriter>()->TakeValue();

      if (!outerChild.isNull()) {
        parent.update(outerChild);
      }
    }
  }

  return parent;
}

folly::dynamic ABIViewManager::GetNativeProps() const {
  folly::dynamic innerParent = Super::GetNativeProps();

  if (m_nativeProps) {
    for (const auto &pair : m_nativeProps) {
      std::string key = to_string(pair.Key());

      folly::dynamic value;
      switch (pair.Value()) {
        case ViewManagerPropertyType::Boolean:
          value = folly::dynamic("boolean");
          break;
        case ViewManagerPropertyType::Number:
          value = folly::dynamic("number");
          break;
        case ViewManagerPropertyType::String:
          value = folly::dynamic("string");
          break;
        case ViewManagerPropertyType::Array:
          value = folly::dynamic("array");
          break;
        case ViewManagerPropertyType::Map:
          value = folly::dynamic("Map");
          break;
        case ViewManagerPropertyType::Color:
          value = folly::dynamic("Color");
          break;
      }

      innerParent.insert(key, value);
    }
  }

  return innerParent;
}

void ABIViewManager::UpdateProperties(react::uwp::ShadowNodeBase *nodeToUpdate, const folly::dynamic &reactDiffMap) {
  if (m_viewManagerWithNativeProperties) {
    auto view = nodeToUpdate->GetView().as<winrt::FrameworkElement>();

    IJSValueReader propertyMapReader = winrt::make<DynamicReader>(reactDiffMap);

    if (reactDiffMap.size() > 0) {
      m_viewManagerWithNativeProperties.UpdateProperties(view, propertyMapReader);
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

folly::dynamic ABIViewManager::GetCommands() const {
  folly::dynamic commandMap = folly::dynamic::object();

  // Why are we providing commands with the same key and value? React Native 0.61 internally introduced string command
  // IDs which can be dispatched directly without querying the ViewManager for commands. Integer command IDs are
  // internally deprecated, but querying for command ID is still the documented path. Returning constants as their
  // string lets us internally only support the string path.
  if (m_viewManagerWithCommands) {
    for (const auto &commandName : m_viewManagerWithCommands.Commands()) {
      auto commandAsStr = to_string(commandName);
      commandMap[commandAsStr] = commandAsStr;
    }
  }

  return commandMap;
}

void ABIViewManager::DispatchCommand(
    const winrt::Windows::UI::Xaml::DependencyObject &viewToUpdate,
    const std::string &commandId,
    const folly::dynamic &commandArgs) {
  if (m_viewManagerWithCommands) {
    auto view = viewToUpdate.as<winrt::FrameworkElement>();

    IJSValueReader argReader = winrt::make<DynamicReader>(commandArgs);
    m_viewManagerWithCommands.DispatchCommand(view, to_hstring(commandId), argReader);
  }
}

folly::dynamic ABIViewManager::GetExportedCustomBubblingEventTypeConstants() const {
  folly::dynamic parent = Super::GetExportedCustomBubblingEventTypeConstants();

  if (m_viewManagerWithExportedEventTypeConstants) {
    if (auto constantProvider =
            m_viewManagerWithExportedEventTypeConstants.ExportedCustomBubblingEventTypeConstants()) {
      IJSValueWriter argWriter = winrt::make<DynamicWriter>();
      argWriter.WriteObjectBegin();
      constantProvider(argWriter);
      argWriter.WriteObjectEnd();

      auto outerChild = argWriter.as<DynamicWriter>()->TakeValue();

      if (!outerChild.isNull()) {
        parent.update(outerChild);
      }
    }
  }

  return parent;
}

folly::dynamic ABIViewManager::GetExportedCustomDirectEventTypeConstants() const {
  folly::dynamic parent = Super::GetExportedCustomDirectEventTypeConstants();

  if (m_viewManagerWithExportedEventTypeConstants) {
    if (auto constantProvider = m_viewManagerWithExportedEventTypeConstants.ExportedCustomDirectEventTypeConstants()) {
      IJSValueWriter argWriter = winrt::make<DynamicWriter>();
      argWriter.WriteObjectBegin();
      constantProvider(argWriter);
      argWriter.WriteObjectEnd();

      auto outerChild = argWriter.as<DynamicWriter>()->TakeValue();

      if (!outerChild.isNull()) {
        parent.update(outerChild);
      }
    }
  }

  return parent;
}

void ABIViewManager::AddView(
    const winrt::Windows::UI::Xaml::DependencyObject &parent,
    const winrt::Windows::UI::Xaml::DependencyObject &child,
    int64_t index) {
  if (m_viewManagerWithChildren) {
    m_viewManagerWithChildren.AddView(parent.as<winrt::FrameworkElement>(), child.as<winrt::UIElement>(), index);
  } else {
    Super::AddView(parent, child, index);
  }
}

void ABIViewManager::RemoveAllChildren(const winrt::Windows::UI::Xaml::DependencyObject &parent) {
  if (m_viewManagerWithChildren) {
    m_viewManagerWithChildren.RemoveAllChildren(parent.as<winrt::FrameworkElement>());
  } else {
    Super::RemoveAllChildren(parent);
  }
}

void ABIViewManager::RemoveChildAt(const winrt::Windows::UI::Xaml::DependencyObject &parent, int64_t index) {
  if (m_viewManagerWithChildren) {
    m_viewManagerWithChildren.RemoveChildAt(parent.as<winrt::FrameworkElement>(), index);
  } else {
    Super::RemoveChildAt(parent, index);
  }
}

void ABIViewManager::ReplaceChild(
    const winrt::Windows::UI::Xaml::DependencyObject &parent,
    const winrt::Windows::UI::Xaml::DependencyObject &oldChild,
    const winrt::Windows::UI::Xaml::DependencyObject &newChild) {
  if (m_viewManagerWithChildren) {
    m_viewManagerWithChildren.ReplaceChild(
        parent.as<winrt::FrameworkElement>(), oldChild.as<winrt::UIElement>(), newChild.as<winrt::UIElement>());
  } else {
    Super::ReplaceChild(parent, oldChild, newChild);
  }
}

} // namespace winrt::Microsoft::ReactNative
