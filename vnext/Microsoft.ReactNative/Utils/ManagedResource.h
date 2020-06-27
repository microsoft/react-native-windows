// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

template<typename T, typename closeFn_t>
class managed_resource {
  T handle;
  closeFn_t* closeFn;
 public:
  operator T() const {
    return handle;
  }

  managed_resource(T &&t, closeFn_t &&fn) : handle(t), closeFn(fn) {}
  managed_resource(const managed_resource &) = delete;
  managed_resource &operator=(const managed_resource &) = delete;
  ~managed_resource() {
    closeFn(handle);
  }
};
