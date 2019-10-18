#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#define VerifyElseCrash(condition) \
  do {                             \
    if (!(condition)) {            \
      assert(false || #condition); \
      std::terminate();            \
    }                              \
  } while (false)
