// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <Views/FrameworkElementViewManager.h>
#include "ReactImage.h"

namespace react::uwp {

class ImageViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  ImageViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;

  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;
  folly::dynamic GetNativeProps() const override;
  facebook::react::ShadowNode *createShadow() const override;
  void EmitImageEvent(xaml::Controls::Grid grid, const char *eventName, ReactImageSource &source);

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const folly::dynamic &propertyValue) override;

  XamlView CreateViewCore(int64_t tag) override;

 private:
  void setSource(xaml::Controls::Grid grid, const folly::dynamic &sources);
};
} // namespace react::uwp
