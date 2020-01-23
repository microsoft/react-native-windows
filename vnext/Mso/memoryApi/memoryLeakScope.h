// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_MEMORYAPI_MEMORYLEAKSCOPE_H
#define MSO_MEMORYAPI_MEMORYLEAKSCOPE_H

#include "compilerAdapters/functionDecorations.h"

namespace Mso::Memory {

/**
  Is the ShutdownLeakScope currently active?
*/
#ifdef DEBUG
LIBLET_PUBLICAPI bool IsInShutdownLeakScope() noexcept;
#else
inline bool IsInShutdownLeakScope() noexcept {
  return false;
}
#endif

/**
  Enter the scope so allocations on this thread should be marked for shutdown leak detection.
  Scope can be entered a limited number of times.
  Frames to skip is used to track which caller address is associated with this Enter.
*/
#ifdef DEBUG
LIBLET_PUBLICAPI void EnterShutdownLeakScope(unsigned int framesToSkip = 0) noexcept;
#else
inline void EnterShutdownLeakScope(unsigned int /*framesToSkip*/ = 0) noexcept {}
#endif

/**
  Leave
*/
#ifdef DEBUG
LIBLET_PUBLICAPI void LeaveShutdownLeakScope() noexcept;
#else
inline void LeaveShutdownLeakScope() noexcept {}
#endif

/**
  Is the IgnoreLeakScope currently active?
*/
#ifdef DEBUG
LIBLET_PUBLICAPI bool IsInIgnoreLeakScope() noexcept;
#else
inline bool IsInIgnoreLeakScope() noexcept {
  return false;
}
#endif

/**
  Enter the scope so allocations on this thread should be ignored for leak detection.
  Scope can be entered a limited number of times.
  Frames to skip is used to track which caller address is associated with this Enter.
*/
#ifdef DEBUG
LIBLET_PUBLICAPI void EnterIgnoreLeakScope(unsigned int framesToSkip = 0) noexcept;
#else
inline void EnterIgnoreLeakScope(unsigned int /*framesToSkip*/ = 0) noexcept {}
#endif

/**
  Leave
*/
#ifdef DEBUG
LIBLET_PUBLICAPI void LeaveIgnoreLeakScope() noexcept;
#else
inline void LeaveIgnoreLeakScope() noexcept {}
#endif

struct AutoShutdownLeakScope {
  AutoShutdownLeakScope(unsigned int framesToSkip = 0) noexcept {
    EnterShutdownLeakScope(++framesToSkip);
  }

  AutoShutdownLeakScope(const AutoShutdownLeakScope & /*other*/) noexcept : AutoShutdownLeakScope(1) {}
  AutoShutdownLeakScope(AutoShutdownLeakScope && /*other*/) noexcept : AutoShutdownLeakScope(1) {}

  ~AutoShutdownLeakScope() noexcept {
    LeaveShutdownLeakScope();
  }
};

struct AutoIgnoreLeakScope {
  AutoIgnoreLeakScope(unsigned int framesToSkip = 0) noexcept {
    EnterIgnoreLeakScope(++framesToSkip);
  }

  AutoIgnoreLeakScope(const AutoIgnoreLeakScope & /*other*/) noexcept : AutoIgnoreLeakScope(1) {}
  AutoIgnoreLeakScope(AutoIgnoreLeakScope && /*other*/) noexcept : AutoIgnoreLeakScope(1) {}

  ~AutoIgnoreLeakScope() noexcept {
    LeaveIgnoreLeakScope();
  }
};

} // namespace Mso::Memory

#endif // MSO_MEMORYAPI_MEMORYLEAKSCOPE_H
