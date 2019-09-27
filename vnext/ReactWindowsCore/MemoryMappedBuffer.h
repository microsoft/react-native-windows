#pragma once

#include "jsi/jsi.h"

#include <memory>

namespace Microsoft::JSI {

// We only support files whose size can fit within an uint32_t. Memory
// mapping an empty or a larger file fails.
std::shared_ptr<facebook::jsi::Buffer> MakeMemoryMappedBuffer(
    const wchar_t *const filename,
    uint32_t offset = 0);

} // namespace Microsoft::JSI
