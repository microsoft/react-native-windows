// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#define VerifyElseCrash(condition) \
  do {                             \
    if (!(condition)) {            \
      assert(false && #condition); \
      std::terminate();            \
    }                              \
  } while (false)

#define VerifyElseCrashSz(condition, message) \
  do {                                        \
    if (!(condition)) {                       \
      assert(false && (message));             \
      std::terminate();                       \
    }                                         \
  } while (false)
