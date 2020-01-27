// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "memoryApi/memoryLeakScope.h"

#ifdef DEBUG

namespace Mso {
namespace Memory {

bool IsInShutdownLeakScope() noexcept {
  return false;
}

void EnterShutdownLeakScope(unsigned int /*framesToSkip*/) noexcept {}

void LeaveShutdownLeakScope() noexcept {}

bool IsInIgnoreLeakScope() noexcept {
  return false;
}

void EnterIgnoreLeakScope(unsigned int /*framesToSkip*/) noexcept {}

void LeaveIgnoreLeakScope() noexcept {}

} // namespace Memory
} // namespace Mso

#endif // DEBUG
