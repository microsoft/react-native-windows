// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <utility>

namespace react::uwp {
template <typename T, typename TCloseFunction>
class managed_resource {
  T handle;
  TCloseFunction *closeFn;

 public:
  operator T() const {
    return handle;
  }

  managed_resource(T const &&t, TCloseFunction &&fn) : handle(std::move(t)), closeFn(std::move(fn)) {}
  managed_resource(const managed_resource &) = delete;
  managed_resource &operator=(const managed_resource &) = delete;
  ~managed_resource() {
    closeFn(handle);
  }
};
} // namespace react::uwp
