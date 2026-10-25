/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <span>
#include <string>

#include <react/bridging/Base.h>

namespace facebook::react {

namespace detail {

/**
 * An owning implementation of jsi::MutableBuffer backed by a std::vector.
 * Use this when the source of the bytes cannot be guaranteed to outlive the
 * buffer (for example, returning data to JS from an asynchronous native call,
 * or copying a JS ArrayBuffer argument before storing it).
 */
class OwnedBytesBuffer final : public jsi::MutableBuffer {
 public:
  explicit OwnedBytesBuffer(std::vector<uint8_t> bytes) noexcept : bytes_(std::move(bytes)) {}

  size_t size() const override
  {
    return bytes_.size();
  }

  uint8_t *data() override
  {
    return bytes_.data();
  }

 private:
  std::vector<uint8_t> bytes_;
};

} // namespace detail

template <>
struct Bridging<jsi::ArrayBuffer> {
  static jsi::ArrayBuffer fromJs(jsi::Runtime &rt, const jsi::Object &obj)
  {
    if (!obj.isArrayBuffer(rt)) {
      throw jsi::JSError(rt, "Expected ArrayBuffer");
    }
    return obj.getArrayBuffer(rt);
  }

  static jsi::Value toJs(jsi::Runtime &rt, jsi::ArrayBuffer buf)
  {
    return {rt, buf};
  }
};

// [Windows] AsyncArrayBuffer requires newer JSI runtime APIs that are not yet
// available in node-api-jsi. Keep synchronous ArrayBuffer bridging supported.

} // namespace facebook::react
