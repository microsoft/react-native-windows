// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "object/refCountedObject.h"

// Common code for functor (Functor, SmallFunctor) unit tests.

namespace FunctionalTests {

class TestData : public Mso::RefCountedObjectNoVTable<TestData> {
  using Super = Mso::RefCountedObjectNoVTable<TestData>;

 public:
  TestData(int &addRefCalls, int &releaseCalls) noexcept
      : m_addRefCalls(addRefCalls), m_releaseCalls(releaseCalls), Value(0) {}

  void AddRef() const {
    ++m_addRefCalls;
    Super::AddRef();
  }

  void Release() const {
    ++m_releaseCalls;
    Super::Release();
  }

  int Value;

 private:
  int &m_addRefCalls;
  int &m_releaseCalls;
};

struct StaticMethod {
  static int Add(int x, int y) noexcept {
    return x + y;
  }

  OACR_WARNING_PUSH
  OACR_WARNING_DISABLE(FUNC_COULD_BE_NOEXCEPT, "Simple non-nonexcept function for use in unit tests")
  static int AddThrow(int x, int y) {
    return x + y;
  }
  OACR_WARNING_POP
};

struct StructParam {
  void AddRef() const {
    UNREFERENCED_OACR(this);
  }
  void Release() const {
    UNREFERENCED_OACR(this);
  }
};

struct BaseClass {
  virtual int GetInt() noexcept {
    return 0;
  }
};

struct Derived final : public BaseClass {
  int i;

  int GetInt() noexcept override {
    return i;
  }
};

static Derived &DerivedFromBaseClass(BaseClass &baseClass) noexcept {
  static Derived derived;

  derived.i = baseClass.GetInt();
  return derived;
}

} // namespace FunctionalTests
