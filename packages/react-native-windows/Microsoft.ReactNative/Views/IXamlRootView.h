// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IReactRootView.h>
#include <React.h>
#include <folly/dynamic.h>
#include "XamlView.h"

namespace Microsoft::ReactNative {

struct IXamlRootView : public facebook::react::IReactRootView {
  virtual Microsoft::ReactNative::XamlView GetXamlView() const noexcept = 0;
};

} // namespace Microsoft::ReactNative
