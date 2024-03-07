// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/Composition/CompositionHelpers.h>
#include <IReactRootView.h>
#include <React.h>
#include <folly/dynamic.h>
#include <winrt/Windows.UI.Composition.h>

namespace Microsoft::ReactNative {

struct ICompositionRootView : public facebook::react::IReactRootView {
  virtual float ScaleFactor() noexcept = 0;
};

} // namespace Microsoft::ReactNative
