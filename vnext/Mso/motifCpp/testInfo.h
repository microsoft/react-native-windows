// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_MOTIFCPP_TESTINFO_H
#define MSO_MOTIFCPP_TESTINFO_H

#include <memory>
#include <vector>
#include "motifCpp/motifCppTestBase.h"

#define TEST_CLASS(className)                                                                      \
  struct className;                                                                                \
                                                                                                   \
  struct TestClassInfo_##className final                                                           \
      : Mso::UnitTests::Internal::TestClassInfoReg<TestClassInfo_##className, className> {         \
    TestClassInfo_##className() noexcept : TestClassInfoRegType{#className, __FILE__, __LINE__} {} \
  };                                                                                               \
                                                                                                   \
  struct className : Mso::UnitTests::Internal::TestClassBase<className, TestClassInfo_##className>

#define TEST_CLASS_EX(className, baseClass)                                                                     \
  struct className;                                                                                             \
                                                                                                                \
  struct TestClassInfo_##className final                                                                        \
      : baseClass,                                                                                              \
        Mso::UnitTests::Internal::TestClassInfoReg<TestClassInfo_##className, className> {                      \
    TestClassInfo_##className() noexcept : baseClass{}, TestClassInfoRegType{#className, __FILE__, __LINE__} {} \
  };                                                                                                            \
                                                                                                                \
  struct className : Mso::UnitTests::Internal::TestClassBase<className, TestClassInfo_##className>

#define TEST_METHOD(methodName)                                                                                  \
  struct TestMethodInfo_##methodName final                                                                       \
      : Mso::UnitTests::Internal::TestMethodInfoReg<TestMethodInfo_##methodName> {                               \
    TestMethodInfo_##methodName() : TestMethodInfoRegType{TestClassInfoType::Instance, #methodName, __LINE__} {} \
                                                                                                                 \
    void Invoke(Mso::UnitTests::TestClass &test) const override {                                                \
      static_cast<TestClassType &>(test).methodName();                                                           \
    }                                                                                                            \
  };                                                                                                             \
  virtual void methodName()

#define TESTMETHOD_REQUIRES_SEH(methodName) TEST_METHOD(methodName)

#define TestClassComponent(x1, x2)

// Allows a test to be compiled, but not executed
#define SKIPTESTMETHOD(methodName) virtual void skipped_##methodName()

namespace Mso::UnitTests {

using TestClass = MotifCppTestBase;
struct TestClassInfo;
struct TestMethodInfo;

template <class T>
struct TestClassInfos {
  static std::vector<TestClassInfo *> s_classInfos;
};

template <class T>
std::vector<TestClassInfo *> TestClassInfos<T>::s_classInfos;

struct TestClassInfo {
  TestClassInfo(const char *className, const char *fileName, int sourceLine) noexcept
      : m_className{className}, m_fileName{fileName}, m_sourceLine{sourceLine} {
    TestClassInfos<int>::s_classInfos.push_back(this);
  }

  const char *ClassName() const noexcept {
    return m_className;
  }
  const char *FileName() const noexcept {
    return m_fileName;
  }
  int SourceLine() const noexcept {
    return m_sourceLine;
  }
  const std::vector<TestMethodInfo *> &MethodInfos() const noexcept {
    return m_methodInfos;
  }
  static const std::vector<TestClassInfo *> &ClassInfos() noexcept {
    return TestClassInfos<int>::s_classInfos;
  }

  void AddMethodInfo(TestMethodInfo &methodInfo) noexcept {
    m_methodInfos.push_back(&methodInfo);
  }

 public: // To implement in derived classes
  virtual std::unique_ptr<TestClass> CreateTest() const = 0;

 private:
  const char *m_className{nullptr};
  const char *m_fileName{nullptr};
  int m_sourceLine{0};
  std::vector<TestMethodInfo *> m_methodInfos;
};

struct TestMethodInfo {
  TestMethodInfo(TestClassInfo &classInfo, const char *methodName, int sourceLine) noexcept
      : m_classInfo{classInfo}, m_methodName{methodName}, m_sourceLine{sourceLine} {
    classInfo.AddMethodInfo(*this);
  }

  const TestClassInfo &ClassInfo() const noexcept {
    return m_classInfo;
  }
  const char *MethodName() const noexcept {
    return m_methodName;
  }
  int SourceLine() const noexcept {
    return m_sourceLine;
  }

 public: // To implement in derived classes
  virtual void Invoke(TestClass &test) const = 0;

 private:
  const TestClassInfo &m_classInfo;
  const char *m_methodName{nullptr};
  int m_sourceLine{0};
};

} // namespace Mso::UnitTests

namespace Mso::UnitTests::Internal {

template <class TTestClass, class TTestClassInfo>
struct TestClassBase : TestClass {
  using TestClassType = TTestClass;
  using TestClassInfoType = TTestClassInfo;
};

template <class TClassInfo, class TTestClass>
struct TestClassInfoReg : TestClassInfo {
  using TestClassInfoRegType = TestClassInfoReg;
  using TestClassInfo::TestClassInfo;

  static TClassInfo Instance;

  std::unique_ptr<TestClass> CreateTest() const override {
    return std::unique_ptr<TestClass>{new TTestClass()};
  }
};

template <class TClassInfo, class TTestClass>
TClassInfo TestClassInfoReg<TClassInfo, TTestClass>::Instance;

template <class TMethodInfo>
struct TestMethodInfoReg : TestMethodInfo {
  using TestMethodInfoRegType = TestMethodInfoReg;

  TestMethodInfoReg(TestClassInfo &classInfo, const char *methodName, int sourceLine) noexcept
      : TestMethodInfo{classInfo, methodName, sourceLine} {
    (void)&Instance; // To ensure that we create static instance
  }

  static TMethodInfo Instance;
};

template <class TMethodInfo>
TMethodInfo TestMethodInfoReg<TMethodInfo>::Instance;

} // namespace Mso::UnitTests::Internal

#endif // MSO_MOTIFCPP_TESTINFO_H
