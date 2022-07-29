// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <Views/FrameworkElementViewManager.h>
#include "ReactImage.h"

namespace Microsoft::ReactNative {

class ImageViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  ImageViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;

  void GetExportedCustomDirectEventTypeConstants(
      const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  ShadowNode *createShadow() const override;
  void EmitImageEvent(xaml::Controls::Grid grid, const char *eventName, ReactImageSource &source);
  void SetLayoutProps(
      ShadowNodeBase &nodeToUpdate,
      const XamlView &viewToUpdate,
      float left,
      float top,
      float width,
      float height) override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const winrt::Microsoft::ReactNative::JSValue &propertyValue) override;

  XamlView CreateViewCore(int64_t tag, const winrt::Microsoft::ReactNative::JSValueObject &) override;

 private:
  void setSource(xaml::Controls::Grid grid, const winrt::Microsoft::ReactNative::JSValue &sources);
};
} // namespace Microsoft::ReactNative
