// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Helpers.h"
#include <Modules/NativeUIManager.h>

namespace react { namespace uwp {

ReactId getReactId(IReactInstance *instance, winrt::FrameworkElement const& fe)
{
  ReactId reactId;
  assert(instance);
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
