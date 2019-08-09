// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <chrono>

namespace facebook {
namespace react {

struct INativeTraceHandler {
  virtual void JSBeginSection(
      const char *profileName,
      const char *args) noexcept = 0;
  virtual void JSEndSection() noexcept = 0;
  virtual void JSBeginAsyncSection(
      const char *profileName,
      int cookie) noexcept = 0;
  virtual void JSEndAsyncSection(
      const char *profileName,
      int cookie) noexcept = 0;
  virtual void JSCounter(const char *profileName, int value) noexcept = 0;

  virtual void NativeBeginSection(
      const char *profileName,
      const char *args) noexcept = 0;
  virtual void NativeEndSection(
      const char *profileName,
      const char *args,
      std::chrono::nanoseconds duration) noexcept = 0;
};

void InitializeTracing(INativeTraceHandler *handler);

} // namespace react
} // namespace facebook
