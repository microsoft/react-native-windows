// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ABIViewManager.h"

#include <ReactUWP/Utils/ValueUtils.h>
#include "ReactSupport.h"

namespace winrt::Microsoft::ReactNative::Bridge {

ABIViewManager::ABIViewManager(
    const std::shared_ptr<react::uwp::IReactInstance> &reactInstance,
    const winrt::Microsoft::ReactNative::Bridge::IViewManager &viewManager)
    : Super(reactInstance),
      m_viewManager{viewManager},
      m_viewManagerWithExportedViewConstants{viewManager.try_as<IViewManagerWithExportedViewConstants>()},
      m_viewManagerWithNativeProperties{viewManager.try_as<IViewManagerWithNativeProperties>()},
      m_viewManagerWithCommands{viewManager.try_as<IViewManagerWithCommands>()},
      m_name{to_string(viewManager.Name())} {
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
    auto outerChild = m_viewManagerWithExportedViewConstants.ExportedViewConstants();
    for (const auto &pair : outerChild) {
      std::string key = to_string(pair.Key());
      folly::dynamic value = ConvertToDynamic(pair.Value());
      parent.insert(key, value);
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

    auto propertyMap = winrt::single_threaded_map<hstring, IInspectable>();

    for (const auto &pair : reactDiffMap.items()) {
      auto propertyName = pair.first.getString();
      auto propertyNameHstring = react::uwp::asHstring(propertyName);

      if (const auto &propertyType = m_nativeProps.TryLookup(propertyNameHstring)) {
        IInspectable propertyValue = nullptr;

        if (propertyType.value() == ViewManagerPropertyType::Color && react::uwp::IsValidColorValue(pair.second)) {
          propertyValue = react::uwp::BrushFrom(pair.second);
        } else {
          propertyValue = ConvertToIInspectable(pair.second);
        }

        propertyMap.Insert(react::uwp::asHstring(propertyName), propertyValue);
      }
    }

    if (propertyMap.Size() > 0) {
      m_viewManagerWithNativeProperties.UpdateProperties(view, propertyMap.GetView());
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

folly::dynamic ABIViewManager::GetCommands() const {
  folly::dynamic innerParent = Super::GetCommands();

  if (m_viewManagerWithCommands) {
    auto outerChild = m_viewManagerWithCommands.Commands();
    for (const auto &pair : outerChild) {
      std::string key = to_string(pair.Key());
      folly::dynamic value{pair.Value()};
      innerParent.insert(key, value);
    }
  }

  return innerParent;
}

void ABIViewManager::DispatchCommand(
    winrt::Windows::UI::Xaml::DependencyObject viewToUpdate,
    int64_t commandId,
    const folly::dynamic &commandArgs) {
  if (m_viewManagerWithCommands) {
    auto view = viewToUpdate.as<winrt::FrameworkElement>();

    auto iinspectableArgs = ConvertToIInspectable(commandArgs);

    auto listArgs =
        iinspectableArgs.try_as<winrt::Windows::Foundation::Collections::IVectorView<winrt::IInspectable>>();

    if (!listArgs) {
      auto args = single_threaded_vector<winrt::IInspectable>();
      args.Append(iinspectableArgs);
      listArgs = args.GetView();
    }

    m_viewManagerWithCommands.DispatchCommand(view, commandId, listArgs);
  }
}

} // namespace winrt::Microsoft::ReactNative::Bridge
