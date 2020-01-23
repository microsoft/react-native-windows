// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_MOTIFCPP_GTESTADAPTER_H
#define MSO_MOTIFCPP_GTESTADAPTER_H

#include "gtest/gtest.h"
#include "motifCpp/testInfo.h"

namespace Mso::UnitTests::GTest {

struct GTestFixture : ::testing::Test {
  explicit GTestFixture(const TestClassInfo &classInfo, const TestMethodInfo &methodInfo)
      : m_methodInfo{methodInfo}, m_test{classInfo.CreateTest()} {}

  void TestBody() override {
    m_methodInfo.Invoke(*m_test);
  }

  static void SetUpTestSuite() {}

  static void TearDownTestSuite() {}

  void SetUp() override {}

  void TearDown() override {}

 private:
  const TestMethodInfo &m_methodInfo;
  std::unique_ptr<TestClass> m_test;
};

template <int &... ExplicitParameterBarrier, typename Factory>
inline auto RegisterTest(
    const char *test_suite_name,
    const char *test_name,
    const char *type_param,
    const char *value_param,
    const char *file,
    int line,
    Factory factory) {
  using TestT = typename std::remove_pointer<decltype(factory())>::type;

  class FactoryImpl : public ::testing::internal::TestFactoryBase {
   public:
    explicit FactoryImpl(Factory f) : factory_(std::move(f)) {}
    ::testing::Test *CreateTest() override {
      return factory_();
    }

   private:
    Factory factory_;
  };

  return ::testing::internal::MakeAndRegisterTestInfo(
      test_suite_name,
      test_name,
      type_param,
      value_param,
      ::testing::internal::CodeLocation(file, line),
      ::testing::internal::GetTypeId<TestT>(),
      TestT::SetUpTestCase,
      TestT::TearDownTestCase,
      new FactoryImpl{std::move(factory)});
}

inline void RegisterUnitTests() {
  for (const TestClassInfo *classInfo : TestClassInfo::ClassInfos()) {
    for (const TestMethodInfo *methodInfo : classInfo->MethodInfos()) {
      RegisterTest(
          /*test_suite_name:*/ classInfo->ClassName(),
          /*test_name:*/ methodInfo->MethodName(),
          /*type_param:*/ nullptr,
          /*value_param:*/ nullptr,
          /*file:*/ classInfo->FileName(),
          /*line:*/ methodInfo->SourceLine(),
          /*factory:*/ [classInfo, methodInfo]() {
            return new GTestFixture{*classInfo, *methodInfo};
          });
    }
  }
}

} // namespace Mso::UnitTests::GTest

#endif // MSO_MOTIFCPP_GTESTADAPTER_H
