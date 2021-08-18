// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>

namespace Microsoft::ReactNative {

class ScrollContentViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  ScrollContentViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;

  void AddView(const XamlView &parent, const XamlView &child, int64_t index) override;
  void RemoveAllChildren(const XamlView &parent) override;
  void RemoveChildAt(const XamlView &parent, int64_t index) override;

 protected:
  XamlView CreateViewCore(int64_t tag, const winrt::Microsoft::ReactNative::JSValueObject &props) override;
};

} // namespace Microsoft::ReactNative
