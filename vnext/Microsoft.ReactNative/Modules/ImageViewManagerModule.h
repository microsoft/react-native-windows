// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <cxxreact/CxxModule.h>

namespace facebook {
namespace react {
class MessageQueueThread;
}
} // namespace facebook

namespace react::uwp {

class ImageViewManagerModule : public facebook::xplat::module::CxxModule {
 public:
  ImageViewManagerModule();
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

} // namespace react::uwp
