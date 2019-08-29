// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <Views/FrameworkElementViewManager.h>
#include <Views/ShadowNodeBase.h>
#include "ReactImage.h"

namespace react {
namespace uwp {

class ImageShadowNode : public ShadowNodeBase {
 public:
  ImageShadowNode() = default;

  void createView() override;
  void onDropViewInstance() override;
  void updateProperties(const folly::dynamic &&props) override;

  winrt::Windows::Foundation::Size m_maxSize{};

 private:
  void EmitImageEvent(const char *eventName, ImageSource &source);
  void setSource(const folly::dynamic &data, const winrt::Size &maxSize);
  void updateMaxSize(const folly::dynamic &props);
  float tryGetPropAsFloat(const folly::dynamic &props, const char *propName);

  winrt::event_token m_onLoadEndToken;
};

class ImageViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  ImageViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;

  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;
  folly::dynamic GetNativeProps() const override;
  facebook::react::ShadowNode *createShadow() const override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;
};
} // namespace uwp
} // namespace react
