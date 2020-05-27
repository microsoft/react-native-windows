// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <stdint.h>
#include <string>

namespace facebook {
namespace react {

struct IReactRootView {
  virtual void ResetView() = 0;
  virtual std::string JSComponentName() const noexcept = 0;
  virtual int64_t GetActualHeight() const = 0;
  virtual int64_t GetActualWidth() const = 0;
  virtual int64_t GetTag() const = 0;
  virtual void SetTag(int64_t tag) = 0;
};

} // namespace react
} // namespace facebook
