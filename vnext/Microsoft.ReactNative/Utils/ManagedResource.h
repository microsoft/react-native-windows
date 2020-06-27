// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <utility>

template <typename T, typename closeFn_t>
class managed_resource {
  T handle;
  closeFn_t *closeFn;

 public:
  operator T() const {
    return handle;
  }

  managed_resource(T const &&t, closeFn_t &&fn) : handle(std::move(t)), closeFn(std::move(fn)) {}
  managed_resource(const managed_resource &) = delete;
  managed_resource &operator=(const managed_resource &) = delete;
  ~managed_resource() {
    closeFn(handle);
  }
};
