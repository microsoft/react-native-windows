// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// The ABIViewManager implements a FrameworkElementViewManager subclass and
// wraps up the ABI-safe ViewManager.
//

#pragma once

#include <pch.h>

#include "DynamicReader.h"
#include "DynamicWriter.h"

#include <ReactUWP/ReactUwp.h>
#include <ReactUWP/Views/FrameworkElementViewManager.h>
#include <ReactUWP/Views/ShadowNodeBase.h>

#include "winrt/Microsoft.ReactNative.Bridge.h"

namespace winrt::Microsoft::ReactNative::Bridge {
class ABIViewManager : public react::uwp::FrameworkElementViewManager {
  using Super = react::uwp::FrameworkElementViewManager;

 public:
  ABIViewManager(
      const std::shared_ptr<react::uwp::IReactInstance> &reactInstance,
      const winrt::Microsoft::ReactNative::Bridge::IViewManager &viewManager);

  const char *GetName() const override;

  folly::dynamic GetExportedViewConstants() const override;

  folly::dynamic GetNativeProps() const override;

  void UpdateProperties(react::uwp::ShadowNodeBase *nodeToUpdate, const folly::dynamic &reactDiffMap) override;

  folly::dynamic GetCommands() const override;

  void DispatchCommand(
      winrt::Windows::UI::Xaml::DependencyObject viewToUpdate,
      int64_t commandId,
      const folly::dynamic &commandArgs) override;

  folly::dynamic GetExportedCustomBubblingEventTypeConstants() const override;

  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;

  void AddView(
      winrt::Windows::UI::Xaml::DependencyObject parent,
      winrt::Windows::UI::Xaml::DependencyObject child,
      int64_t index) override;
  void RemoveAllChildren(winrt::Windows::UI::Xaml::DependencyObject parent) override;
  void RemoveChildAt(winrt::Windows::UI::Xaml::DependencyObject parent, int64_t index) override;
  void ReplaceChild(
      winrt::Windows::UI::Xaml::DependencyObject parent,
      winrt::Windows::UI::Xaml::DependencyObject oldChild,
      winrt::Windows::UI::Xaml::DependencyObject newChild)
      override;

 protected:
  winrt::Windows::UI::Xaml::DependencyObject CreateViewCore(int64_t) override;

  std::string m_name;
  winrt::Microsoft::ReactNative::Bridge::IViewManager m_viewManager;
  winrt::Microsoft::ReactNative::Bridge::IViewManagerWithExportedViewConstants m_viewManagerWithExportedViewConstants;
  winrt::Microsoft::ReactNative::Bridge::IViewManagerWithNativeProperties m_viewManagerWithNativeProperties;
  winrt::Microsoft::ReactNative::Bridge::IViewManagerWithCommands m_viewManagerWithCommands;
  winrt::Microsoft::ReactNative::Bridge::IViewManagerWithExportedEventTypeConstants
      m_viewManagerWithExportedEventTypeConstants;
  winrt::Microsoft::ReactNative::Bridge::IViewManagerWithChildren m_viewManagerWithChildren;

  winrt::Windows::Foundation::Collections::IMapView<winrt::hstring, ViewManagerPropertyType> m_nativeProps;
};
} // namespace winrt::Microsoft::ReactNative::Bridge
