// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <JSValue.h>
#include <cxxreact/CxxModule.h>

namespace Microsoft::ReactNative {

struct ShadowNode;

class IViewManager {
 public:
  virtual ~IViewManager() {}
  virtual const wchar_t *GetName() const = 0;
  virtual void GetExportedViewConstants(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const = 0;
  virtual void GetCommands(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const = 0;
  virtual void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const = 0;
  virtual ShadowNode *createShadow() const = 0;
  virtual void destroyShadow(ShadowNode *) const = 0;

  virtual void GetConstants(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const = 0;
  virtual void GetExportedCustomBubblingEventTypeConstants(
      const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const = 0;
  virtual void GetExportedCustomDirectEventTypeConstants(
      const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const = 0;
};

} // namespace Microsoft::ReactNative
