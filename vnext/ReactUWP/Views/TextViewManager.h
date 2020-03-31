// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>

namespace react {
namespace uwp {

class TextViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  TextViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  facebook::react::ShadowNode *createShadow() const override;

  VIEWMANAGER_IMPL("RCTText", winrt::Windows::UI::Xaml::Controls::TextBlock, TextViewManager);
  void VIEWMANAGER_DECLARE_PROPERTY_HANDLER(color);

  VIEWMANAGER_PROPERTY_MAP_BEGIN {
      VIEWMANAGER_PROPERTY_MAP_ENTRY(color),
    //{ "color", &TextViewManager::UpdateProperty_color }
  } VIEWMANAGER_PROPERTY_MAP_END


  void UpdateProperties(ShadowNodeBase *nodeToUpdate, const folly::dynamic &reactDiffMap) override;

  void AddView(XamlView parent, XamlView child, int64_t index) override;
  void RemoveAllChildren(XamlView parent) override;
  void RemoveChildAt(XamlView parent, int64_t index) override;

  YGMeasureFunc GetYogaCustomMeasureFunc() const override;

  void OnDescendantTextPropertyChanged(ShadowNodeBase *node);

 protected:
  XamlView CreateViewCore(int64_t tag) override;
};

} // namespace uwp
} // namespace react
