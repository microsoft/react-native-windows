// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ControlViewManager.h>

namespace Microsoft::ReactNative {

class SwitchViewManager : public ControlViewManager {
  using Super = ControlViewManager;

 public:
  SwitchViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;
  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  ShadowNode *createShadow() const override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const winrt::Microsoft::ReactNative::JSValue &propertyValue) override;

  XamlView CreateViewCore(int64_t tag) override;

  friend class SwitchShadowNode;
};

} // namespace Microsoft::ReactNative
