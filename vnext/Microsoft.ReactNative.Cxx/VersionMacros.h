// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#ifdef RNW_CANARY
#define RNW_VERSION_AT_LEAST(major, minor, patch) true
#define RNW_VERSION_LESS_THAN(major, minor, patch) false
#else

#define RNW_VERSION_AT_LEAST(major, minor, patch)                      \
  ((RNW_MAJOR > major) || (RNW_MAJOR == major && RNW_MINOR > minor) || \
   (RNW_MAJOR == major && RNW_MINOR == minor && RNW_PATCH >= patch))

#define RNW_VERSION_LESS_THAN(major, minor, patch)                     \
  ((RNW_MAJOR < major) || (RNW_MAJOR == major && RNW_MINOR < minor) || \
   (RNW_MAJOR == major && RNW_MINOR == minor && RNW_PATCH < patch))

#endif
