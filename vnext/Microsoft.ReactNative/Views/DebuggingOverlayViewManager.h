// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "FrameworkElementViewManager.h"

namespace Microsoft::ReactNative {

struct ShadowNodeBase;

class REACTWINDOWS_EXPORT DebuggingOverlayViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  DebuggingOverlayViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;
  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  void GetCommands(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  XamlView DebuggingOverlayViewManager::CreateViewCore(
      int64_t /*tag*/,
      const winrt::Microsoft::ReactNative::JSValueObject &) override;
  void DispatchCommand(
      const XamlView &viewToUpdate,
      const std::string &commandId,
      winrt::Microsoft::ReactNative::JSValueArray &&commandArgs) override;
};

} // namespace Microsoft::ReactNative
