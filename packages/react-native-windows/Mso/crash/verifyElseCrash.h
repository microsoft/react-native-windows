// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_CRASH_VERIFYELSECRASH_H
#define MSO_CRASH_VERIFYELSECRASH_H

#include <compilerAdapters/declspecDefinitions.h>
#include <debugAssertApi/debugAssertApi.h>

DECLSPEC_NORETURN void CrashWithRecovery(uint32_t tag) noexcept;

#define DisableVecAssert() false
#ifndef VerifyElseCrash
#define VerifyElseCrash(f) VerifyElseCrashTag(f, UNTAGGED)
#endif
#define VerifySucceededElseCrash(hr) VerifySucceededElseCrashTag(hr, UNTAGGED)
#define VerifyElseCrashTag(f, tag) VerifyElseCrashSzTag(f, "False: " #f, tag)
#ifndef VerifyElseCrashSz
#define VerifyElseCrashSz(f, sz) VerifyElseCrashSzTag(f, sz, UNTAGGED)
#endif

// Asserts first so the dev can easily attach. If already attached, it does not assert
// because bringing up the assert dialog can cause more code to run (OM:278842).
#define VerifyElseCrashSzTag(f, sz, tag)                              \
  Statement(if (!(f)) {                                               \
    AssertSz1Tag(DisableVecAssert(), "Fatal error: %s", (sz), (tag)); \
    CrashWithRecovery(tag);                                           \
  })

#define VerifySucceededElseCrashTag(hr, tag)                       \
  Statement(const HRESULT _vec_hr_ = (hr); if (FAILED(_vec_hr_)) { \
    AssertSzTag(DisableVecAssert(), "Failed: " #hr, tag);          \
    CrashWithRecovery(/*static_cast<int32_t>(_vec_hr_), */ tag);   \
  })

#define VerifyAllocElseCrashTag(ptr, tag) Statement(if (!(ptr)) CrashWithRecovery(tag);)

#define CrashWithRecoveryOnOOM() CrashWithRecovery(0)

#endif // MSO_CRASH_VERIFYELSECRASH_H
