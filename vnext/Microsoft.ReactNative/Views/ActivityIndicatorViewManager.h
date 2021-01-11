// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ControlViewManager.h>

namespace Microsoft::ReactNative {

class ActivityIndicatorViewManager : public ControlViewManager {
  using Super = ControlViewManager;

 public:
  ActivityIndicatorViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;
  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const winrt::Microsoft::ReactNative::JSValue &propertyValue) override;

  XamlView CreateViewCore(int64_t tag) override;
};

} // namespace Microsoft::ReactNative
