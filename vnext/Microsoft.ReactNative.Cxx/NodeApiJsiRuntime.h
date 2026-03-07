// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#ifndef NODEAPIJSIRUNTIME_H_
#define NODEAPIJSIRUNTIME_H_

#include <jsi/jsi.h>
#include <functional>
#include <memory>
#include "ApiLoaders/JSRuntimeApi.h"

namespace Microsoft::NodeApiJsi {

std::unique_ptr<facebook::jsi::Runtime> makeNodeApiJsiRuntime(
    napi_env env,
    JSRuntimeApi *jsrApi,
    std::function<void()> onDelete) noexcept;

struct NodeApiEnvScope {
  NodeApiEnvScope(napi_env env) : env_(env) {
    JSRuntimeApi::current()->jsr_open_napi_env_scope(env, &scope_);
  }

  NodeApiEnvScope(const NodeApiEnvScope &) = delete;
  NodeApiEnvScope &operator=(const NodeApiEnvScope &) = delete;

  ~NodeApiEnvScope() {
    JSRuntimeApi::current()->jsr_close_napi_env_scope(env_, scope_);
  }

 private:
  napi_env env_{};
  jsr_napi_env_scope scope_{};
};
} // namespace Microsoft::NodeApiJsi

#endif // !NODEAPIJSIRUNTIME_H_
