// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IReactRootView.h>
#include <React.h>
#include <folly/dynamic.h>
#include <winrt/Windows.UI.Composition.h>

namespace Microsoft::ReactNative {

struct ICompRootView : public facebook::react::IReactRootView {
  virtual winrt::Windows::UI::Composition::Visual GetVisual() const noexcept = 0;
  virtual winrt::Windows::UI::Composition::Compositor Compositor() noexcept = 0;
  virtual double ScaleFactor() noexcept = 0;
};

} // namespace Microsoft::ReactNative
