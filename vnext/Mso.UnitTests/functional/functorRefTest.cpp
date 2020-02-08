// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "functional/functorRef.h"
#include <functional>
#include "functional/functor.h"
#include "motifCpp/testCheck.h"

// use the anonymous namespace to avoid potential conflict with MyParam def in FunctorRef.cpp
namespace {
struct MyParam {
  void AddRef() const {
    ++AddRefCount;
  }
  void Release() const {
    UNREFERENCED_OACR(this); /* Do not decrement */
  }
  mutable int AddRefCount = 0; // AddRef() call count
  int Value;
};
} // namespace

struct TestClass {
  static void Execute(const Mso::VoidFunctorRef &func) noexcept {
    func();
  }

  static void ExecuteThrow(const Mso::VoidFunctorRefThrow &func) {
    func();
  }

  static void ExecuteNullable(const Mso::VoidFunctorRef &func) noexcept {
    if (func) {
      func();
    }
  }

  static void ExecuteNullableThrow(const Mso::VoidFunctorRefThrow &func) {
    if (func) {
      func();
    }
  }

  static int Aggregate(int x, int y, const Mso::FunctorRef<int(int, int)> &func) noexcept {
    return func(x, y);
  }

  static int AggregateThrow(int x, int y, const Mso::FunctorRefThrow<int(int, int)> &func) {
    return func(x, y);
  }

  static int PassByRef(
      const Mso::CntPtr<MyParam> &p,
      const Mso::FunctorRef<int(const Mso::CntPtr<MyParam> &)> &func) noexcept {
    return func(p);
  }

  static int PassByRefThrow(
      const Mso::CntPtr<MyParam> &p,
      const Mso::FunctorRefThrow<int(const Mso::CntPtr<MyParam> &)> &func) {
    return func(p);
  }

  static Mso::CntPtr<MyParam> PassAsRValue(
      Mso::CntPtr<MyParam> &&p,
      const Mso::FunctorRef<Mso::CntPtr<MyParam>(Mso::CntPtr<MyParam> &&)> &func) noexcept {
    return func(std::move(p));
  }

  static Mso::CntPtr<MyParam> PassAsRValueThrow(
      Mso::CntPtr<MyParam> &&p,
      const Mso::FunctorRefThrow<Mso::CntPtr<MyParam>(Mso::CntPtr<MyParam> &&)> &func) {
    return func(std::move(p));
  }

  OACR_WARNING_PUSH
  OACR_WARNING_DISABLE(BY_VALUE_TEMPLATEFORMAL_WITH_DTOR, "We want to test that we can pass by value")

  static std::unique_ptr<int> PassByValue(
      std::unique_ptr<int> p,
      const Mso::FunctorRef<std::unique_ptr<int>(std::unique_ptr<int>)> &func) noexcept {
    return func(std::move(p));
  }

  static std::unique_ptr<int> PassByValueThrow(
      std::unique_ptr<int> p,
      const Mso::FunctorRefThrow<std::unique_ptr<int>(std::unique_ptr<int>)> &func) {
    return func(std::move(p));
  }

  OACR_WARNING_POP
};

struct TestClassNoexcept {
  static void Execute(const Mso::FunctorRef<void() noexcept> &func) noexcept {
    func();
  }

  static int Aggregate(int x, int y, const Mso::FunctorRef<int(int, int) noexcept> &func) noexcept {
    return func(x, y);
  }

  static int PassByRef(
      const Mso::CntPtr<MyParam> &p,
      const Mso::FunctorRef<int(const Mso::CntPtr<MyParam> &) noexcept> &func) noexcept {
    return func(p);
  }

  static Mso::CntPtr<MyParam> PassAsRValue(
      Mso::CntPtr<MyParam> &&p,
      const Mso::FunctorRef<Mso::CntPtr<MyParam>(Mso::CntPtr<MyParam> &&) noexcept> &func) noexcept {
    return func(std::move(p));
  }
};

