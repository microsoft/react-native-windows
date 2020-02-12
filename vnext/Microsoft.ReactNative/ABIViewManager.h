// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

//
// The ABIViewManager implements a FrameworkElementViewManager subclass and
// wraps up the ABI-safe ViewManager.
//

#pragma once

#include "DynamicReader.h"
#include "DynamicWriter.h"

#include <ReactUWP/ReactUwp.h>
#include <ReactUWP/Views/FrameworkElementViewManager.h>
#include <ReactUWP/Views/ShadowNodeBase.h>

#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

class ABIViewManager : public react::uwp::FrameworkElementViewManager {
  using Super = react::uwp::FrameworkElementViewManager;

 public:
  ABIViewManager(
      const std::shared_ptr<react::uwp::IReactInstance> &reactInstance,
      const ReactNative::IViewManager &viewManager);

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
      winrt::Windows::UI::Xaml::DependencyObject newChild) override;

 protected:
  winrt::Windows::UI::Xaml::DependencyObject CreateViewCore(int64_t) override;

  std::string m_name;
  ReactNative::IViewManager m_viewManager;
  IViewManagerWithReactContext m_viewManagerWithReactContext;
  IViewManagerWithExportedViewConstants m_viewManagerWithExportedViewConstants;
  IViewManagerWithNativeProperties m_viewManagerWithNativeProperties;
  IViewManagerWithCommands m_viewManagerWithCommands;
  IViewManagerWithExportedEventTypeConstants m_viewManagerWithExportedEventTypeConstants;
  IViewManagerWithChildren m_viewManagerWithChildren;

  winrt::Windows::Foundation::Collections::IMapView<winrt::hstring, ViewManagerPropertyType> m_nativeProps;
};

} // namespace winrt::Microsoft::ReactNative
