// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include <cxxreact/CxxModule.h>

namespace facebook {
namespace react {
class MessageQueueThread;
}
} // namespace facebook

namespace react {
namespace uwp {

class ImageViewManagerModule : public facebook::xplat::module::CxxModule {
 public:
  ImageViewManagerModule(
      const std::shared_ptr<facebook::react::MessageQueueThread>
          &defaultQueueThread);
  virtual ~ImageViewManagerModule();

  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  auto getMethods() -> std::vector<Method> override;

  static const char *name;

 private:
  class ImageViewManagerModuleImpl;
  std::shared_ptr<ImageViewManagerModuleImpl> m_imageViewManagerModule;
};

} // namespace uwp
} // namespace react
