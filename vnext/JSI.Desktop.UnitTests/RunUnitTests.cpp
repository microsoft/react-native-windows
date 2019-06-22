// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "gtest/gtest.h"
#include "windows.h"

int main(int argc, char **argv)
{
  system("pause");
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
