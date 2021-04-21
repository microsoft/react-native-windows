// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ShadowNodeBase.h>
#include <Views/ViewManagerBase.h>
#include <folly/dynamic.h>

namespace Microsoft::ReactNative {

struct RawTextShadowNode final : public ShadowNodeBase {
  using Super = ShadowNodeBase;
  winrt::hstring originalText{};
};

class RawTextViewManager : public ViewManagerBase {
  using Super = ViewManagerBase;

 public:
  RawTextViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;
  ShadowNode *createShadow() const override {
    return new RawTextShadowNode();
  }

  void SetLayoutProps(
      ShadowNodeBase &nodeToUpdate,
      const XamlView &viewToUpdate,
      float left,
      float top,
      float width,
      float height) override;
  bool RequiresYogaNode() const override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const winrt::Microsoft::ReactNative::JSValue &propertyValue) override;

  XamlView CreateViewCore(int64_t tag, const winrt::Microsoft::ReactNative::JSValueObject &) override;

 private:
  void NotifyAncestorsTextChanged(ShadowNodeBase *nodeToUpdate);
};

} // namespace Microsoft::ReactNative
