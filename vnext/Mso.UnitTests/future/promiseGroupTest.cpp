// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "future/future.h"
#include "future/futureWait.h"
#include "motifCpp/libletAwareMemLeakDetection.h"
#include "testCheck.h"
#include "testExecutor.h"

namespace FutureTests {

TEST_CLASS_EX (PromiseGroupTest, LibletAwareMemLeakDetection) {
  // MemoryLeakDetectionHook::TrackPerTest m_trackLeakPerTest;

  TEST_METHOD(PromiseGroup_ctor_Default) {
    Mso::PromiseGroup<int> p1;
    TestCheck(p1);
  }

  TEST_METHOD(PromiseGroup_ctor_nullptr) {
    Mso::PromiseGroup<int> p1(nullptr);
    TestCheck(!p1);
  }

  TEST_METHOD(PromiseGroup_ctor_state) {
    Mso::PromiseGroup<int> p1;
    Mso::PromiseGroup<int> p2(Mso::CntPtr<Mso::Futures::IFuture>(Mso::GetIFuture(p1)));
    TestCheck(p1);
    TestCheck(p1 == p2);
  }

  TEST_METHOD(PromiseGroup_ctor_copy) {
    Mso::PromiseGroup<int> p1;
    Mso::PromiseGroup<int> p2(p1);
    TestCheck(p1);
    TestCheck(p1 == p2);
  }

  TEST_METHOD(PromiseGroup_ctor_move) {
    Mso::PromiseGroup<int> p1;
    Mso::PromiseGroup<int> p2(p1); // copy of p1
    Mso::PromiseGroup<int> p3(std::move(p1));
    TestCheck(!p1);
    TestCheck(p3);
    TestCheck(p3 == p2);
  }

  TEST_METHOD(PromiseGroup_Assign_nullptr) {
    Mso::PromiseGroup<int> p1;
    p1 = nullptr;
    TestCheck(!p1);
  }

  TEST_METHOD(PromiseGroup_Assign_copy) {
    Mso::PromiseGroup<int> p1;
    Mso::PromiseGroup<int> p2;
    TestCheck(p1 != p2);
    Mso::PromiseGroup<int> p3(p1); // copy of p1
    p1 = p2;
    TestCheck(p2);
    TestCheck(p1 == p2);
    TestCheck(p1 != p3);
  }

  TEST_METHOD(PromiseGroup_Assign_move) {
    Mso::PromiseGroup<int> p1;
    Mso::PromiseGroup<int> p2;
    Mso::PromiseGroup<int> p3(p1); // copy of p1
    Mso::PromiseGroup<int> p4(p2); // copy of p2
    p1 = std::move(p2);
    TestCheck(p1);
    TestCheck(!p2);
    TestCheck(p1 != p3);
    TestCheck(p1 == p4);
  }

  TEST_METHOD(PromiseGroup_Assign_EmptyInit) {
    Mso::PromiseGroup<int> p1;
    Mso::PromiseGroup<int> p2(p1); // copy of p1
    p1 = {}; // Assigns new non-empty Promise
    TestCheck(p1);
    TestCheck(p2);
    TestCheck(p1 != p2);
  }

  TEST_METHOD(PromiseGroup_dtor_Unfulfilled) {
    Mso::PromiseGroup<int> p1;
    // This PromiseGroup is unfulfilled but no one observes it. We let it to be destroyed without any side effects.
  }

  TEST_METHOD(PromiseGroup_dtor_Unfulfilled_Observed) {
    Mso::PromiseGroup<int> p1;
    Mso::Future<void> f1 = p1.AddFuture().Then([](int /*value*/) noexcept {});

    // Move p1 to the local variable p2 which should destroy the promise group when goes out of scope.
    { Mso::PromiseGroup<int> p2(std::move(p1)); }

    TestCheck(Mso::FutureWaitIsFailed(f1));
  }

  TEST_METHOD(PromiseGroup_Swap) {
    Mso::PromiseGroup<int> p1;
    Mso::PromiseGroup<int> p2;
    Mso::PromiseGroup<int> p3(p1); // copy of p1
    Mso::PromiseGroup<int> p4(p2); // copy of p2
    p1.Swap(p2);
    TestCheck(p1 == p4);
    TestCheck(p2 == p3);
  }

  TEST_METHOD(PromiseGroup_std_swap) {
    using std::swap; // The typical pattern how to call the swap method.
    Mso::PromiseGroup<int> p1;
    Mso::PromiseGroup<int> p2;
    Mso::PromiseGroup<int> p3(p1); // copy of p1
    Mso::PromiseGroup<int> p4(p2); // copy of p2
    swap(p1, p2); // Never use the std::swap call. The swap can be overridden in the custom namespace for a type T.
    TestCheck(p1 == p4);
    TestCheck(p2 == p3);
  }

  TEST_METHOD(PromiseGroup_GetIFuture) {
    Mso::PromiseGroup<int> p1;
    Mso::PromiseGroup<int> p2(nullptr);
    TestCheck(Mso::GetIFuture(p1) != nullptr);
    TestCheck(Mso::GetIFuture(p2) == nullptr);
  }

  TEST_METHOD(PromiseGroup_operator_bool) {
    Mso::PromiseGroup<int> p1;
    Mso::PromiseGroup<int> p2(nullptr);
    TestCheck(p1);
    TestCheck(!p2);
  }

  TEST_METHOD(PromiseGroup_AddFuture) {
    Mso::PromiseGroup<int> p1;
    Mso::Future<int> f1 = p1.AddFuture();
    TestCheck(f1);
  }

  TEST_METHOD(PromiseGroup_AddFuture_three) {
    Mso::PromiseGroup<int> p1;
    Mso::Future<int> f1 = p1.AddFuture();
    Mso::Future<int> f2 = p1.AddFuture();
    Mso::Future<int> f3 = p1.AddFuture();
    TestCheck(f1);
    TestCheck(f2);
    TestCheck(f3);
  }

  TEST_METHOD(PromiseGroup_SetValue_Completes) {
    Mso::PromiseGroup<int> p1;
    p1.SetValue(5);
    TestCheck(Mso::GetIFuture(p1)->IsDone());
    TestCheck(Mso::GetIFuture(p1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(PromiseGroup_SetValue_Observe) {
    Mso::PromiseGroup<int> p1;
    auto future = p1.AddFuture().Then([&](int value) noexcept { TestCheck(value == 5); });

    p1.SetValue(5);
    Mso::FutureWait(future);
  }

  TEST_METHOD(PromiseGroup_SetValue_Observe_three) {
    Mso::PromiseGroup<int> p1;
    auto future1 = p1.AddFuture().Then([&](int value) noexcept { TestCheck(value == 5); });
    auto future2 = p1.AddFuture().Then([&](int value) noexcept { TestCheck(value == 5); });
    auto future3 = p1.AddFuture().Then([&](int value) noexcept { TestCheck(value == 5); });

    p1.SetValue(5);
    Mso::FutureWait(future1);
    Mso::FutureWait(future2);
    Mso::FutureWait(future3);
  }

  TEST_METHOD(PromiseGroup_SetValue_Copy) {
    Mso::PromiseGroup<std::vector<int>> p1;
    std::vector<int> vec({1, 2, 3});
    p1.SetValue(vec);

    // modify original vector and see that the Promise still has the original copy.
    vec.push_back(4);

    auto future1 = p1.AddFuture().Then([&](std::vector<int> & value) noexcept {
      std::vector<int> v1 = std::move(value);
      TestCheckEqual(3, v1.size());
    });
    auto future2 = p1.AddFuture().Then([&](std::vector<int> & value) noexcept {
      std::vector<int> v1 = std::move(value);
      TestCheckEqual(3, v1.size());
    });
    auto future3 = p1.AddFuture().Then([&](std::vector<int> & value) noexcept {
      std::vector<int> v1 = std::move(value);
      TestCheckEqual(3, v1.size());
    });

    Mso::FutureWait(future1);
    Mso::FutureWait(future2);
    Mso::FutureWait(future3);
    TestCheckEqual(4, vec.size());
  }

  TEST_METHOD(PromiseGroup_SetValue_Moves) {
    // Note that we cannot use move-only types like std::unique_ptr with the PromiseGroup.
    Mso::PromiseGroup<std::vector<int>> p1;
    std::vector<int> vec({1, 2, 3});
    p1.SetValue(std::move(vec));

    TestCheck(vec.empty());

    auto future1 = p1.AddFuture().Then([&](std::vector<int> & value) noexcept {
      std::vector<int> v1 = std::move(value);
      TestCheckEqual(3, v1.size());
    });
    auto future2 = p1.AddFuture().Then([&](std::vector<int> & value) noexcept {
      std::vector<int> v1 = std::move(value);
      TestCheckEqual(3, v1.size());
    });
    auto future3 = p1.AddFuture().Then([&](std::vector<int> & value) noexcept {
      std::vector<int> v1 = std::move(value);
      TestCheckEqual(3, v1.size());
    });

    Mso::FutureWait(future1);
    Mso::FutureWait(future2);
    Mso::FutureWait(future3);
  }

  TESTMETHOD_REQUIRES_SEH(PromiseGroup_SetValue_Empty) {
    Mso::PromiseGroup<int> p1(nullptr);
    TestCheckCrash(p1.SetValue(5));
  }

  TESTMETHOD_REQUIRES_SEH(PromiseGroup_SetValue_AlreadySet) {
    Mso::PromiseGroup<int> p1;
    p1.SetValue(5);
    TestCheckCrash(p1.SetValue(5));
  }

  TESTMETHOD_REQUIRES_SEH(PromiseGroup_SetValue_AlreadyAbandoned) {
    Mso::PromiseGroup<int> p1;
    TestCheck(p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
    TestCheckCrash(p1.SetValue(5));
  }

  TEST_METHOD(PromiseGroup_TrySetValue_Completes) {
    Mso::PromiseGroup<int> p1;
    TestCheck(p1.TrySetValue(5));
    TestCheck(Mso::GetIFuture(p1)->IsDone());
    TestCheck(Mso::GetIFuture(p1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(p1)->IsFailed());
  };

  TEST_METHOD(PromiseGroup_TrySetValue_Observe) {
    Mso::PromiseGroup<int> p1;
    auto future = p1.AddFuture().Then([&](int value) noexcept { TestCheck(value == 5); });

    TestCheck(p1.TrySetValue(5));
    Mso::FutureWait(future);
  }

  TEST_METHOD(PromiseGroup_TrySetValue_Copy) {
    Mso::PromiseGroup<std::vector<int>> p1;
    std::vector<int> vec({1, 2, 3});
    TestCheck(p1.TrySetValue(vec));

    // modify original vector and see that the Promise still has the original copy.
    vec.push_back(4);

    auto future1 =
        p1.AddFuture().Then([&](const std::vector<int> &value) noexcept { TestCheckEqual(3, value.size()); });
    auto future2 =
        p1.AddFuture().Then([&](const std::vector<int> &value) noexcept { TestCheckEqual(3, value.size()); });
    auto future3 =
        p1.AddFuture().Then([&](const std::vector<int> &value) noexcept { TestCheckEqual(3, value.size()); });

    Mso::FutureWait(future1);
    Mso::FutureWait(future2);
    Mso::FutureWait(future3);
    TestCheckEqual(4, vec.size());
  }

  TEST_METHOD(PromiseGroup_TrySetValue_Moves) {
    Mso::PromiseGroup<std::vector<int>> p1;
    std::vector<int> vec({1, 2, 3});
    TestCheck(p1.TrySetValue(std::move(vec)));

    // modify original vector and see that the Promise still has the original copy.
    TestCheck(vec.empty());

    auto future1 =
        p1.AddFuture().Then([&](const std::vector<int> &value) noexcept { TestCheckEqual(3, value.size()); });
    auto future2 =
        p1.AddFuture().Then([&](const std::vector<int> &value) noexcept { TestCheckEqual(3, value.size()); });
    auto future3 =
        p1.AddFuture().Then([&](const std::vector<int> &value) noexcept { TestCheckEqual(3, value.size()); });

    Mso::FutureWait(future1);
    Mso::FutureWait(future2);
    Mso::FutureWait(future3);
  }

  TESTMETHOD_REQUIRES_SEH(PromiseGroup_TrySetValue_Empty) {
    Mso::PromiseGroup<int> p1(nullptr);
    TestCheckCrash(p1.TrySetValue(5));
  }

  TEST_METHOD(PromiseGroup_TrySetValue_AlreadySet) {
    Mso::PromiseGroup<int> p1;
    TestCheck(p1.TrySetValue(5));
    TestCheck(!p1.TrySetValue(5));
  }

  TEST_METHOD(PromiseGroup_TrySetValue_AlreadyAbandoned) {
    Mso::PromiseGroup<int> p1;
    TestCheck(p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
    TestCheck(!p1.TrySetValue(5));
  }

  TEST_METHOD(PromiseGroup_EmplaceValue_Completes) {
    Mso::PromiseGroup<int> p1;
    p1.EmplaceValue(5);
    TestCheck(Mso::GetIFuture(p1)->IsDone());
    TestCheck(Mso::GetIFuture(p1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(PromiseGroup_EmplaceValue_Observe) {
    Mso::PromiseGroup<int> p1;
    auto future = p1.AddFuture().Then([&](int value) noexcept { TestCheck(value == 5); });

    p1.EmplaceValue(5);
    Mso::FutureWait(future);
  }

  TEST_METHOD(PromiseGroup_EmplaceValue_pair) {
    Mso::PromiseGroup<std::pair<int, double>> p1;
    p1.EmplaceValue(1, 2.3);

    auto future1 = p1.AddFuture().Then([&](const std::pair<int, double> &value) noexcept {
      TestCheckEqual(1, value.first);
      TestCheckEqual(2.3, value.second);
    });

    Mso::FutureWait(future1);
  }

  TEST_METHOD(PromiseGroup_EmplaceValue_Vector_init) {
    Mso::PromiseGroup<std::vector<int>> p1;
    p1.EmplaceValue({1, 2, 3});

    auto future1 =
        p1.AddFuture().Then([&](const std::vector<int> &value) noexcept { TestCheckEqual(3, value.size()); });
    auto future2 =
        p1.AddFuture().Then([&](const std::vector<int> &value) noexcept { TestCheckEqual(3, value.size()); });
    auto future3 =
        p1.AddFuture().Then([&](const std::vector<int> &value) noexcept { TestCheckEqual(3, value.size()); });

    Mso::FutureWait(future1);
    Mso::FutureWait(future2);
    Mso::FutureWait(future3);
  }

  TEST_METHOD(PromiseGroup_EmplaceValue_NoArgs) {
    Mso::PromiseGroup<EmplacedType> p1;
    auto future =
        p1.AddFuture().Then([&](const EmplacedType &value) noexcept { ValidateEmplacedType(value, 1, 2, 3); });

    p1.EmplaceValue();
    Mso::FutureWait(future);
  }

  TEST_METHOD(PromiseGroup_EmplaceValue_OneArg) {
    Mso::PromiseGroup<EmplacedType> p1;
    auto future =
        p1.AddFuture().Then([&](const EmplacedType &value) noexcept { ValidateEmplacedType(value, 5, 2, 3); });

    p1.EmplaceValue(5);
    Mso::FutureWait(future);
  }

  TEST_METHOD(PromiseGroup_EmplaceValue_TwoArgs) {
    Mso::PromiseGroup<EmplacedType> p1;
    auto future =
        p1.AddFuture().Then([&](const EmplacedType &value) noexcept { ValidateEmplacedType(value, 5, 6, 3); });

    p1.EmplaceValue(5, 6);
    Mso::FutureWait(future);
  }

  TEST_METHOD(PromiseGroup_EmplaceValue_ThreeArgs) {
    Mso::PromiseGroup<EmplacedType> p1;
    auto future =
        p1.AddFuture().Then([&](const EmplacedType &value) noexcept { ValidateEmplacedType(value, 5, 6, 7); });

    p1.EmplaceValue(5, 6, 7);
    Mso::FutureWait(future);
  }

  TESTMETHOD_REQUIRES_SEH(PromiseGroup_EmplaceValue_Empty) {
    Mso::PromiseGroup<int> p1(nullptr);
    TestCheckCrash(p1.EmplaceValue(5));
  }

  TESTMETHOD_REQUIRES_SEH(PromiseGroup_EmplaceValue_AlreadySet) {
    Mso::PromiseGroup<int> p1;
    p1.EmplaceValue(5);
    TestCheckCrash(p1.EmplaceValue(5));
  }

  TESTMETHOD_REQUIRES_SEH(PromiseGroup_EmplaceValue_AlreadyAbandoned) {
    Mso::PromiseGroup<int> p1;
    TestCheck(p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
    TestCheckCrash(p1.EmplaceValue(5));
  }

  TEST_METHOD(PromiseGroup_TryEmplaceValue_Completes) {
    Mso::PromiseGroup<int> p1;
    TestCheck(p1.TryEmplaceValue(5));
    TestCheck(Mso::GetIFuture(p1)->IsDone());
    TestCheck(Mso::GetIFuture(p1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(PromiseGroup_TryEmplaceValue_Observe) {
    Mso::PromiseGroup<int> p1;
    auto future = p1.AddFuture().Then([&](int value) noexcept { TestCheck(value == 5); });

    TestCheck(p1.TryEmplaceValue(5));
    Mso::FutureWait(future);
  }

  TEST_METHOD(PromiseGroup_TryEmplaceValue_pair) {
    Mso::PromiseGroup<std::pair<int, double>> p1;
    TestCheck(p1.TryEmplaceValue(1, 2.3));

    auto future1 = p1.AddFuture().Then([&](const std::pair<int, double> &value) noexcept {
      TestCheckEqual(1, value.first);
      TestCheckEqual(2.3, value.second);
    });

    Mso::FutureWait(future1);
  }

  TEST_METHOD(PromiseGroup_TryEmplaceValue_Vector_init) {
    Mso::PromiseGroup<std::vector<int>> p1;
    TestCheck(p1.TryEmplaceValue({1, 2, 3}));

    auto future1 =
        p1.AddFuture().Then([&](const std::vector<int> &value) noexcept { TestCheckEqual(3, value.size()); });
    auto future2 =
        p1.AddFuture().Then([&](const std::vector<int> &value) noexcept { TestCheckEqual(3, value.size()); });
    auto future3 =
        p1.AddFuture().Then([&](const std::vector<int> &value) noexcept { TestCheckEqual(3, value.size()); });

    Mso::FutureWait(future1);
    Mso::FutureWait(future2);
    Mso::FutureWait(future3);
  }

  TEST_METHOD(PromiseGroup_TryEmplaceValue_NoArgs) {
    Mso::PromiseGroup<EmplacedType> p1;
    auto future =
        p1.AddFuture().Then([&](const EmplacedType &value) noexcept { ValidateEmplacedType(value, 1, 2, 3); });

    TestCheck(p1.TryEmplaceValue());
    Mso::FutureWait(future);
  }

  TEST_METHOD(PromiseGroup_TryEmplaceValue_OneArg) {
    Mso::PromiseGroup<EmplacedType> p1;
    auto future =
        p1.AddFuture().Then([&](const EmplacedType &value) noexcept { ValidateEmplacedType(value, 5, 2, 3); });

    TestCheck(p1.TryEmplaceValue(5));
    Mso::FutureWait(future);
  }

  TEST_METHOD(PromiseGroup_TryEmplaceValue_TwoArgs) {
    Mso::PromiseGroup<EmplacedType> p1;
    auto future =
        p1.AddFuture().Then([&](const EmplacedType &value) noexcept { ValidateEmplacedType(value, 5, 6, 3); });

    TestCheck(p1.TryEmplaceValue(5, 6));
    Mso::FutureWait(future);
  }

  TEST_METHOD(PromiseGroup_TryEmplaceValue_ThreeArgs) {
    Mso::PromiseGroup<EmplacedType> p1;
    auto future =
        p1.AddFuture().Then([&](const EmplacedType &value) noexcept { ValidateEmplacedType(value, 5, 6, 7); });

    TestCheck(p1.TryEmplaceValue(5, 6, 7));
    Mso::FutureWait(future);
  }

  TESTMETHOD_REQUIRES_SEH(PromiseGroup_TryEmplaceValue_Empty) {
    Mso::PromiseGroup<int> p1(nullptr);
    TestCheckCrash(p1.TryEmplaceValue(5));
  }

  TEST_METHOD(PromiseGroup_TryEmplaceValue_AlreadySet) {
    Mso::PromiseGroup<int> p1;
    TestCheck(p1.TryEmplaceValue(5));
    TestCheck(!p1.TryEmplaceValue(5));
  }

  TEST_METHOD(PromiseGroup_TryEmplaceValue_AlreadyAbandoned) {
    Mso::PromiseGroup<int> p1;
    TestCheck(p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
    TestCheck(!p1.TryEmplaceValue(5));
  }

  TEST_METHOD(PromiseGroup_TrySetError) {
    Mso::PromiseGroup<int> p1;
    TestCheck(p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
    TestCheck(Mso::GetIFuture(p1)->IsDone());
    TestCheck(!Mso::GetIFuture(p1)->IsSucceeded());
    TestCheck(Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(PromiseGroup_TrySetError_AlreadySet) {
    Mso::PromiseGroup<int> p1;
    p1.SetValue(5);
    TestCheck(!p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
    TestCheck(!Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(PromiseGroup_TrySetError_AlreadyAbandoned) {
    Mso::PromiseGroup<int> p1;
    TestCheck(p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
    TestCheck(!p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
    TestCheck(Mso::GetIFuture(p1)->IsFailed());
  }

  TESTMETHOD_REQUIRES_SEH(PromiseGroup_TrySetError_Empty) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();

    Mso::PromiseGroup<int> p1(nullptr);
    TestCheckCrash(p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
  }

  TEST_METHOD(PromiseGroup_operator_Equal) {
    Mso::PromiseGroup<int> p1;
    Mso::PromiseGroup<int> p2(p1); // copy
    Mso::PromiseGroup<int> p3;
    Mso::PromiseGroup<int> p4(nullptr);
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

  TEST_METHOD(PromiseGroupVoid_ctor_Default) {
    Mso::PromiseGroup<void> p1;
    TestCheck(p1);
  }

  TEST_METHOD(PromiseGroupVoid_ctor_nullptr) {
    Mso::PromiseGroup<void> p1(nullptr);
    TestCheck(!p1);
  }

  TEST_METHOD(PromiseGroupVoid_ctor_state) {
    Mso::PromiseGroup<void> p1;
    Mso::PromiseGroup<void> p2(Mso::CntPtr<Mso::Futures::IFuture>(Mso::GetIFuture(p1)));
    TestCheck(p1);
    TestCheck(p1 == p2);
  }

  TEST_METHOD(PromiseGroupVoid_ctor_copy) {
    Mso::PromiseGroup<void> p1;
    Mso::PromiseGroup<void> p2(p1);
    TestCheck(p1);
    TestCheck(p1 == p2);
  }

  TEST_METHOD(PromiseGroupVoid_ctor_move) {
    Mso::PromiseGroup<void> p1;
    Mso::PromiseGroup<void> p2(p1); // copy of p1
    Mso::PromiseGroup<void> p3(std::move(p1));
    TestCheck(!p1);
    TestCheck(p3);
    TestCheck(p3 == p2);
  }

  TEST_METHOD(PromiseGroupVoid_Assign_nullptr) {
    Mso::PromiseGroup<void> p1;
    p1 = nullptr;
    TestCheck(!p1);
  }

  TEST_METHOD(PromiseGroupVoid_Assign_copy) {
    Mso::PromiseGroup<void> p1;
    Mso::PromiseGroup<void> p2;
    TestCheck(p1 != p2);
    Mso::PromiseGroup<void> p3(p1); // copy of p1
    p1 = p2;
    TestCheck(p2);
    TestCheck(p1 == p2);
    TestCheck(p1 != p3);
  }

  TEST_METHOD(PromiseGroupVoid_Assign_move) {
    Mso::PromiseGroup<void> p1;
    Mso::PromiseGroup<void> p2;
    Mso::PromiseGroup<void> p3(p1); // copy of p1
    Mso::PromiseGroup<void> p4(p2); // copy of p2
    p1 = std::move(p2);
    TestCheck(p1);
    TestCheck(!p2);
    TestCheck(p1 != p3);
    TestCheck(p1 == p4);
  }

  TEST_METHOD(PromiseGroupVoid_Assign_EmptyInit) {
    Mso::PromiseGroup<void> p1;
    Mso::PromiseGroup<void> p2(p1); // copy of p1
    p1 = {}; // Assigns new non-empty Promise
    TestCheck(p1);
    TestCheck(p2);
    TestCheck(p1 != p2);
  }

  TEST_METHOD(PromiseGroupVoid_dtor_Unfulfilled) {
    Mso::PromiseGroup<void> p1;
    // This Promise is unfulfilled but no one observes it. We let it to be destroyed without any side effects.
  }

  TEST_METHOD(PromiseGroupVoid_dtor_Unfulfilled_Observed) {
    Mso::PromiseGroup<void> p1;
    Mso::Future<void> f1 = p1.AddFuture().Then([]() noexcept {});

    // Move p1 to the local variable p2 which should destroy the promise group when goes out of scope.
    { Mso::PromiseGroup<void> p2(std::move(p1)); }

    TestCheck(Mso::FutureWaitIsFailed(f1));
  }

  TEST_METHOD(PromiseGroupVoid_Swap) {
    Mso::PromiseGroup<void> p1;
    Mso::PromiseGroup<void> p2;
    Mso::PromiseGroup<void> p3(p1); // copy of p1
    Mso::PromiseGroup<void> p4(p2); // copy of p2
    p1.Swap(p2);
    TestCheck(p1 == p4);
    TestCheck(p2 == p3);
  }

  TEST_METHOD(PromiseGroupVoid_std_swap) {
    using std::swap; // The typical pattern how to call the swap method.
    Mso::PromiseGroup<void> p1;
    Mso::PromiseGroup<void> p2;
    Mso::PromiseGroup<void> p3(p1); // copy of p1
    Mso::PromiseGroup<void> p4(p2); // copy of p2
    swap(p1, p2); // Never use the std::swap call. The swap can be overridden in the custom namespace for a type T.
    TestCheck(p1 == p4);
    TestCheck(p2 == p3);
  }

  TEST_METHOD(PromiseGroupVoid_GetIFuture) {
    Mso::PromiseGroup<void> p1;
    Mso::PromiseGroup<void> p2(nullptr);
    TestCheck(Mso::GetIFuture(p1) != nullptr);
    TestCheck(Mso::GetIFuture(p2) == nullptr);
  }

  TEST_METHOD(PromiseGroupVoid_operator_bool) {
    Mso::PromiseGroup<void> p1;
    Mso::PromiseGroup<void> p2(nullptr);
    TestCheck(p1);
    TestCheck(!p2);
  }

  TEST_METHOD(PromiseGroupVoid_AddFuture) {
    Mso::PromiseGroup<void> p1;
    Mso::Future<void> f1 = p1.AddFuture();
    TestCheck(f1);
  }

  TEST_METHOD(PromiseGroupVoid_AddFuture_three) {
    Mso::PromiseGroup<void> p1;
    Mso::Future<void> f1 = p1.AddFuture();
    Mso::Future<void> f2 = p1.AddFuture();
    Mso::Future<void> f3 = p1.AddFuture();
    TestCheck(f1);
    TestCheck(f2);
    TestCheck(f3);
  }

  TEST_METHOD(PromiseGroupVoid_SetValue_Completes) {
    Mso::PromiseGroup<void> p1;
    p1.SetValue();
    TestCheck(Mso::GetIFuture(p1)->IsDone());
    TestCheck(Mso::GetIFuture(p1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(PromiseGroupVoid_SetValue_Observe) {
    Mso::PromiseGroup<void> p1;
    auto future = p1.AddFuture().Then([&]() noexcept {});

    p1.SetValue();
    Mso::FutureWait(future);
  }

  TEST_METHOD(PromiseGroupVoid_SetValue_Observe_three) {
    Mso::PromiseGroup<void> p1;
    auto future1 = p1.AddFuture().Then([&]() noexcept {});
    auto future2 = p1.AddFuture().Then([&]() noexcept {});
    auto future3 = p1.AddFuture().Then([&]() noexcept {});

    p1.SetValue();
    Mso::FutureWait(future1);
    Mso::FutureWait(future2);
    Mso::FutureWait(future3);
  }

  TESTMETHOD_REQUIRES_SEH(PromiseGroupVoid_SetValue_Empty) {
    Mso::PromiseGroup<void> p1(nullptr);
    TestCheckCrash(p1.SetValue());
  }

  TESTMETHOD_REQUIRES_SEH(PromiseGroupVoid_SetValue_AlreadySet) {
    Mso::PromiseGroup<void> p1;
    p1.SetValue();
    TestCheckCrash(p1.SetValue());
  }

  TESTMETHOD_REQUIRES_SEH(PromiseGroupVoid_SetValue_AlreadyFailed) {
    Mso::PromiseGroup<void> p1;
    TestCheck(p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
    TestCheckCrash(p1.SetValue());
  }

  TEST_METHOD(PromiseGroupVoid_TrySetValue) {
    Mso::PromiseGroup<void> p1;
    TestCheck(p1.TrySetValue());
    TestCheck(Mso::GetIFuture(p1)->IsDone());
    TestCheck(Mso::GetIFuture(p1)->IsSucceeded());
    TestCheck(!Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(PromiseGroupVoid_TrySetValue_Observe) {
    Mso::PromiseGroup<void> p1;
    auto future = p1.AddFuture().Then([&]() noexcept {});

    TestCheck(p1.TrySetValue());
    Mso::FutureWait(future);
  }

  TESTMETHOD_REQUIRES_SEH(PromiseGroupVoid_TrySetValue_Empty) {
    Mso::PromiseGroup<void> p1(nullptr);
    TestCheckCrash(p1.TrySetValue());
  }

  TEST_METHOD(PromiseGroupVoid_TrySetValue_AlreadySet) {
    Mso::PromiseGroup<void> p1;
    TestCheck(p1.TrySetValue());
    TestCheck(!p1.TrySetValue());
  }

  TEST_METHOD(PromiseGroupVoid_TrySetValue_AlreadyAbandoned) {
    Mso::PromiseGroup<void> p1;
    TestCheck(p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
    TestCheck(!p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
  }

  TEST_METHOD(PromiseGroupVoid_TrySetError) {
    Mso::PromiseGroup<void> p1;
    TestCheck(p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
    TestCheck(Mso::GetIFuture(p1)->IsDone());
    TestCheck(!Mso::GetIFuture(p1)->IsSucceeded());
    TestCheck(Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(PromiseGroupVoid_TrySetError_AlreadySet) {
    Mso::PromiseGroup<void> p1;
    p1.SetValue();
    TestCheck(!p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
    TestCheck(!Mso::GetIFuture(p1)->IsFailed());
  }

  TEST_METHOD(PromiseGroupVoid_TrySetError_AlreadyAbandoned) {
    Mso::PromiseGroup<void> p1;
    TestCheck(p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
    TestCheck(!p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
    TestCheck(Mso::GetIFuture(p1)->IsFailed());
  }

  TESTMETHOD_REQUIRES_SEH(PromiseGroupVoid_TrySetError_Empty) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();

    Mso::PromiseGroup<void> p1(nullptr);
    TestCheckCrash(p1.TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true)));
  }

  TEST_METHOD(PromiseGroupVoid_operator_Equal) {
    Mso::PromiseGroup<void> p1;
    Mso::PromiseGroup<void> p2(p1); // copy
    Mso::PromiseGroup<void> p3;
    Mso::PromiseGroup<void> p4(nullptr);
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
