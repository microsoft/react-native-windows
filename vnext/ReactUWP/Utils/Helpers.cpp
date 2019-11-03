// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Modules/NativeUIManager.h>
#include "Helpers.h"

namespace winrt {
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Media;
} // namespace winrt

namespace react {
namespace uwp {

// Not only react-native, native modules could set tag too for controls.
// For example, to identify an clicked item, customer may add tag in
// NavigationView since content for the two NavigationViewItem are empty.
//
// <NavigationView>
//  <NavigationViewItem Icon="Accept" Tag="1" />
//  <NavigationViewItem Icon="Accept" Tag="2" />
// </NavigationView>
// Instead of deduce view id directly from FrameworkElement.Tag, this do
// additional check by uimanager.
ReactId getViewId(_In_ IReactInstance *instance, winrt::FrameworkElement const &fe) {
  ReactId reactId;
  if (auto uiManager = static_cast<NativeUIManager *>(instance->NativeUIManager())) {
    if (auto peer = uiManager->reactPeerOrContainerFrom(fe)) {
      reactId.isValid = true;
      reactId.tag = GetTag(peer);
    }
  }
  return reactId;
};

std::int32_t CountOpenPopups() {
  // TODO: Use VisualTreeHelper::GetOpenPopupsFromXamlRoot when running against
  // RS6
  winrt::Windows::Foundation::Collections::IVectorView<winrt::Popup> popups =
      winrt::VisualTreeHelper::GetOpenPopups(winrt::Window::Current());
  return (int32_t)popups.Size();
}

} // namespace uwp
}; // namespace react
