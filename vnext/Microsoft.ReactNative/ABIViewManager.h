// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

//
// The ABIViewManager implements a FrameworkElementViewManager subclass and
// wraps up the ABI-safe ViewManager.
//

#pragma once

#include "DynamicReader.h"
#include "DynamicWriter.h"

#include <Views/FrameworkElementViewManager.h>
#include <Views/ShadowNodeBase.h>
#include "ReactHost/React.h"

#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

class ABIViewManager : public ::Microsoft::ReactNative::FrameworkElementViewManager {
  using Super = ::Microsoft::ReactNative::FrameworkElementViewManager;

 public:
  ABIViewManager(
      Mso::CntPtr<Mso::React::IReactContext> const &reactContext,
      ReactNative::IViewManager const &viewManager);

  const wchar_t *GetName() const override;

  void GetExportedViewConstants(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;

  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;

  void UpdateProperties(
      ::Microsoft::ReactNative::ShadowNodeBase *nodeToUpdate,
      winrt::Microsoft::ReactNative::JSValueObject &props) override;

  void GetCommands(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;

  void DispatchCommand(
      const xaml::DependencyObject &viewToUpdate,
      const std::string &commandId,
      winrt::Microsoft::ReactNative::JSValueArray &&commandArgs) override;

  void GetExportedCustomBubblingEventTypeConstants(
      const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;

  void GetExportedCustomDirectEventTypeConstants(
      const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;

  void AddView(const xaml::DependencyObject &parent, const xaml::DependencyObject &child, int64_t index) override;
  void RemoveAllChildren(const xaml::DependencyObject &parent) override;
  void RemoveChildAt(const xaml::DependencyObject &parent, int64_t index) override;
  void ReplaceChild(
      const xaml::DependencyObject &parent,
      const xaml::DependencyObject &oldChild,
      const xaml::DependencyObject &newChild) override;

 protected:
  xaml::DependencyObject CreateViewCore(int64_t) override;

  std::wstring m_name;
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
