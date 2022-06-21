// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/WinComp/CompHelpers.h>
#include <IReactRootView.h>
#include <React.h>
#include <folly/dynamic.h>
#include <winrt/Windows.UI.Composition.h>

namespace Microsoft::ReactNative {

struct ICompRootView : public facebook::react::IReactRootView {
  virtual winrt::com_ptr<Composition::IVisual> GetVisual() const noexcept = 0;
  virtual winrt::com_ptr<Composition::ICompositionContext> CompContext() noexcept = 0;
  virtual double ScaleFactor() noexcept = 0;
};

} // namespace Microsoft::ReactNative
