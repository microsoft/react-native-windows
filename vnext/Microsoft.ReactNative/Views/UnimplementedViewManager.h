// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ControlViewManager.h>

namespace Microsoft::ReactNative {

class UnimplementedViewManager : public ControlViewManager {
  using Super = ControlViewManager;

 public:
  UnimplementedViewManager(const Mso::React::IReactContext &context, std::wstring name);

  const wchar_t *GetName() const override;
  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  ShadowNode *createShadow() const override;

 protected:
  XamlView CreateViewCore(int64_t tag, const winrt::Microsoft::ReactNative::JSValueObject &) override;

  friend class UnimplementedShadowNode;
  std::wstring m_name;
};

} // namespace Microsoft::ReactNative
