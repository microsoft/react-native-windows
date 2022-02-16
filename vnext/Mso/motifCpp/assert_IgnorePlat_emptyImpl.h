// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_MOTIFCPP_ASSERT_IGNOREPLAT_EMPTYIMPL_H
#define MSO_MOTIFCPP_ASSERT_IGNOREPLAT_EMPTYIMPL_H
#ifdef MSO_MOTIFCPP

/*----------------------------------------------------------------------------
    MotifCpp assert APIs
----------------------------------------------------------------------------*/

#include <cstdint>

namespace Mso {

class IgnoreAllAssertsPlatformImpl {};

class IgnoreAssertPlatformImpl {
 public:
  IgnoreAssertPlatformImpl(uint32_t) {}
};

} // namespace Mso

#endif // MSO_MOTIFCPP
#endif // MSO_MOTIFCPP_ASSERT_IGNOREPLAT_EMPTYIMPL_H
