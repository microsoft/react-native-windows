#pragma once

#define VerifyElseCrash(condition) \
  do {                             \
    if (!(condition)) {            \
      assert(false || #condition); \
      std::terminate();            \
    }                              \
  } while (false)
