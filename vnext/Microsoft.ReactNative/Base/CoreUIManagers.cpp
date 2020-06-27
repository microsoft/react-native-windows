// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <IReactInstance.h>
#include <IUIManager.h>
#include <Modules/NativeUIManager.h>
#include <ViewManagerProvider.h>

// Standard View Managers
#include <Views/ActivityIndicatorViewManager.h>
#include <Views/CheckboxViewManager.h>
#include <Views/DatePickerViewManager.h>
#include <Views/FlyoutViewManager.h>
#include <Views/Image/ImageViewManager.h>
#include <Views/PickerViewManager.h>
#include <Views/PopupViewManager.h>
#include <Views/RawTextViewManager.h>
#include <Views/RefreshControlManager.h>
#include <Views/RootViewManager.h>
#include <Views/ScrollContentViewManager.h>
#include <Views/ScrollViewManager.h>
#include <Views/SliderViewManager.h>
#include <Views/SwitchViewManager.h>
#include <Views/TextInputViewManager.h>
#include <Views/TextViewManager.h>
#include <Views/ViewViewManager.h>
#include <Views/VirtualTextViewManager.h>

// Polyester View Managers // TODO: Move Polyester implementations out of this
// library and depot
#include <Polyester/ButtonContentViewManager.h>
#include <Polyester/ButtonViewManager.h>
#include <Polyester/HyperlinkViewManager.h>
#include <Polyester/IconViewManager.h>

namespace react::uwp {

void AddStandardViewManagers(
    std::vector<std::unique_ptr<facebook::react::IViewManager>> &viewManagers,
    std::shared_ptr<IReactInstance> const &instance) noexcept {
  viewManagers.push_back(std::make_unique<ActivityIndicatorViewManager>(instance));
  viewManagers.push_back(std::make_unique<CheckBoxViewManager>(instance));
  viewManagers.push_back(std::make_unique<DatePickerViewManager>(instance));
  viewManagers.push_back(std::make_unique<FlyoutViewManager>(instance));
  viewManagers.push_back(std::make_unique<ImageViewManager>(instance));
  viewManagers.push_back(std::make_unique<PickerViewManager>(instance));
  viewManagers.push_back(std::make_unique<PopupViewManager>(instance));
  viewManagers.push_back(std::make_unique<RawTextViewManager>(instance));
  viewManagers.push_back(std::make_unique<RootViewManager>(instance));
  viewManagers.push_back(std::make_unique<ScrollContentViewManager>(instance));
  viewManagers.push_back(std::make_unique<SliderViewManager>(instance));
  viewManagers.push_back(std::make_unique<ScrollViewManager>(instance));
  viewManagers.push_back(std::make_unique<SwitchViewManager>(instance));
  viewManagers.push_back(std::make_unique<TextViewManager>(instance));
  viewManagers.push_back(std::make_unique<TextInputViewManager>(instance));
  viewManagers.push_back(std::make_unique<ViewViewManager>(instance));
  viewManagers.push_back(std::make_unique<VirtualTextViewManager>(instance));
  viewManagers.push_back(std::make_unique<RefreshControlViewManager>(instance));
}

void AddPolyesterViewManagers(
    std::vector<std::unique_ptr<facebook::react::IViewManager>> &viewManagers,
    std::shared_ptr<IReactInstance> const &instance) noexcept {
  viewManagers.push_back(std::make_unique<polyester::ButtonViewManager>(instance));
  viewManagers.push_back(std::make_unique<polyester::ButtonContentViewManager>(instance));
  viewManagers.push_back(std::make_unique<polyester::HyperlinkViewManager>(instance));
  viewManagers.push_back(std::make_unique<polyester::IconViewManager>(instance));
}

std::shared_ptr<facebook::react::IUIManager> CreateUIManager2(
    Mso::React::IReactContext *context,
    std::vector<react::uwp::NativeViewManager> &&viewManagers) noexcept {
  // Create UIManager, passing in ViewManagers
  return createIUIManager(std::move(viewManagers), new NativeUIManager(context));
}

} // namespace react::uwp
