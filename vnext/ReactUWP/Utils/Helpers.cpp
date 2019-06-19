// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Helpers.h"
#include <Modules/NativeUIManager.h>

namespace react { namespace uwp {

// Not only react-native, native modules could set tag too for controls.
// For example, to identify an clicked item, customer may add tag in NavigationView since content for the two NavigationViewItem are empty.
// 
// <NavigationView>
//  <NavigationViewItem Icon="Accept" Tag="1" />
//  <NavigationViewItem Icon="Accept" Tag="2" />
// </NavigationView>
// Instead of deduce view id directly from FrameworkElement.Tag, this do additional check by uimanager.
ReactId getViewId(_In_ IReactInstance *instance, winrt::FrameworkElement const& fe)
{
  ReactId reactId;
  if (auto uiManager = static_cast<NativeUIManager*>(instance->NativeUIManager()))
  {
    if (auto peer = uiManager->reactPeerOrContainerFrom(fe))
    {
      reactId.isValid = true;
      reactId.tag = GetTag(peer);
    }
  }
  return reactId;
};

}};
