#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>

namespace winrt {
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Interop;
} // namespace winrt

static const std::unordered_map<std::string, winrt::FlyoutPlacementMode>
    placementModeMinVersion = {{"top", winrt::FlyoutPlacementMode::Top},
                               {"bottom", winrt::FlyoutPlacementMode::Bottom},
                               {"left", winrt::FlyoutPlacementMode::Left},
                               {"right", winrt::FlyoutPlacementMode::Right},
                               {"full", winrt::FlyoutPlacementMode::Full}};

static const std::unordered_map<std::string, winrt::FlyoutPlacementMode>
    placementModeRS5 = {{"top", winrt::FlyoutPlacementMode::Top},
                        {"bottom", winrt::FlyoutPlacementMode::Bottom},
                        {"left", winrt::FlyoutPlacementMode::Left},
                        {"right", winrt::FlyoutPlacementMode::Right},
                        {"full", winrt::FlyoutPlacementMode::Full},
                        {"top-edge-aligned-left",
                         winrt::FlyoutPlacementMode::TopEdgeAlignedLeft},
                        {"top-edge-aligned-right",
                         winrt::FlyoutPlacementMode::TopEdgeAlignedRight},
                        {"bottom-edge-aligned-left",
                         winrt::FlyoutPlacementMode::BottomEdgeAlignedLeft},
                        {"bottom-edge-aligned-right",
                         winrt::FlyoutPlacementMode::BottomEdgeAlignedRight},
                        {"left-edge-aligned-top",
                         winrt::FlyoutPlacementMode::LeftEdgeAlignedTop},
                        {"left-edge-aligned-bottom",
                         winrt::FlyoutPlacementMode::LeftEdgeAlignedBottom},
                        {"right-edge-aligned-top",
                         winrt::FlyoutPlacementMode::RightEdgeAlignedTop},
                        {"right-edge-aligned-bottom",
                         winrt::FlyoutPlacementMode::RightEdgeAlignedBottom}};

namespace react {
namespace uwp {

class FlyoutViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  FlyoutViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;
  facebook::react::ShadowNode *createShadow() const override;
  folly::dynamic GetNativeProps() const override;
  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;
  void SetLayoutProps(
      ShadowNodeBase &nodeToUpdate,
      XamlView viewToUpdate,
      float left,
      float top,
      float width,
      float height) override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;
  friend class FlyoutShadowNode;
};

} // namespace uwp
} // namespace react