TEST_CLASS (FunctorRefTest) {
  TEST_METHOD(FunctorRef_Lambda) {
    bool isCalled = false;
    TestClass::Execute([&isCalled]() noexcept { isCalled = true; });
    TestCheck(isCalled);
  }

  TEST_METHOD(FunctorRefThrow_Lambda) {
    bool isCalled = false;
    TestClass::ExecuteThrow([&isCalled]() noexcept { isCalled = true; });
    TestCheck(isCalled);
  }

  TEST_METHOD(FunctorRef_Lambda2) {
    bool isCalled = false;
    auto lambda = [&isCalled]() noexcept {
      isCalled = true;
    };
    TestClass::Execute(lambda);
    TestCheck(isCalled);
  }

  TEST_METHOD(FunctorRefThrow_Lambda2) {
    bool isCalled = false;
    auto lambda = [&isCalled]() noexcept {
      isCalled = true;
    };
    TestClass::ExecuteThrow(lambda);
    TestCheck(isCalled);
  }

  TEST_METHOD(FunctorRef_Nullable) {
    TestClass::ExecuteNullable(nullptr);

    bool isCalled = false;
    TestClass::ExecuteNullable([&isCalled]() noexcept { isCalled = true; });
    TestCheck(isCalled);
  }

  TEST_METHOD(FunctorRefThrow_Nullable) {
    TestClass::ExecuteNullableThrow(nullptr);

    bool isCalled = false;
    TestClass::ExecuteNullableThrow([&isCalled]() noexcept { isCalled = true; });
    TestCheck(isCalled);
  }

  TESTMETHOD_REQUIRES_SEH(FunctorRef_Nullable_Crash) {
    TestCheckCrash(TestClass::Execute(nullptr));
  }

  TESTMETHOD_REQUIRES_SEH(FunctorRefThrow_Nullable_Crash) {
    TestCheckCrash(TestClass::ExecuteThrow(nullptr));
  }

  TEST_METHOD(FunctorRef_MsoFunctor) {
    // It can be useful in case when we have an std::function available.
    bool isCalled = false;
    auto func = Mso::VoidFunctor([&isCalled]() noexcept { isCalled = true; });
    TestClass::Execute(func);
    TestCheck(isCalled);
  }

  TEST_METHOD(FunctorRef_LambdaReturnValue) {
    int addResult = TestClass::Aggregate(3, 5, [](int x, int y) noexcept { return x + y; });
    TestCheckEqual(8, addResult);

    int multResult = TestClass::Aggregate(3, 5, [](int x, int y) noexcept { return x * y; });
    TestCheckEqual(15, multResult);
  }

  TEST_METHOD(FunctorRef_LambdaPassParamByRef) {
    // Pass an Mso::CntPtr as an const reference and see that it was never called AddRef()
    MyParam param;
    auto spParam = Mso::CntPtr<MyParam>(&param);
    TestCheckEqual(1, param.AddRefCount);
    param.Value = 10;

    int result = TestClass::PassByRef(spParam, [](const Mso::CntPtr<MyParam> &p) noexcept { return p->Value; });
    TestCheckEqual(10, result);
    TestCheckEqual(1, param.AddRefCount);
  }

  TEST_METHOD(FunctorRef_LambdaPassParamAsRValue) {
    // Pass an Mso::CntPtr as an r-value and see that it was never called AddRef()
    MyParam param;
    auto spParam = Mso::CntPtr<MyParam>(&param);
    TestCheckEqual(1, param.AddRefCount);

    auto spParam2 =
        TestClass::PassAsRValue(std::move(spParam), [](Mso::CntPtr<MyParam> && p) noexcept { return std::move(p); });
    TestCheckEqual(1, param.AddRefCount);
  }

  TEST_METHOD(FunctorRef_noexcept_Lambda) {
    bool isCalled = false;
    TestClass::Execute([&isCalled]() noexcept { isCalled = true; });
    TestCheck(isCalled);
  }

  TEST_METHOD(FunctorRef_noexcept_MsoFunctor) {
    // It can be useful in case when we have an std::function available.
    bool isCalled = false;
    auto func = Mso::VoidFunctor([&isCalled]() noexcept { isCalled = true; });
    TestClass::Execute(func);
    TestCheck(isCalled);
  }

  TEST_METHOD(FunctorRef_noexcept_LambdaReturnValue) {
    int addResult = TestClass::Aggregate(3, 5, [](int x, int y) noexcept { return x + y; });
    TestCheckEqual(8, addResult);

    int multResult = TestClass::Aggregate(3, 5, [](int x, int y) noexcept { return x * y; });
    TestCheckEqual(15, multResult);
  }

  TEST_METHOD(FunctorRef_noexcept_LambdaPassParamByRef) {
    // Pass an Mso::CntPtr as an const reference and see that it was never called AddRef()
    MyParam param;
    auto spParam = Mso::CntPtr<MyParam>(&param);
    TestCheckEqual(1, param.AddRefCount);
    param.Value = 10;

    int result = TestClass::PassByRef(spParam, [](const Mso::CntPtr<MyParam> &p) noexcept { return p->Value; });
    TestCheckEqual(10, result);
    TestCheckEqual(1, param.AddRefCount);
  }

  TEST_METHOD(FunctorRef_noexcept_LambdaPassParamAsRValue) {
    // Pass an Mso::CntPtr as an r-value and see that it was never called AddRef()
    MyParam param;
    auto spParam = Mso::CntPtr<MyParam>(&param);
    TestCheckEqual(1, param.AddRefCount);

    auto spParam2 =
        TestClass::PassAsRValue(std::move(spParam), [](Mso::CntPtr<MyParam> && p) noexcept { return std::move(p); });
    TestCheckEqual(1, param.AddRefCount);
  }

  TEST_METHOD(FunctorRefThrow_StdFunction) {
    // We use the std::function as an example of throwing function object.
    bool isCalled = false;
    TestClass::ExecuteThrow(std::function<void()>([&isCalled]() noexcept { isCalled = true; }));
    TestCheck(isCalled);
  }

  TEST_METHOD(FunctorRefThrow_LambdaReturnValue) {
    int addResult =
        TestClass::AggregateThrow(3, 5, std::function<int(int, int)>([](int x, int y) noexcept { return x + y; }));
    TestCheckEqual(8, addResult);

    int multResult =
        TestClass::AggregateThrow(3, 5, std::function<int(int, int)>([](int x, int y) noexcept { return x * y; }));
    TestCheckEqual(15, multResult);
  }

  TEST_METHOD(FunctorRefThrow_LambdaPassParamByRef) {
    // Pass an Mso::CntPtr as an const reference and see that it was never called AddRef()
    MyParam param;
    auto spParam = Mso::CntPtr<MyParam>(&param);
    TestCheckEqual(1, param.AddRefCount);
    param.Value = 10;

    using Function = std::function<int(const Mso::CntPtr<MyParam> &)>;
    int result =
        TestClass::PassByRefThrow(spParam, Function([](const Mso::CntPtr<MyParam> &p) noexcept { return p->Value; }));
    TestCheckEqual(10, result);
    TestCheckEqual(1, param.AddRefCount);
  }

  TEST_METHOD(FunctorRefThrow_LambdaPassParamAsRValue) {
    // Pass an Mso::CntPtr as an r-value and see that it was never called AddRef()
    MyParam param;
    auto spParam = Mso::CntPtr<MyParam>(&param);
    TestCheckEqual(1, param.AddRefCount);

    using Function = std::function<Mso::CntPtr<MyParam>(Mso::CntPtr<MyParam> &&)>;
    auto spParam2 = TestClass::PassAsRValueThrow(
        std::move(spParam), Function([](Mso::CntPtr<MyParam> && p) noexcept { return std::move(p); }));
    TestCheckEqual(1, param.AddRefCount);
  }

  TEST_METHOD(FunctorRef_MutableLambda) {
    std::unique_ptr<bool> isCalled{nullptr};
    auto mutableValue = std::make_unique<bool>(true);

    TestClass::Execute([&isCalled, mutableValue = std::move(mutableValue) ]() mutable noexcept {
      isCalled = std::move(mutableValue);
    });

    TestCheck(*isCalled);
  }

  TEST_METHOD(FunctorRefThrow_MutableLambda) {
    std::unique_ptr<bool> isCalled{nullptr};
    auto mutableValue = std::make_unique<bool>(true);

    TestClass::ExecuteThrow([&isCalled, mutableValue = std::move(mutableValue) ]() mutable noexcept {
      isCalled = std::move(mutableValue);
    });

    TestCheck(*isCalled);
  }

  OACR_WARNING_PUSH
  OACR_WARNING_DISABLE(BY_VALUE_TEMPLATEFORMAL_WITH_DTOR, "We want to test that we can pass by value")

  TEST_METHOD(FunctorRef_LambdaPassParamByValue) {
    auto inValue = std::make_unique<int>(5);
    auto result = TestClass::PassByValue(std::move(inValue), [](std::unique_ptr<int> p) noexcept { return p; });
    TestCheckEqual(5, *result);
  }

  TEST_METHOD(FunctorRefThrow_LambdaPassParamByValue) {
    auto inValue = std::make_unique<int>(5);
    auto result = TestClass::PassByValueThrow(std::move(inValue), [](std::unique_ptr<int> p) noexcept { return p; });
    TestCheckEqual(5, *result);
  }

  OACR_WARNING_POP
};
