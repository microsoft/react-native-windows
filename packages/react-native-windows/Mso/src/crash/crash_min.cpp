// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "compilerAdapters/cppMacros.h"
#include "crash/verifyElseCrash.h"
#include "platformAdapters/windowsFirst.h"

#if defined(MS_TARGET_POSIX)

void CrashWithRecovery(uint32_t /*tag*/) noexcept {
  *((int *)0) = 1;
  __builtin_trap();
}

#else

int CrashHandler(const EXCEPTION_POINTERS *excPtr, uint32_t tag) noexcept {
  excPtr->ExceptionRecord->ExceptionCode = tag;
  return EXCEPTION_CONTINUE_SEARCH;
}

void CrashWithRecovery(uint32_t tag) noexcept {
  __try {
    *((int *)0) = 1;
  } __except (CrashHandler(GetExceptionInformation(), tag)) {
  }
  __fastfail(tag);
}

#endif
