// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "FrameworkElementViewManager.h"

namespace Microsoft::ReactNative {

struct ShadowNodeBase;

class REACTWINDOWS_EXPORT ControlViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  ControlViewManager(const Mso::React::IReactContext &context);

  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const winrt::Microsoft::ReactNative::JSValue &propertyValue) override;
  void TransferProperties(const XamlView &oldView, const XamlView &newView) override;

 protected:
  void OnViewCreated(XamlView view) override;
};

} // namespace Microsoft::ReactNative
