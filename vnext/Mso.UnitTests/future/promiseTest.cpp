// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "future/future.h"
#include "future/futureWait.h"
#include "motifCpp/libletAwareMemLeakDetection.h"
#include "testCheck.h"
#include "testExecutor.h"

namespace FutureTests {

TEST_CLASS_EX (PromiseTest, LibletAwareMemLeakDetection) {
  // MemoryLeakDetectionHook::TrackPerTest m_trackLeakPerTest;

  TEST_METHOD(Promise_ctor_Default) {
    Mso::Promise<int> p1;
    TestCheck(p1);
  }

  TEST_METHOD(Promise_ctor_nullptr) {
    Mso::Promise<int> p1(nullptr);
    TestCheck(!p1);
  }

  TEST_METHOD(Promise_ctor_state) {
    Mso::Promise<int> p1;
    Mso::Promise<int> p2(Mso::CntPtr<Mso::Futures::IFuture>(Mso::GetIFuture(p1)));
    TestCheck(p1);
    TestCheck(p1 == p2);
  }

  TEST_METHOD(Promise_ctor_copy) {
    Mso::Promise<int> p1;
    Mso::Promise<int> p2(p1);
    TestCheck(p1);
    TestCheck(p1 == p2);
  }

  TEST_METHOD(Promise_ctor_move) {
    Mso::Promise<int> p1;
    Mso::Promise<int> p2(p1); // copy of p1
    Mso::Promise<int> p3(std::move(p1));
    TestCheck(!p1);
    TestCheck(p3);
    TestCheck(p3 == p2);
  }

  TEST_METHOD(Promise_Assign_nullptr) {
    Mso::Promise<int> p1;
    p1 = nullptr;
    TestCheck(!p1);
  }

  TEST_METHOD(Promise_Assign_copy) {
    Mso::Promise<int> p1;
    Mso::Promise<int> p2;
    TestCheck(p1 != p2);
    Mso::Promise<int> p3(p1); // copy of p1
    p1 = p2;
    TestCheck(p2);
    TestCheck(p1 == p2);
    TestCheck(p1 != p3);
  }

  TEST_METHOD(Promise_Assign_move) {
    Mso::Promise<int> p1;
    Mso::Promise<int> p2;
    Mso::Promise<int> p3(p1); // copy of p1
    Mso::Promise<int> p4(p2); // copy of p2
    p1 = std::move(p2);
    TestCheck(p1);
    TestCheck(!p2);
    TestCheck(p1 != p3);
    TestCheck(p1 == p4);
  }

  TEST_METHOD(Promise_Assign_EmptyInit) {
    Mso::Promise<int> p1;
    Mso::Promise<int> p2(p1); // copy of p1
    p1 = {}; // Assigns new non-empty Promise
    TestCheck(p1);
    TestCheck(p2);
    TestCheck(p1 != p2);
  }

  TEST_METHOD(Promise_void_Assign_EmptyInit) {
    Mso::Promise<void> p1;
    Mso::Promise<void> p2(p1); // copy of p1
    p1 = {}; // Assigns new non-empty Promise
    TestCheck(p1);
    TestCheck(p2);
    TestCheck(p1 != p2);
  }

  TEST_METHOD(Promise_dtor_Unfulfilled) {
    Mso::Promise<int> p1;
    // This Promise is unfulfilled but no one observes it. We let it to be destroyed without any side effects.
  }

  TEST_METHOD(Promise_dtor_Unfulfilled_Observed) {
    // Unfulfilled Promise generates a cancellation error.
    Mso::Promise<int> p1;
    Mso::Future<void> f1 = p1.AsFuture().Then([](int /*value*/) noexcept {});

    // Move p1 to the local variable p2 which should destroy the promise when goes out of scope.
    { Mso::Promise<int> p2(std::move(p1)); }

    // Move f1 to the local variable f2 which should destroy the future when goes out of scope.
    // TODO:TestCheckShipAssert(Mso::Async::FutureError::UnhandledErrorTag, Mso::Future<void> f2(std::move(f1)));
  }

  TEST_METHOD(Promise_Swap) {
    Mso::Promise<int> p1;
    Mso::Promise<int> p2;
    Mso::Promise<int> p3(p1); // copy of p1
    Mso::Promise<int> p4(p2); // copy of p2
    p1.Swap(p2);
    TestCheck(p1 == p4);
    TestCheck(p2 == p3);
  }

  TEST_METHOD(Promise_std_swap) {
    using std::swap; // The typical pattern how to call the swap method.
    Mso::Promise<int> p1;
    Mso::Promise<int> p2;
    Mso::Promise<int> p3(p1); // copy of p1
    Mso::Promise<int> p4(p2); // copy of p2
    swap(p1, p2); // Never use the std::swap call. The swap can be overridden in the custom namespace for a type T.
    TestCheck(p1 == p4);
    TestCheck(p2 == p3);
  }

  TEST_METHOD(Promise_Get) {
    Mso::Promise<int> p1;
    Mso::Promise<int> p2(nullptr);
    TestCheck(Mso::GetIFuture(p1) != nullptr);
    TestCheck(Mso::GetIFuture(p2) == nullptr);
  }

  TEST_METHOD(Promise_operator_bool) {
    Mso::Promise<int> p1;
    Mso::Promise<int> p2(nullptr);
    TestCheck(p1);
    TestCheck(!p2);
  }

  TEST_METHOD(Promise_AsFuture) {
    Mso::Promise<int> p1;
    Mso::Future<int> f1 = p1.AsFuture();
    TestCheck(Mso::GetIFuture(p1) == Mso::GetIFuture(f1));
  }

  TEST_METHOD(Promise_SetValue_Succeeds) {
    Mso::Promise<int> p1;
    p1.SetValue(5);
    TestCheck(Mso::GetIFuture(p1)->IsDone());
    TestCheck(Mso::GetIFuture(p1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(Promise_SetValue_Observe) {
    Mso::Promise<int> p1;
    auto future = p1.AsFuture().Then([&](int value) noexcept { TestCheck(value == 5); });

    p1.SetValue(5);
    Mso::FutureWait(future);
  }

  TEST_METHOD(Promise_SetValue_Copy) {
    Mso::Promise<std::vector<int>> p1;
    std::vector<int> vec({1, 2, 3});
    p1.SetValue(vec);

    // modify original vector and see that the Promise still has the original copy.
    vec.push_back(4);

    auto future = p1.AsFuture().Then(
        Mso::Executors::Inline{}, [&](const std::vector<int> &value) noexcept { TestCheckEqual(3u, value.size()); });

    Mso::FutureWait(future);
    TestCheckEqual(4u, vec.size());
  }

  TEST_METHOD(Promise_SetValue_Moves) {
    Mso::Promise<std::unique_ptr<int>> p1;
    std::unique_ptr<int> ptr(new int(5));
    p1.SetValue(std::move(ptr));
    TestCheck(ptr.get() == nullptr);
  }

  TEST_METHOD(Promise_SetValue_ObserveMoved) {
    Mso::Promise<std::unique_ptr<int>> p1;
    auto future = p1.AsFuture().Then([&](std::unique_ptr<int> &&value) noexcept {
      std::unique_ptr<int> v = std::move(value); // We can move value because only one continuation is allowed.
      TestCheck(*v == 5);
    });

    p1.SetValue(std::unique_ptr<int>(new int(5)));
    Mso::FutureWait(future);
  }

  TEST_METHOD(Promise_SetValue_ObserveMoved_Vector) {
    Mso::Promise<std::vector<std::unique_ptr<int>>> p1;
    auto future = p1.AsFuture().Then([&](std::vector<std::unique_ptr<int>> &&value) noexcept {
      std::vector<std::unique_ptr<int>> v =
          std::move(value); // We can move value because only one continuation is allowed.
      TestCheck(*v[0] == 5);
    });

    std::vector<std::unique_ptr<int>> vec;
    vec.emplace_back(new int(5));
    p1.SetValue(std::move(vec));
    Mso::FutureWait(future);
  }

  TESTMETHOD_REQUIRES_SEH(Promise_SetValue_Empty) {
    Mso::Promise<int> p1(nullptr);
    TestCheckCrash(p1.SetValue(5));
  }

  TESTMETHOD_REQUIRES_SEH(Promise_SetValue_AlreadySet) {
    Mso::Promise<int> p1;
    p1.SetValue(5);
    TestCheckCrash(p1.SetValue(5));
  }

  TESTMETHOD_REQUIRES_SEH(Promise_SetValue_AlreadyCanceled) {
    Mso::Promise<int> p1;
    TestCheck(p1.TryCancel());
    TestCheckCrash(p1.SetValue(5));
  }

  TEST_METHOD(Promise_TrySetValue_Succeeds) {
    Mso::Promise<int> p1;
    TestCheck(p1.TrySetValue(5));
    TestCheck(Mso::GetIFuture(p1)->IsDone());
    TestCheck(Mso::GetIFuture(p1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(Promise_TrySetValue_Observe) {
    Mso::Promise<int> p1;
    auto future = p1.AsFuture().Then([&](int value) noexcept { TestCheck(value == 5); });

    TestCheck(p1.TrySetValue(5));
    Mso::FutureWait(future);
  }

  TEST_METHOD(Promise_TrySetValue_Copy) {
    Mso::Promise<std::vector<int>> p1;
    std::vector<int> vec({1, 2, 3});
    TestCheck(p1.TrySetValue(vec));

    // modify original vector and see that the Promise still has the original copy.
    vec.push_back(4);

    auto future = p1.AsFuture().Then(
        Mso::Executors::Inline{}, [&](const std::vector<int> &value) noexcept { TestCheckEqual(3u, value.size()); });

    Mso::FutureWait(future);
    TestCheckEqual(4u, vec.size());
  }

  TEST_METHOD(Promise_TrySetValue_Moves) {
    Mso::Promise<std::unique_ptr<int>> p1;
    std::unique_ptr<int> ptr(new int(5));
    TestCheck(p1.TrySetValue(std::move(ptr)));
    TestCheck(ptr.get() == nullptr);
  }

  TEST_METHOD(Promise_TrySetValue_ObserveMoved) {
    Mso::Promise<std::unique_ptr<int>> p1;
    auto future = p1.AsFuture().Then([&](std::unique_ptr<int> &&value) noexcept {
      std::unique_ptr<int> v = std::move(value); // We can move value because only one continuation is allowed.
      TestCheck(*v == 5);
    });

    TestCheck(p1.TrySetValue(std::unique_ptr<int>(new int(5))));
    Mso::FutureWait(future);
  }

  TEST_METHOD(Promise_TrySetValue_ObserveMoved_Vector) {
    Mso::Promise<std::vector<std::unique_ptr<int>>> p1;
    auto future = p1.AsFuture().Then([&](std::vector<std::unique_ptr<int>> &&value) noexcept {
      std::vector<std::unique_ptr<int>> v =
          std::move(value); // We can move value because only one continuation is allowed.
      TestCheck(*v[0] == 5);
    });

    std::vector<std::unique_ptr<int>> vec;
    vec.emplace_back(new int(5));
    TestCheck(p1.TrySetValue(std::move(vec)));
    Mso::FutureWait(future);
  }

  TESTMETHOD_REQUIRES_SEH(Promise_TrySetValue_Empty) {
    Mso::Promise<int> p1(nullptr);
    TestCheckCrash(p1.TrySetValue(5));
  }

  TEST_METHOD(Promise_TrySetValue_AlreadySet) {
    Mso::Promise<int> p1;
    TestCheck(p1.TrySetValue(5));
    TestCheck(!p1.TrySetValue(5));
  }

  TEST_METHOD(Promise_TrySetValue_AlreadyCanceled) {
    Mso::Promise<int> p1;
    TestCheck(p1.TryCancel());
    TestCheck(!p1.TrySetValue(5));
  }

  TEST_METHOD(Promise_EmplaceValue_Succeeds) {
    Mso::Promise<int> p1;
    p1.EmplaceValue(5);
    TestCheck(Mso::GetIFuture(p1)->IsDone());
    TestCheck(Mso::GetIFuture(p1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(Promise_EmplaceValue_Observe) {
    Mso::Promise<int> p1;
    auto future = p1.AsFuture().Then([&](int value) noexcept { TestCheck(value == 5); });

    p1.EmplaceValue(5);
    Mso::FutureWait(future);
  }

  TEST_METHOD(Promise_EmplaceValue_Moves) {
    Mso::Promise<std::unique_ptr<int>> p1;
    std::unique_ptr<int> ptr(new int(5));
    p1.EmplaceValue(std::move(ptr));
    TestCheck(ptr.get() == nullptr);
  }

  TEST_METHOD(Promise_EmplaceValue_ObserveMoved) {
    Mso::Promise<std::unique_ptr<int>> p1;
    auto future = p1.AsFuture().Then([&](std::unique_ptr<int> &&value) noexcept {
      std::unique_ptr<int> v = std::move(value); // We can move value because only one continuation is allowed.
      TestCheck(*v == 5);
    });

    p1.EmplaceValue(std::unique_ptr<int>(new int(5)));
    Mso::FutureWait(future);
  }

  TEST_METHOD(Promise_EmplaceValue_NoArgs) {
    Mso::Promise<EmplacedType> p1;
    auto future = p1.AsFuture().Then([&](const EmplacedType &value) noexcept { ValidateEmplacedType(value, 1, 2, 3); });

    p1.EmplaceValue();
    Mso::FutureWait(future);
  }

  TEST_METHOD(Promise_EmplaceValue_OneArg) {
    Mso::Promise<EmplacedType> p1;
    auto future = p1.AsFuture().Then([&](const EmplacedType &value) noexcept { ValidateEmplacedType(value, 5, 2, 3); });

    p1.EmplaceValue(5);
    Mso::FutureWait(future);
  }

  TEST_METHOD(Promise_EmplaceValue_TwoArgs) {
    Mso::Promise<EmplacedType> p1;
    auto future = p1.AsFuture().Then([&](const EmplacedType &value) noexcept { ValidateEmplacedType(value, 5, 6, 3); });

    p1.EmplaceValue(5, 6);
    Mso::FutureWait(future);
  }

  TEST_METHOD(Promise_EmplaceValue_ThreeArgs) {
    Mso::Promise<EmplacedType> p1;
    auto future = p1.AsFuture().Then([&](const EmplacedType &value) noexcept { ValidateEmplacedType(value, 5, 6, 7); });

    p1.EmplaceValue(5, 6, 7);
    Mso::FutureWait(future);
  }

  TESTMETHOD_REQUIRES_SEH(Promise_EmplaceValue_Empty) {
    Mso::Promise<int> p1(nullptr);
    TestCheckCrash(p1.EmplaceValue(5));
  }

  TESTMETHOD_REQUIRES_SEH(Promise_EmplaceValue_AlreadySet) {
    Mso::Promise<int> p1;
    p1.EmplaceValue(5);
    TestCheckCrash(p1.EmplaceValue(5));
  }

  TESTMETHOD_REQUIRES_SEH(Promise_EmplaceValue_AlreadyCanceled) {
    Mso::Promise<int> p1;
    TestCheck(p1.TryCancel());
    TestCheckCrash(p1.EmplaceValue(5));
  }

  TEST_METHOD(Promise_TryEmplaceValue_Succeeds) {
    Mso::Promise<int> p1;
    TestCheck(p1.TryEmplaceValue(5));
    TestCheck(Mso::GetIFuture(p1)->IsDone());
    TestCheck(Mso::GetIFuture(p1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(Promise_TryEmplaceValue_Observe) {
    Mso::Promise<int> p1;
    auto future = p1.AsFuture().Then([&](int value) noexcept { TestCheck(value == 5); });

    TestCheck(p1.TryEmplaceValue(5));
    Mso::FutureWait(future);
  }

  TEST_METHOD(Promise_TryEmplaceValue_Moves) {
    Mso::Promise<std::unique_ptr<int>> p1;
    std::unique_ptr<int> ptr(new int(5));
    TestCheck(p1.TryEmplaceValue(std::move(ptr)));
    TestCheck(ptr.get() == nullptr);
  }

  TEST_METHOD(Promise_TryEmplaceValue_ObserveMoved) {
    Mso::Promise<std::unique_ptr<int>> p1;
    auto future = p1.AsFuture().Then([&](std::unique_ptr<int> &&value) noexcept {
      std::unique_ptr<int> v = std::move(value); // We can move value because only one continuation is allowed.
      TestCheck(*v == 5);
    });

    TestCheck(p1.TryEmplaceValue(std::unique_ptr<int>(new int(5))));
    Mso::FutureWait(future);
  }

  TEST_METHOD(Promise_TryEmplaceValue_NoArgs) {
    Mso::Promise<EmplacedType> p1;
    auto future = p1.AsFuture().Then([&](const EmplacedType &value) noexcept { ValidateEmplacedType(value, 1, 2, 3); });

    TestCheck(p1.TryEmplaceValue());
    Mso::FutureWait(future);
  }

  TEST_METHOD(Promise_TryEmplaceValue_OneArg) {
    Mso::Promise<EmplacedType> p1;
    auto future = p1.AsFuture().Then([&](const EmplacedType &value) noexcept { ValidateEmplacedType(value, 5, 2, 3); });

    TestCheck(p1.TryEmplaceValue(5));
    Mso::FutureWait(future);
  }

  TEST_METHOD(Promise_TryEmplaceValue_TwoArgs) {
    Mso::Promise<EmplacedType> p1;
    auto future = p1.AsFuture().Then([&](const EmplacedType &value) noexcept { ValidateEmplacedType(value, 5, 6, 3); });

    TestCheck(p1.TryEmplaceValue(5, 6));
    Mso::FutureWait(future);
  }

  TEST_METHOD(Promise_TryEmplaceValue_ThreeArgs) {
    Mso::Promise<EmplacedType> p1;
    auto future = p1.AsFuture().Then([&](const EmplacedType &value) noexcept { ValidateEmplacedType(value, 5, 6, 7); });

    TestCheck(p1.TryEmplaceValue(5, 6, 7));
    Mso::FutureWait(future);
  }

  TESTMETHOD_REQUIRES_SEH(Promise_TryEmplaceValue_Empty) {
    Mso::Promise<int> p1(nullptr);
    TestCheckCrash(p1.TryEmplaceValue(5));
  }

  TEST_METHOD(Promise_TryEmplaceValue_AlreadySet) {
    Mso::Promise<int> p1;
    TestCheck(p1.TryEmplaceValue(5));
    TestCheck(!p1.TryEmplaceValue(5));
  }

  TEST_METHOD(Promise_TryEmplaceValue_AlreadyCanceled) {
    Mso::Promise<int> p1;
    TestCheck(p1.TryCancel());
    TestCheck(!p1.TryEmplaceValue(5));
  }

  TEST_METHOD(Promise_TryCancel) {
    Mso::Promise<int> p1;
    TestCheck(p1.TryCancel());
    TestCheck(Mso::GetIFuture(p1)->IsDone());
    TestCheck(!Mso::GetIFuture(p1)->IsSucceeded());
    TestCheck(Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(Promise_TryCancel_AlreadySet) {
    Mso::Promise<int> p1;
    p1.SetValue(5);
    TestCheck(!p1.TryCancel());
    TestCheck(!Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(Promise_TryCancel_AlreadyCanceled) {
    Mso::Promise<int> p1;
    TestCheck(p1.TryCancel());
    TestCheck(!p1.TryCancel());
    TestCheck(Mso::GetIFuture(p1)->IsFailed());
  }

  TESTMETHOD_REQUIRES_SEH(Promise_TryCancel_Empty) {
    Mso::Promise<int> p1(nullptr);
    TestCheckCrash(p1.TryCancel());
  }

  TEST_METHOD(Promise_operator_Equal) {
    Mso::Promise<int> p1;
    Mso::Promise<int> p2(p1); // copy
    Mso::Promise<int> p3;
    Mso::Promise<int> p4(nullptr);
    TestCheck(p1 == p2);
    TestCheck(p2 == p1);
    TestCheck(p1 != p3);
    TestCheck(p3 != p1);
    TestCheck(p4 != p1);
    TestCheck(p1 != p4);
    TestCheck(p4 == nullptr);
    TestCheck(nullptr == p4);
    TestCheck(p1 != nullptr);
    TestCheck(nullptr != p1);
  }

  TEST_METHOD(PromiseVoid_ctor_Default) {
    Mso::Promise<void> p1;
    TestCheck(p1);
  }

  TEST_METHOD(PromiseVoid_ctor_nullptr) {
    Mso::Promise<void> p1(nullptr);
    TestCheck(!p1);
  }

  TEST_METHOD(PromiseVoid_ctor_state) {
    Mso::Promise<void> p1;
    Mso::Promise<void> p2(Mso::CntPtr<Mso::Futures::IFuture>(Mso::GetIFuture(p1)));
    TestCheck(p1);
    TestCheck(p1 == p2);
  }

  TEST_METHOD(PromiseVoid_ctor_copy) {
    Mso::Promise<void> p1;
    Mso::Promise<void> p2(p1);
    TestCheck(p1);
    TestCheck(p1 == p2);
  }

  TEST_METHOD(PromiseVoid_ctor_move) {
    Mso::Promise<void> p1;
    Mso::Promise<void> p2(p1); // copy of p1
    Mso::Promise<void> p3(std::move(p1));
    TestCheck(!p1);
    TestCheck(p3);
    TestCheck(p3 == p2);
  }

  TEST_METHOD(PromiseVoid_Assign_nullptr) {
    Mso::Promise<void> p1;
    p1 = nullptr;
    TestCheck(!p1);
  }

  TEST_METHOD(PromiseVoid_Assign_copy) {
    Mso::Promise<void> p1;
    Mso::Promise<void> p2;
    TestCheck(p1 != p2);
    Mso::Promise<void> p3(p1); // copy of p1
    p1 = p2;
    TestCheck(p2);
    TestCheck(p1 == p2);
    TestCheck(p1 != p3);
  }

  TEST_METHOD(PromiseVoid_Assign_move) {
    Mso::Promise<void> p1;
    Mso::Promise<void> p2;
    Mso::Promise<void> p3(p1); // copy of p1
    Mso::Promise<void> p4(p2); // copy of p2
    p1 = std::move(p2);
    TestCheck(p1);
    TestCheck(!p2);
    TestCheck(p1 != p3);
    TestCheck(p1 == p4);
  }

  TEST_METHOD(PromiseVoid_dtor_Unfulfilled) {
    Mso::Promise<void> p1;
    // This Promise is unfulfilled but no one observes it. We let it to be destroyed without any side effects.
  }

  TEST_METHOD(PromiseVoid_dtor_Unfulfilled_Observed) {
    Mso::Promise<void> p1;
    Mso::Future<void> f1 = p1.AsFuture().Then([]() noexcept {});

    // Move p1 to the local variable p2 which should destroy the promise when goes out of scope.
    { Mso::Promise<void> p2(std::move(p1)); }
  }

  TEST_METHOD(PromiseVoid_Swap) {
    Mso::Promise<void> p1;
    Mso::Promise<void> p2;
    Mso::Promise<void> p3(p1); // copy of p1
    Mso::Promise<void> p4(p2); // copy of p2
    p1.Swap(p2);
    TestCheck(p1 == p4);
    TestCheck(p2 == p3);
  }

  TEST_METHOD(PromiseVoid_std_swap) {
    using std::swap; // The typical pattern how to call the swap method.
    Mso::Promise<void> p1;
    Mso::Promise<void> p2;
    Mso::Promise<void> p3(p1); // copy of p1
    Mso::Promise<void> p4(p2); // copy of p2
    swap(p1, p2); // Never use the std::swap call. The swap can be overridden in the custom namespace for a type T.
    TestCheck(p1 == p4);
    TestCheck(p2 == p3);
  }

  TEST_METHOD(PromiseVoid_Get) {
    Mso::Promise<void> p1;
    Mso::Promise<void> p2(nullptr);
    TestCheck(Mso::GetIFuture(p1) != nullptr);
    TestCheck(Mso::GetIFuture(p2) == nullptr);
  }

  TEST_METHOD(PromiseVoid_operator_bool) {
    Mso::Promise<void> p1;
    Mso::Promise<void> p2(nullptr);
    TestCheck(p1);
    TestCheck(!p2);
  }

  TEST_METHOD(PromiseVoid_AsFuture) {
    Mso::Promise<void> p1;
    Mso::Future<void> f1 = p1.AsFuture();
    TestCheck(Mso::GetIFuture(p1) == Mso::GetIFuture(f1));
  }

  TEST_METHOD(PromiseVoid_SetValue) {
    Mso::Promise<void> p1;
    p1.SetValue();
    TestCheck(Mso::GetIFuture(p1)->IsDone());
    TestCheck(Mso::GetIFuture(p1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(PromiseVoid_SetValue_Observe) {
    Mso::Promise<void> p1;
    auto future = p1.AsFuture().Then([&]() noexcept {});

    p1.SetValue();
    Mso::FutureWait(future);
  }

  TESTMETHOD_REQUIRES_SEH(PromiseVoid_SetValue_Empty) {
    Mso::Promise<void> p1(nullptr);
    TestCheckCrash(p1.SetValue());
  }

  TESTMETHOD_REQUIRES_SEH(PromiseVoid_SetValue_AlreadySet) {
    Mso::Promise<void> p1;
    p1.SetValue();
    TestCheckCrash(p1.SetValue());
  }

  TESTMETHOD_REQUIRES_SEH(PromiseVoid_SetValue_AlreadyCanceled) {
    Mso::Promise<void> p1;
    TestCheck(p1.TryCancel());
    TestCheckCrash(p1.SetValue());
  }

  TEST_METHOD(PromiseVoid_TrySetValue) {
    Mso::Promise<void> p1;
    TestCheck(p1.TrySetValue());
    TestCheck(Mso::GetIFuture(p1)->IsDone());
    TestCheck(Mso::GetIFuture(p1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(PromiseVoid_TrySetValue_Observe) {
    Mso::Promise<void> p1;
    auto future = p1.AsFuture().Then([&]() noexcept {});

    TestCheck(p1.TrySetValue());
    Mso::FutureWait(future);
  }

  TESTMETHOD_REQUIRES_SEH(PromiseVoid_TrySetValue_Empty) {
    Mso::Promise<void> p1(nullptr);
    TestCheckCrash(p1.TrySetValue());
  }

  TEST_METHOD(PromiseVoid_TrySetValue_AlreadySet) {
    Mso::Promise<void> p1;
    TestCheck(p1.TrySetValue());
    TestCheck(!p1.TrySetValue());
  }

  TEST_METHOD(PromiseVoid_TrySetValue_AlreadyCanceled) {
    Mso::Promise<void> p1;
    TestCheck(p1.TryCancel());
    TestCheck(!p1.TrySetValue());
  }

  TEST_METHOD(PromiseVoid_TryCancel) {
    Mso::Promise<void> p1;
    TestCheck(p1.TryCancel());
    TestCheck(Mso::GetIFuture(p1)->IsDone());
    TestCheck(!Mso::GetIFuture(p1)->IsSucceeded());
    TestCheck(Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(PromiseVoid_TryCancel_AlreadySet) {
    Mso::Promise<void> p1;
    p1.SetValue();
    TestCheck(!p1.TryCancel());
    TestCheck(!Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(PromiseVoid_TryCancel_AlreadyCanceled) {
    Mso::Promise<void> p1;
    TestCheck(p1.TryCancel());
    TestCheck(!p1.TryCancel());
    TestCheck(Mso::GetIFuture(p1)->IsFailed());
  }

  TESTMETHOD_REQUIRES_SEH(PromiseVoid_TryCancel_Empty) {
    Mso::Promise<void> p1(nullptr);
    TestCheckCrash(p1.TryCancel());
  }

  TEST_METHOD(PromiseVoid_operator_Equal) {
    Mso::Promise<void> p1;
    Mso::Promise<void> p2(p1); // copy
    Mso::Promise<void> p3;
    Mso::Promise<void> p4(nullptr);
    TestCheck(p1 == p2);
    TestCheck(p2 == p1);
    TestCheck(p1 != p3);
    TestCheck(p3 != p1);
    TestCheck(p4 != p1);
    TestCheck(p1 != p4);
    TestCheck(p4 == nullptr);
    TestCheck(nullptr == p4);
    TestCheck(p1 != nullptr);
    TestCheck(nullptr != p1);
  }
};

} // namespace FutureTests
