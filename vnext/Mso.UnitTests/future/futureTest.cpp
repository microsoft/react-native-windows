// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "future/future.h"
#include "cppExtensions/autoRestore.h"
#include "future/futureWait.h"
#include "motifCpp/libletAwareMemLeakDetection.h"
#include "testCheck.h"
#include "testExecutor.h"

#define NOTHING(x) x

namespace FutureTests {

TEST_CLASS_EX (Future2Test, LibletAwareMemLeakDetection) {
  // MemoryLeakDetectionHook::TrackPerTest m_trackLeakPerTest;

  TEST_METHOD(Futureint_ctor_Default) {
    Mso::Future<int> f1;
    TestCheck(!f1);
  }

  TEST_METHOD(Futureint_ctor_nullptr) {
    Mso::Future<int> f1(nullptr);
    TestCheck(!f1);
  }

  TEST_METHOD(Futureint_ctor_copy) {
    Mso::Promise<int> p1;
    p1.SetValue(5);
    Mso::Future<int> f1 = p1.AsFuture();
    Mso::Future<int> f2(f1);
    TestCheck(f1);
    TestCheck(f1 == f2);
  }

  TEST_METHOD(Futureint_ctor_move) {
    Mso::Promise<int> p1;
    p1.SetValue(5);
    Mso::Future<int> f1 = p1.AsFuture();
    Mso::Future<int> f2(f1); // copy of f1
    Mso::Future<int> f3(std::move(f1));
    TestCheck(!f1);
    TestCheck(f3);
    TestCheck(f3 == f2);
  }

  TEST_METHOD(Futureint_CreateMultiple) {
    Mso::Promise<int> p1;
    p1.SetValue(5);
    Mso::Future<int> f1 = p1.AsFuture();
    Mso::Future<int> f2 = p1.AsFuture();

    TestCheck(f1);
    TestCheck(f2);
  }

  TEST_METHOD(Futureint_Assign_nullptr) {
    Mso::Promise<int> p1;
    p1.SetValue(5);
    Mso::Future<int> f1 = p1.AsFuture();
    f1 = nullptr;
    TestCheck(!f1);
  }

  TEST_METHOD(Futureint_Assign_copy) {
    Mso::Promise<int> p1;
    p1.SetValue(5);
    Mso::Promise<int> p2;
    p2.SetValue(5);
    Mso::Future<int> f1 = p1.AsFuture();
    Mso::Future<int> f2 = p2.AsFuture();
    TestCheck(f1 != f2);
    Mso::Future<int> f3(f1); // copy of p1
    f1 = f2;
    TestCheck(f2);
    TestCheck(f1 == f2);
    TestCheck(f1 != f3);
  }

  TEST_METHOD(Futureint_Assign_move) {
    Mso::Promise<int> p1;
    p1.SetValue(5);
    Mso::Promise<int> p2;
    p2.SetValue(5);
    Mso::Future<int> f1 = p1.AsFuture();
    Mso::Future<int> f2 = p2.AsFuture();
    Mso::Future<int> f3(f1); // copy of f1
    Mso::Future<int> f4(f2); // copy of f2
    f1 = std::move(f2);
    TestCheck(f1);
    TestCheck(!f2);
    TestCheck(f1 != f3);
    TestCheck(f1 == f4);
  }

  TEST_METHOD(Futureint_Assign_EmptyInit) {
    Mso::Promise<int> p1;
    p1.SetValue(5);
    Mso::Future<int> f1 = p1.AsFuture();
    f1 = {}; // Assigns empty future which has null state.
    TestCheck(!f1);
  }

  TEST_METHOD(Futureint_Swap) {
    Mso::Promise<int> p1;
    p1.SetValue(5);
    Mso::Promise<int> p2;
    p2.SetValue(5);
    Mso::Future<int> f1 = p1.AsFuture();
    Mso::Future<int> f2 = p2.AsFuture();
    Mso::Future<int> f3(f1); // copy of f1
    Mso::Future<int> f4(f2); // copy of f2
    f1.Swap(f2);
    TestCheck(f1 == f4);
    TestCheck(f2 == f3);
  }

  TEST_METHOD(Futureint_std_swap) {
    using std::swap; // The typical pattern how to call the swap method.
    Mso::Promise<int> p1;
    p1.SetValue(5);
    Mso::Promise<int> p2;
    p2.SetValue(5);
    Mso::Future<int> f1 = p1.AsFuture();
    Mso::Future<int> f2 = p2.AsFuture();
    Mso::Future<int> f3(f1); // copy of f1
    Mso::Future<int> f4(f2); // copy of f2
    swap(
        f1, f2); // Never use the std::swap call. The swap can be overridden in the custom namespace for a entry.Type T.
    TestCheck(f1 == f4);
    TestCheck(f2 == f3);
  }

  TEST_METHOD(Futureint_Get) {
    Mso::Promise<int> p1;
    p1.SetValue(5);
    Mso::Future<int> f1 = p1.AsFuture();
    Mso::Future<int> f2;
    TestCheck(Mso::GetIFuture(f1) != nullptr);
    TestCheck(Mso::GetIFuture(f2) == nullptr);
  }

  TEST_METHOD(Futureint_operator_bool) {
    Mso::Promise<int> p1;
    p1.SetValue(5);
    Mso::Future<int> f1 = p1.AsFuture();
    Mso::Future<int> f2;
    TestCheck(f1);
    TestCheck(!f2);
  }

  TEST_METHOD(Futureint_operator_Equal) {
    Mso::Promise<int> p1;
    p1.SetValue(5);
    Mso::Promise<int> p3;
    p3.SetValue(5);
    Mso::Future<int> f1 = p1.AsFuture();
    Mso::Future<int> f2(f1); // copy
    Mso::Future<int> f3 = p3.AsFuture();
    Mso::Future<int> f4;
    TestCheck(f1 == f2);
    TestCheck(f2 == f1);
    TestCheck(f1 != f3);
    TestCheck(f3 != f1);
    TestCheck(f4 != f1);
    TestCheck(f1 != f4);
    TestCheck(f4 == nullptr);
    TestCheck(nullptr == f4);
    TestCheck(f1 != nullptr);
    TestCheck(nullptr != f1);
  }

  TESTMETHOD_REQUIRES_SEH(Futureint_Then_EmptyFails) {
    Mso::Future<int> f1;
    TestCheckCrash(f1.Then(Mso::Executors::Concurrent{}, [](int /*value*/) noexcept {}));
  }

  TEST_METHOD(Futureint_Then) {
    Mso::Promise<int> p1;
    auto f1 = p1.AsFuture().Then(Mso::Executors::Concurrent{}, [&](int &value) noexcept { TestCheck(value == 5); });

    p1.SetValue(5);
    Mso::FutureWait(f1);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futureint_Then_Moves) {
    Mso::Promise<std::unique_ptr<int>> p1;
    auto f1 = p1.AsFuture().Then(Mso::Executors::Concurrent{}, [&](std::unique_ptr<int> &value) noexcept {
      std::unique_ptr<int> v = std::move(value); // We can move value because only one continuation is allowed.
      TestCheck(*v == 5);
    });

    p1.SetValue(std::unique_ptr<int>(new int(5)));
    Mso::FutureWait(f1);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futureint_Then_AfterCompletion) {
    Mso::Promise<int> p1;
    p1.SetValue(5);

    auto f1 = p1.AsFuture().Then(Mso::Executors::Concurrent{}, [&](int &value) noexcept {
      TestCheck(value == 5);
      ;
    });

    Mso::FutureWait(f1);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futureint_Then_DestroyLambdaAfterInvoke) {
    // See that lambda's destructor is called even if we still hold the future f1 alive.
    // To observe that the lambda is deleted we move observer to its capture.
    // When the observed Mso::TRestorer<bool> is deleted it will reset isDestroyed to true.
    // Creation of Mso::TRestorer<bool> sets it to false.
    bool isDestroyed = true;
    auto observer = std::make_shared<Mso::TRestorer<bool>>(isDestroyed, false);

    Mso::Promise<int> p1;
    p1.SetValue(5);

    auto f1 = p1.AsFuture().Then(Mso::Executors::Concurrent{}, [&, ob = std::move(observer)](int &value) noexcept {
      TestCheck(value == 5);
      ;
    });

    Mso::FutureWait(f1);

    TestCheck(isDestroyed);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futureint_Then_TDefaultExecutor) {
    // See that we can skip passing executor when we want to use the default Concurrent executor.
    Mso::Promise<int> p1;
    p1.SetValue(5);
    bool isInvoked = false;

    auto f1 = p1.AsFuture().Then([&](int &value) noexcept {
      TestCheck(value == 5);
      isInvoked = true;
    });

    Mso::FutureWait(f1);
    TestCheck(isInvoked);
  }

  TEST_METHOD(Futureint_Then_TInlineExecutor) {
    // See that we can pass Mso::Executors::Inline or Mso::Executors::Inline::Optional as TExecutor parameter.
    Mso::Promise<int> p1;
    p1.SetValue(5);
    bool isInvoked = false;

    auto f1 = p1.AsFuture().Then<Mso::Executors::Inline>([&](int &value) noexcept {
      TestCheck(value == 5);
      isInvoked = true;
    });

    Mso::FutureWait(f1);
    TestCheck(isInvoked);
  }

  TEST_METHOD(Futureint_Then_TInlineThrowExecutor) {
    // See that we can pass Mso::Executors::Inline::Throw or Mso::Executors::Inline::Optional::Throw as TExecutor
    // parameter.
    Mso::Promise<int> p1;
    p1.SetValue(5);
    bool isInvoked = false;

    auto f1 = p1.AsFuture().Then<Mso::Executors::Inline::Throwing>([&](int &value) {
      TestCheck(value == 5);
      isInvoked = true;
    });

    Mso::FutureWait(f1);
    TestCheck(isInvoked);
  }

  TEST_METHOD(Futureint_Then_TConcurrentExecutor) {
    // See that we can pass Mso::Executors::Concurrent or Mso::Executors::Concurrent::Optional as TExecutor parameter.
    Mso::Promise<int> p1;
    p1.SetValue(5);
    bool isInvoked = false;

    auto f1 = p1.AsFuture().Then<Mso::Executors::Concurrent>([&](int &value) noexcept {
      TestCheck(value == 5);
      isInvoked = true;
    });

    Mso::FutureWait(f1);
    TestCheck(isInvoked);
  }

  TEST_METHOD(Futureint_Then_TConcurrentThrowExecutor) {
    // See that we can pass Mso::Executors::Concurrent::Throwing as TExecutor parameter.
    Mso::Promise<int> p1;
    p1.SetValue(5);
    bool isInvoked = false;

    auto f1 = p1.AsFuture().Then<Mso::Executors::Concurrent::Throwing>([&](int &value) {
      TestCheck(value == 5);
      isInvoked = true;
    });

    Mso::FutureWait(f1);
    TestCheck(isInvoked);
  }

  TEST_METHOD(Futureint_Then_Then) {
    Mso::Promise<int> p1;
    auto f1 = p1.AsFuture()
                  .Then(Mso::Executors::Concurrent{}, [&](int &value) noexcept { return value; })
                  .Then(Mso::Executors::Concurrent{}, [&](int &value) noexcept {
                    TestCheck(value == 5);
                    ;
                  });

    p1.SetValue(5);
    Mso::FutureWait(f1);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futureint_Then_Then_Moves) {
    Mso::Promise<std::unique_ptr<int>> p1;
    auto f1 =
        p1.AsFuture()
            .Then(Mso::Executors::Concurrent{}, [&](std::unique_ptr<int> &value) noexcept { return std::move(value); })
            .Then(Mso::Executors::Concurrent{}, [&](std::unique_ptr<int> &value) noexcept {
              std::unique_ptr<int> v = std::move(value); // We can move value because only one continuation is allowed.
              TestCheck(*v == 5);
            });

    p1.SetValue(std::unique_ptr<int>(new int(5)));
    Mso::FutureWait(f1);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futureint_Then_ExecutorInvoke) {
    Mso::Promise<int> p1;
    auto f1 = p1.AsFuture().Then(MockExecutor(ExecutorInvoke::RefParam), [&](int /*value*/) noexcept {});

    p1.SetValue(5);
    Mso::FutureWait(f1);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futureint_Then_UnfulfilledPromise) {
    bool isFutureCompleted = false;
    Mso::Promise<int> p1;
    auto f1 =
        p1.AsFuture().Then(Mso::Executors::Concurrent{}, [&](int /*value*/) noexcept { isFutureCompleted = true; });

    // Move p1 to the local variable p2 which should destroy the promise when goes out of scope.
    { Mso::Promise<int> p2(std::move(p1)); }

    auto result = Mso::FutureWait(f1);
    TestCheck(result.IsError());
    TestCheck(!isFutureCompleted);
    TestCheck(!Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futureint_Then_AbandonPromise) {
    bool isFutureCompleted = false;
    Mso::Promise<int> p1;
    auto f1 =
        p1.AsFuture().Then(Mso::Executors::Concurrent{}, [&](int /*value*/) noexcept { isFutureCompleted = true; });

    // Move p1 to the local variable p2 which should destroy the promise when goes out of scope.
    p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
    auto result = Mso::FutureWait(f1);
    TestCheck(result.IsError());
    TestCheck(!isFutureCompleted);
    TestCheck(!Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(Mso::GetIFuture(f1)->IsFailed());
  }

  TESTMETHOD_REQUIRES_SEH(Futureint_Then_ExecutorOnCancel) {
    bool isFutureCompleted = false;
    bool isCancelCalled = false;
    Mso::Promise<int> p1;
    auto f1 = p1.AsFuture().Then(
        MockExecutorOnCancel(/*shouldVEC:*/ false, &isCancelCalled),
        [&](int /*value*/) noexcept { isFutureCompleted = true; });
    p1.SetValue(5); // MockExecutorOnCancel will check synchronously VEC for OnCancel.
    auto result = Mso::FutureWait(f1);
    TestCheck(result.IsError());
    TestCheck(!isFutureCompleted);
    TestCheck(isCancelCalled);
    TestCheck(!Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(Mso::GetIFuture(f1)->IsFailed());
  }

  TESTMETHOD_REQUIRES_SEH(Futureint_Then_SecondThenFails) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();

    Mso::Promise<int> p1;
    auto f1 = p1.AsFuture();
    f1.Then(Mso::Executors::Concurrent{}, [&](int /*value*/) noexcept {});
    TestCheckCrash(f1.Then(Mso::Executors::Concurrent{}, [&](int /*value*/) noexcept {}));
    p1.SetValue(5);
  }

  TESTMETHOD_REQUIRES_SEH(Futureint_Then_AfterCompletion_SecondThenFails) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();

    Mso::Promise<int> p1;
    p1.SetValue(5);
    auto f1 = p1.AsFuture();
    f1.Then(Mso::Executors::Concurrent{}, [&](int /*value*/) noexcept {});
    TestCheckCrash(f1.Then(Mso::Executors::Concurrent{}, [&](int /*value*/) noexcept {}));
  }

  TESTMETHOD_REQUIRES_SEH(Futureint_Then_Observed_SecondThenFails) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();

    Mso::Promise<int> p1;
    p1.SetValue(5);
    auto f1 = p1.AsFuture();

    auto fr = f1.Then(Mso::Executors::Concurrent{}, [&](int &value) noexcept { TestCheck(value == 5); });

    Mso::FutureWait(fr);
    TestCheckCrash(f1.Then(Mso::Executors::Concurrent{}, [&](int /*value*/) noexcept {}));
  }

  TEST_METHOD(Futureint_Then_Future) {
    Mso::Promise<int> p1;
    auto f1 = p1.AsFuture()
                  .Then(
                      Mso::Executors::Concurrent{},
                      [&](int &value) noexcept {
                        return Mso::PostFuture(Mso::Executors::Concurrent{}, [&]() noexcept { return value + 2; });
                      })
                  .Then(Mso::Executors::Concurrent{}, [&](int &value) noexcept { TestCheck(value == 7); });

    p1.SetValue(5);
    Mso::FutureWait(f1);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futureint_Then_FutureUniquePtr) {
    Mso::Promise<std::unique_ptr<int>> p1;
    auto f1 = p1.AsFuture()
                  .Then(
                      Mso::Executors::Concurrent{},
                      [&](std::unique_ptr<int> &ptr) noexcept {
                        return Mso::PostFuture(Mso::Executors::Concurrent{}, [&]() noexcept {
                          // We can capture ptr by reference because the outer future is in progress until this future
                          // finishes.
                          return *ptr + 2;
                        });
                      })
                  .Then(Mso::Executors::Concurrent{}, [&](int value) noexcept { TestCheck(value == 7); });

    p1.EmplaceValue(new int(5));
    Mso::FutureWait(f1);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futureint_Then_FutureVoid) {
    Mso::Promise<int> p1;
    auto f1 =
        p1.AsFuture()
            .Then(
                Mso::Executors::Concurrent{},
                [&](int /*value*/) noexcept { return Mso::PostFuture(Mso::Executors::Concurrent{}, []() noexcept {}); })
            .Then(Mso::Executors::Concurrent{}, [&]() noexcept {});

    p1.SetValue(5);
    Mso::FutureWait(f1);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TESTMETHOD_REQUIRES_SEH(Futureint_Then_Future_TryCancel) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    Mso::Promise<int> p1;
    Mso::Promise<int> p2;
    auto f1 = p1.AsFuture().Then(MockInlineExecutor(), [&](int /*value*/) noexcept { return p2.AsFuture(); });

    p1.SetValue(5);
    p2.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
    Mso::FutureWait(f1);
    TestCheck(!Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(Mso::GetIFuture(f1)->IsFailed());
  }

  TESTMETHOD_REQUIRES_SEH(Futureint_Then_FutureVoid_TryCancel) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    Mso::Promise<int> p1;
    Mso::Promise<void> p2;
    auto f1 = p1.AsFuture().Then(MockInlineExecutor(), [&](int /*value*/) noexcept { return p2.AsFuture(); });

    p1.SetValue(5);
    p2.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
    Mso::FutureWait(f1);
    TestCheck(!Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futurevoid_ctor_Default) {
    Mso::Future<void> f1;
    TestCheck(!f1);
  }

  TEST_METHOD(Futurevoid_ctor_nullptr) {
    Mso::Future<void> f1(nullptr);
    TestCheck(!f1);
  }

  TEST_METHOD(Futurevoid_ctor_copy) {
    Mso::Promise<void> p1;
    p1.SetValue();
    Mso::Future<void> f1 = p1.AsFuture();
    Mso::Future<void> f2(f1);
    TestCheck(f1);
    TestCheck(f1 == f2);
  }

  TEST_METHOD(Futurevoid_ctor_move) {
    Mso::Promise<void> p1;
    p1.SetValue();
    Mso::Future<void> f1 = p1.AsFuture();
    Mso::Future<void> f2(f1); // copy of f1
    Mso::Future<void> f3(std::move(f1));
    TestCheck(!f1);
    TestCheck(f3);
    TestCheck(f3 == f2);
  }

  TEST_METHOD(Futurevoid_CreateMultiple) {
    Mso::Promise<void> p1;
    p1.SetValue();
    Mso::Future<void> f1 = p1.AsFuture();
    Mso::Future<void> f2 = p1.AsFuture();

    TestCheck(f1);
    TestCheck(f2);
  }

  TEST_METHOD(Futurevoid_Assign_nullptr) {
    Mso::Promise<void> p1;
    p1.SetValue();
    Mso::Future<void> f1 = p1.AsFuture();
    f1 = nullptr;
    TestCheck(!f1);
  }

  TEST_METHOD(Futurevoid_Assign_copy) {
    Mso::Promise<void> p1;
    p1.SetValue();
    Mso::Promise<void> p2;
    p2.SetValue();
    Mso::Future<void> f1 = p1.AsFuture();
    Mso::Future<void> f2 = p2.AsFuture();
    TestCheck(f1 != f2);
    Mso::Future<void> f3(f1); // copy of p1
    f1 = f2;
    TestCheck(f2);
    TestCheck(f1 == f2);
    TestCheck(f1 != f3);
  }

  TEST_METHOD(Futurevoid_Assign_move) {
    Mso::Promise<void> p1;
    p1.SetValue();
    Mso::Promise<void> p2;
    p2.SetValue();
    Mso::Future<void> f1 = p1.AsFuture();
    Mso::Future<void> f2 = p2.AsFuture();
    Mso::Future<void> f3(f1); // copy of f1
    Mso::Future<void> f4(f2); // copy of f2
    f1 = std::move(f2);
    TestCheck(f1);
    TestCheck(!f2);
    TestCheck(f1 != f3);
    TestCheck(f1 == f4);
  }

  TEST_METHOD(Futurevoid_Assign_EmptyInit) {
    Mso::Promise<void> p1;
    p1.SetValue();
    Mso::Future<void> f1 = p1.AsFuture();
    f1 = {}; // Assigns empty future which has null state.
    TestCheck(!f1);
  }

  TEST_METHOD(Futurevoid_Swap) {
    Mso::Promise<void> p1;
    p1.SetValue();
    Mso::Promise<void> p2;
    p2.SetValue();
    Mso::Future<void> f1 = p1.AsFuture();
    Mso::Future<void> f2 = p2.AsFuture();
    Mso::Future<void> f3(f1); // copy of f1
    Mso::Future<void> f4(f2); // copy of f2
    f1.Swap(f2);
    TestCheck(f1 == f4);
    TestCheck(f2 == f3);
  }

  TEST_METHOD(Futurevoid_std_swap) {
    using std::swap; // The typical pattern how to call the swap method.
    Mso::Promise<void> p1;
    p1.SetValue();
    Mso::Promise<void> p2;
    p2.SetValue();
    Mso::Future<void> f1 = p1.AsFuture();
    Mso::Future<void> f2 = p2.AsFuture();
    Mso::Future<void> f3(f1); // copy of f1
    Mso::Future<void> f4(f2); // copy of f2
    swap(
        f1, f2); // Never use the std::swap call. The swap can be overridden in the custom namespace for a entry.Type T.
    TestCheck(f1 == f4);
    TestCheck(f2 == f3);
  }

  TEST_METHOD(Futurevoid_Get) {
    Mso::Promise<void> p1;
    p1.SetValue();
    Mso::Future<void> f1 = p1.AsFuture();
    Mso::Future<void> f2;
    TestCheck(Mso::GetIFuture(f1) != nullptr);
    TestCheck(Mso::GetIFuture(f2) == nullptr);
  }

  TEST_METHOD(Futurevoid_operator_bool) {
    Mso::Promise<void> p1;
    p1.SetValue();
    Mso::Future<void> f1 = p1.AsFuture();
    Mso::Future<void> f2;
    TestCheck(f1);
    TestCheck(!f2);
  }

  TEST_METHOD(Futurevoid_operator_Equal) {
    Mso::Promise<void> p1;
    p1.SetValue();
    Mso::Promise<void> p3;
    p3.SetValue();
    Mso::Future<void> f1 = p1.AsFuture();
    Mso::Future<void> f2(f1); // copy
    Mso::Future<void> f3 = p3.AsFuture();
    Mso::Future<void> f4;
    TestCheck(f1 == f2);
    TestCheck(f2 == f1);
    TestCheck(f1 != f3);
    TestCheck(f3 != f1);
    TestCheck(f4 != f1);
    TestCheck(f1 != f4);
    TestCheck(f4 == nullptr);
    TestCheck(nullptr == f4);
    TestCheck(f1 != nullptr);
    TestCheck(nullptr != f1);
  }

  TESTMETHOD_REQUIRES_SEH(Futurevoid_Then_EmptyFails) {
    Mso::Future<void> f1;
    TestCheckCrash(f1.Then(Mso::Executors::Concurrent{}, [](void /*value*/) noexcept {}));
  }

  TEST_METHOD(Futurevoid_Then) {
    Mso::Promise<void> p1;
    auto f1 = p1.AsFuture().Then(Mso::Executors::Concurrent{}, [&]() noexcept {

    });

    p1.SetValue();
    Mso::FutureWait(f1);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futurevoid_Then_AfterCompletion) {
    Mso::Promise<void> p1;
    p1.SetValue();

    auto f1 = p1.AsFuture().Then(Mso::Executors::Concurrent{}, [&]() noexcept { ; });

    Mso::FutureWait(f1);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futurevoid_Then_DestroyLambdaAfterInvoke) {
    // See that lambda's destructor is called even if we still hold the future f1 alive.
    // To observe that the lambda is deleted we move observer to its capture.
    // When the observed Mso::TRestorer<bool> is deleted it will reset isDestroyed to true.
    // Creation of Mso::TRestorer<bool> sets it to false.
    bool isDestroyed = true;
    auto observer = std::make_shared<Mso::TRestorer<bool>>(isDestroyed, false);

    Mso::Promise<void> p1;
    p1.SetValue();

    auto f1 = p1.AsFuture().Then(Mso::Executors::Concurrent{}, [&, ob = std::move(observer)]() noexcept { ; });

    Mso::FutureWait(f1);

    TestCheck(isDestroyed);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futurevoid_Then_TDefaultExecutor) {
    // See that we can skip passing executor when we want to use the default Concurrent executor.
    Mso::Promise<void> p1;
    p1.SetValue();
    bool isInvoked = false;

    auto f1 = p1.AsFuture().Then([&]() noexcept { isInvoked = true; });

    Mso::FutureWait(f1);
    TestCheck(isInvoked);
  }

  TEST_METHOD(Futurevoid_Then_TInlineExecutor) {
    // See that we can pass Mso::Executors::Inline or Mso::Executors::Inline::Optional as TExecutor parameter.
    Mso::Promise<void> p1;
    p1.SetValue();
    bool isInvoked = false;

    auto f1 = p1.AsFuture().Then<Mso::Executors::Inline>([&]() noexcept { isInvoked = true; });

    Mso::FutureWait(f1);
    TestCheck(isInvoked);
  }

  TEST_METHOD(Futurevoid_Then_TInlineThrowExecutor) {
    // See that we can pass Mso::Executors::Inline::Throw or Mso::Executors::Inline::Optional::Throw as TExecutor
    // parameter.
    Mso::Promise<void> p1;
    p1.SetValue();
    bool isInvoked = false;

    auto f1 = p1.AsFuture().Then<Mso::Executors::Inline::Throwing>([&]() { isInvoked = true; });

    Mso::FutureWait(f1);
    TestCheck(isInvoked);
  }

  TEST_METHOD(Futurevoid_Then_TConcurrentExecutor) {
    // See that we can pass Mso::Executors::Concurrent or Mso::Executors::Concurrent::Optional as TExecutor parameter.
    Mso::Promise<void> p1;
    p1.SetValue();
    bool isInvoked = false;

    auto f1 = p1.AsFuture().Then<Mso::Executors::Concurrent>([&]() noexcept { isInvoked = true; });

    Mso::FutureWait(f1);
    TestCheck(isInvoked);
  }

  TEST_METHOD(Futurevoid_Then_TConcurrentThrowExecutor) {
    // See that we can pass Mso::Executors::Concurrent::Throw or Mso::Executors::Concurrent::Optional::Throw as
    // TExecutor parameter.
    Mso::Promise<void> p1;
    p1.SetValue();
    bool isInvoked = false;

    auto f1 = p1.AsFuture().Then<Mso::Executors::Concurrent::Throwing>([&]() { isInvoked = true; });

    Mso::FutureWait(f1);
    TestCheck(isInvoked);
  }

  TEST_METHOD(Futurevoid_Then_Then) {
    Mso::Promise<void> p1;
    auto f1 = p1.AsFuture()
                  .Then(
                      Mso::Executors::Concurrent{},
                      [&]() noexcept {

                      })
                  .Then(Mso::Executors::Concurrent{}, [&]() noexcept { ; });

    p1.SetValue();
    Mso::FutureWait(f1);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futurevoid_Then_ExecutorInvoke) {
    Mso::Promise<void> p1;
    auto f1 = p1.AsFuture().Then(MockExecutor(ExecutorInvoke::NoParam), [&](void /*value*/) noexcept {});

    p1.SetValue();
    Mso::FutureWait(f1);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futurevoid_Then_UnfulfilledPromise) {
    bool isFutureCompleted = false;
    Mso::Promise<void> p1;
    auto f1 =
        p1.AsFuture().Then(Mso::Executors::Concurrent{}, [&](void /*value*/) noexcept { isFutureCompleted = true; });

    // Move p1 to the local variable p2 which should destroy the promise when goes out of scope.
    { Mso::Promise<void> p2(std::move(p1)); }

    auto result = Mso::FutureWait(f1);
    TestCheck(result.IsError());
    TestCheck(!isFutureCompleted);
    TestCheck(!Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futurevoid_Then_AbandonPromise) {
    bool isFutureCompleted = false;
    Mso::Promise<void> p1;
    auto f1 =
        p1.AsFuture().Then(Mso::Executors::Concurrent{}, [&](void /*value*/) noexcept { isFutureCompleted = true; });

    // Move p1 to the local variable p2 which should destroy the promise when goes out of scope.
    p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
    auto result = Mso::FutureWait(f1);
    TestCheck(result.IsError());
    TestCheck(!isFutureCompleted);
    TestCheck(!Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(Mso::GetIFuture(f1)->IsFailed());
  }

  TESTMETHOD_REQUIRES_SEH(Futurevoid_Then_ExecutorOnCancel) {
    bool isFutureCompleted = false;
    bool isCancelCalled = false;
    Mso::Promise<void> p1;
    auto f1 = p1.AsFuture().Then(
        MockExecutorOnCancel(/*shouldVEC:*/ false, &isCancelCalled),
        [&](void /*value*/) noexcept { isFutureCompleted = true; });
    p1.SetValue(); // MockExecutorOnCancel will check synchronously VEC for OnCancel.
    auto result = Mso::FutureWait(f1);
    TestCheck(result.IsError());
    TestCheck(!isFutureCompleted);
    TestCheck(isCancelCalled);
    TestCheck(!Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(Mso::GetIFuture(f1)->IsFailed());
  }

  TESTMETHOD_REQUIRES_SEH(Futurevoid_Then_SecondThenFails) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();

    Mso::Promise<void> p1;
    auto f1 = p1.AsFuture();
    f1.Then(Mso::Executors::Concurrent{}, [&](void /*value*/) noexcept {});
    TestCheckCrash(f1.Then(Mso::Executors::Concurrent{}, [&](void /*value*/) noexcept {}));
    p1.SetValue();
  }

  TESTMETHOD_REQUIRES_SEH(Futurevoid_Then_AfterCompletion_SecondThenFails) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();

    Mso::Promise<void> p1;
    p1.SetValue();
    auto f1 = p1.AsFuture();
    f1.Then(Mso::Executors::Concurrent{}, [&](void /*value*/) noexcept {});
    TestCheckCrash(f1.Then(Mso::Executors::Concurrent{}, [&](void /*value*/) noexcept {}));
  }

  TESTMETHOD_REQUIRES_SEH(Futurevoid_Then_Observed_SecondThenFails) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();

    Mso::Promise<void> p1;
    p1.SetValue();
    auto f1 = p1.AsFuture();

    auto fr = f1.Then(Mso::Executors::Concurrent{}, [&]() noexcept {

    });

    Mso::FutureWait(fr);
    TestCheckCrash(f1.Then(Mso::Executors::Concurrent{}, [&](void /*value*/) noexcept {}));
  }

  TEST_METHOD(Futurevoid_Then_Future) {
    Mso::Promise<void> p1;
    auto f1 = p1.AsFuture()
                  .Then(
                      Mso::Executors::Concurrent{},
                      [&]() noexcept {
                        return Mso::PostFuture(Mso::Executors::Concurrent{}, [&]() noexcept {

                        });
                      })
                  .Then(Mso::Executors::Concurrent{}, [&]() noexcept {

                  });

    p1.SetValue();
    Mso::FutureWait(f1);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TEST_METHOD(Futurevoid_Then_FutureVoid) {
    Mso::Promise<void> p1;
    auto f1 = p1.AsFuture()
                  .Then(
                      Mso::Executors::Concurrent{},
                      [&](void /*value*/) noexcept {
                        return Mso::PostFuture(Mso::Executors::Concurrent{}, []() noexcept {});
                      })
                  .Then(Mso::Executors::Concurrent{}, [&]() noexcept {});

    p1.SetValue();
    Mso::FutureWait(f1);
    TestCheck(Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(f1)->IsFailed());
  }

  TESTMETHOD_REQUIRES_SEH(Futurevoid_Then_Future_TryCancel) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    Mso::Promise<void> p1;
    Mso::Promise<int> p2;
    auto f1 = p1.AsFuture().Then(MockInlineExecutor(), [&](void /*value*/) noexcept { return p2.AsFuture(); });

    p1.SetValue();
    p2.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
    Mso::FutureWait(f1);
    TestCheck(!Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(Mso::GetIFuture(f1)->IsFailed());
  }

  TESTMETHOD_REQUIRES_SEH(Futurevoid_Then_FutureVoid_TryCancel) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    Mso::Promise<void> p1;
    Mso::Promise<void> p2;
    auto f1 = p1.AsFuture().Then(MockInlineExecutor(), [&](void /*value*/) noexcept { return p2.AsFuture(); });

    p1.SetValue();
    p2.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
    Mso::FutureWait(f1);
    TestCheck(!Mso::GetIFuture(f1)->IsSucceeded());
    TestCheck(Mso::GetIFuture(f1)->IsFailed());
  }
};

} // namespace FutureTests
