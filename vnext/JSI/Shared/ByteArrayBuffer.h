// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "jsi/jsi.h"

#include <memory>

namespace Microsoft::JSI {

class ByteArrayBuffer final : public facebook::jsi::Buffer {
 public:
  ByteArrayBuffer(size_t bufferSize)
      : m_size(bufferSize), m_data(std::make_unique<uint8_t[]>(bufferSize)) {}

  ByteArrayBuffer(const ByteArrayBuffer &) = delete;
  ByteArrayBuffer &operator=(const ByteArrayBuffer &) = delete;

  ByteArrayBuffer(ByteArrayBuffer &&) = default;
  ByteArrayBuffer &operator=(ByteArrayBuffer &&) = default;

  size_t size() const override {
    return m_size;
  }

  const uint8_t *data() const {
    return m_data.get();
  }

  uint8_t *data() {
    return m_data.get();
  }

 private:
  size_t m_size;
  std::unique_ptr<uint8_t[]> m_data;
};

} // namespace Microsoft::JSI
