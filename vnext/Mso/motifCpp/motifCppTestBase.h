// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_MOTIFCPP_MOTIFCPPTESTBASE_H
#define MSO_MOTIFCPP_MOTIFCPPTESTBASE_H

typedef wchar_t WCHAR;

class MotifCppTestBase {
 public:
  MotifCppTestBase() {}

  ~MotifCppTestBase() {
    TestClassTeardown();
  }

  virtual void TestClassSetup() {}

  virtual void TestClassTeardown() {}

  virtual void Setup() {}

  virtual void Teardown() {}
};

#endif // MSO_MOTIFCPP_MOTIFCPPTESTBASE_H
