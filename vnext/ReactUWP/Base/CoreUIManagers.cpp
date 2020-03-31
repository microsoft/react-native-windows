// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Modules/NativeUIManager.h>
#include <ReactUWP/IReactInstance.h>
#include <ReactUWP/ViewManagerProvider.h>
#include <ReactWindowsCore/IUIManager.h>

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

// TODO: This function is just a stand-in for a system that allows an individual
// host to provide a
//  different set of view managers and native ui manager. Having all of this in
//  one place will simply make it easier to slot in that system when ready.

REACTWINDOWS_API_(std::shared_ptr<facebook::react::IUIManager>)
CreateUIManager(
    std::shared_ptr<IReactInstance> instance,
    const std::shared_ptr<ViewManagerProvider> &viewManagerProvider) {
  std::vector<std::unique_ptr<facebook::react::IViewManager>> viewManagers;

  // Custom view managers
  if (viewManagerProvider) {
    viewManagers = viewManagerProvider->GetViewManagers(instance);
  }

  AddStandardViewManagers(viewManagers, instance);
  AddPolyesterViewManagers(viewManagers, instance);

  // Create UIManager, passing in ViewManagers
  return createIUIManager(std::move(viewManagers), new NativeUIManager());
}

std::shared_ptr<facebook::react::IUIManager> CreateUIManager2(
    std::vector<react::uwp::NativeViewManager> &&viewManagers) noexcept {
  // Create UIManager, passing in ViewManagers
  return createIUIManager(std::move(viewManagers), new NativeUIManager());
}

} // namespace react::uwp
