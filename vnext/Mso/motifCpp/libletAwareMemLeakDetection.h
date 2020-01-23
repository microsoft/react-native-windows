// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_MOTIFCPP_LIBLETAWAREMEMLEAKDETECTION_H
#define MSO_MOTIFCPP_LIBLETAWAREMEMLEAKDETECTION_H

#include "motifCpp/assert_motifApi.h"

// TODO: Implement this as needed
struct LibletAwareMemLeakDetection : MotifCppTestBase {
 protected:
  void InitLiblets() noexcept {}

  void UninitLiblets() noexcept {}

 public:
  virtual void StartTrackingMemoryAllocations() {}
  virtual void StopTrackingMemoryAllocations() {}
};

#endif // MSO_MOTIFCPP_LIBLETAWAREMEMLEAKDETECTION_H
