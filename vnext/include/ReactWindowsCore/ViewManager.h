// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>

namespace facebook {
namespace react {

struct ShadowNode;

class IViewManager {
 public:
  virtual ~IViewManager() {}
  virtual const char *GetName() const = 0;
  virtual ::folly::dynamic GetExportedViewConstants() const = 0;
  virtual ::folly::dynamic GetCommands() const = 0;
  virtual ::folly::dynamic GetNativeProps() const = 0;
  virtual ShadowNode *createShadow() const = 0;
  virtual void destroyShadow(ShadowNode *) const = 0;

  virtual ::folly::dynamic GetConstants() const = 0;
  virtual ::folly::dynamic GetExportedCustomBubblingEventTypeConstants() const = 0;
  virtual ::folly::dynamic GetExportedCustomDirectEventTypeConstants() const = 0;
};

class ViewManagerBase : public IViewManager {
 public:
  folly::dynamic GetConstants() const override;
};

} // namespace react
} // namespace facebook
