// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "motifCpp/gTestAdapter.h"

int main(int argc, char **argv) {
  Mso::UnitTests::GTest::RegisterUnitTests();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
