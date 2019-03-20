// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/ControlViewManager.h>

namespace react { namespace uwp {

class ScrollViewManager : public ControlViewManager
{
  using Super = ControlViewManager;
public:
  ScrollViewManager(const std::shared_ptr<IReactInstance>& reactInstance);

  const char* GetName() const override;
  folly::dynamic GetCommands() const override;
  folly::dynamic GetNativeProps() const override;
  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;

  void AddView(XamlView parent, XamlView child, int64_t index) override;
  void RemoveAllChildren(XamlView parent) override;
  void RemoveChildAt(XamlView parent, int64_t index) override;

  void UpdateProperties(ShadowNodeBase* nodeToUpdate, folly::dynamic reactDiffMap) override;
  void DispatchCommand(XamlView viewToUpdate, int64_t commandId, const folly::dynamic& commandArgs) override;

protected:
  XamlView CreateViewCore(int64_t tag) override;

private:
  void AddHandlers(winrt::Windows::UI::Xaml::Controls::ScrollViewer& scrollViewer, int64_t tag);
  void EmitScrollEvent(
    winrt::Windows::UI::Xaml::Controls::ScrollViewer& scrollViewer,
    int64_t tag,
    const char* eventName,
    double x, double y, double zoom);
};

} }
