// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "future/future.h"
#include "future/futureWait.h"
#include "motifCpp/libletAwareMemLeakDetection.h"
#include "testCheck.h"
#include "testExecutor.h"

namespace FutureTests {

TEST_CLASS_EX (FutureWeakPtrTest, LibletAwareMemLeakDetection) {
  // MemoryLeakDetectionHook::TrackPerTest m_trackLeakPerTest;

  TEST_METHOD(FutureWeakPtr_IsEmpty) {
    Mso::WeakPtr<Mso::Future<int>> weakPtr1;
    TestCheck(weakPtr1.IsEmpty());

    auto f2 = Mso::MakeCompletedFuture(5);

    Mso::WeakPtr<Mso::Future<int>> weakPtr2(f2);
    TestCheck(!weakPtr2.IsEmpty());
  }

  TEST_METHOD(FutureWeakPtr_IsExpired) {
    Mso::WeakPtr<Mso::Future<int>> weakPtr1;
    TestCheck(weakPtr1.IsExpired());

    auto f2 = Mso::MakeCompletedFuture(5);

    Mso::WeakPtr<Mso::Future<int>> weakPtr2(f2);
    TestCheck(!weakPtr2.IsExpired());

    Mso::WeakPtr<Mso::Future<int>> weakPtr3;
    {
      auto f3 = Mso::MakeCompletedFuture(5);
      weakPtr3 = f3;
    }
    TestCheck(weakPtr3.IsExpired());
  }

  TEST_METHOD(FutureWeakPtr_Reset) {
    auto f1 = Mso::MakeCompletedFuture(5);

    Mso::WeakPtr<Mso::Future<int>> weakPtr(f1);
    TestCheck(!weakPtr.IsEmpty());
    TestCheck(!weakPtr.IsExpired());

    weakPtr.Reset();
    TestCheck(weakPtr.IsEmpty());
    TestCheck(weakPtr.IsExpired());
  }

  TEST_METHOD(FutureWeakPtr_GetFuture) {
    auto f1 = Mso::MakeCompletedFuture(5);

    Mso::WeakPtr<Mso::Future<int>> weakPtr = f1;
    TestCheck(!weakPtr.IsEmpty());
    TestCheck(!weakPtr.IsExpired());

    Mso::Future<int> future = weakPtr.GetFuture();
    TestCheck(GetIFuture(future) != nullptr);
    TestCheck(future == f1);
  }

  TEST_METHOD(FutureWeakPtr_ctor_Default) {
    Mso::WeakPtr<Mso::Future<int>> weakPtr;
    TestCheck(weakPtr.IsEmpty());

    Mso::Future<int> future = weakPtr.GetFuture();
    TestCheck(GetIFuture(future) == nullptr);
  }

  TEST_METHOD(FutureWeakPtr_ctor_nullptr) {
    Mso::WeakPtr<Mso::Future<int>> weakPtr(nullptr);
    TestCheck(weakPtr.IsEmpty());

    Mso::Future<int> future = weakPtr.GetFuture();
    TestCheck(GetIFuture(future) == nullptr);
  }

  TEST_METHOD(FutureWeakPtr_ctor_future) {
    auto f1 = Mso::MakeCompletedFuture(5);

    Mso::WeakPtr<Mso::Future<int>> weakPtr(f1);
    TestCheck(!weakPtr.IsEmpty());
    TestCheck(!weakPtr.IsExpired());

    Mso::Future<int> future = weakPtr.GetFuture();
    TestCheck(GetIFuture(future) != nullptr);
    TestCheck(future == f1);
  }

  TEST_METHOD(FutureWeakPtr_ctor_copy) {
    auto f1 = Mso::MakeCompletedFuture(5);

    Mso::WeakPtr<Mso::Future<int>> weakPtr1(f1);
    Mso::WeakPtr<Mso::Future<int>> weakPtr2(weakPtr1);

    Mso::Future<int> future1 = weakPtr1.GetFuture();
    TestCheck(!weakPtr1.IsEmpty());
    TestCheck(!weakPtr1.IsExpired());
    TestCheck(GetIFuture(future1) != nullptr);
    TestCheck(future1 == f1);

    Mso::Future<int> future2 = weakPtr2.GetFuture();
    TestCheck(!weakPtr2.IsEmpty());
    TestCheck(!weakPtr2.IsExpired());
    TestCheck(GetIFuture(future2) != nullptr);
    TestCheck(future2 == f1);

    TestCheck(GetIFuture(future1) == GetIFuture(future2));
  }

  TEST_METHOD(FutureWeakPtr_ctor_move) {
    auto f1 = Mso::MakeCompletedFuture(5);

    Mso::WeakPtr<Mso::Future<int>> weakPtr1(f1);
    Mso::WeakPtr<Mso::Future<int>> weakPtr2(std::move(weakPtr1));

    Mso::Future<int> future1 = weakPtr1.GetFuture();
    TestCheck(weakPtr1.IsEmpty());
    TestCheck(weakPtr1.IsExpired());
    TestCheck(GetIFuture(future1) == nullptr);
    TestCheck(future1 != f1);

    Mso::Future<int> future2 = weakPtr2.GetFuture();
    TestCheck(!weakPtr2.IsEmpty());
    TestCheck(!weakPtr2.IsExpired());
    TestCheck(GetIFuture(future2) != nullptr);
    TestCheck(future2 == f1);

    TestCheck(GetIFuture(future1) != GetIFuture(future2));
  }

  TEST_METHOD(FutureWeakPtr_assignment_nullptr) {
    Mso::WeakPtr<Mso::Future<int>> weakPtr;
    weakPtr = nullptr;
    TestCheck(weakPtr.IsEmpty());

    Mso::Future<int> future = weakPtr.GetFuture();
    TestCheck(GetIFuture(future) == nullptr);
  }

  TEST_METHOD(FutureWeakPtr_assignment_future) {
    auto f1 = Mso::MakeCompletedFuture(5);

    Mso::WeakPtr<Mso::Future<int>> weakPtr;
    weakPtr = f1;
    TestCheck(!weakPtr.IsEmpty());
    TestCheck(!weakPtr.IsExpired());

    Mso::Future<int> future = weakPtr.GetFuture();
    TestCheck(GetIFuture(future) != nullptr);
    TestCheck(future == f1);
  }

  TEST_METHOD(FutureWeakPtr_assignment_copy) {
    auto f1 = Mso::MakeCompletedFuture(5);

    Mso::WeakPtr<Mso::Future<int>> weakPtr1(f1);
    Mso::WeakPtr<Mso::Future<int>> weakPtr2;
    weakPtr2 = weakPtr1;

    Mso::Future<int> future1 = weakPtr1.GetFuture();
    TestCheck(!weakPtr1.IsEmpty());
    TestCheck(!weakPtr1.IsExpired());
    TestCheck(GetIFuture(future1) != nullptr);
    TestCheck(future1 == f1);

    Mso::Future<int> future2 = weakPtr2.GetFuture();
    TestCheck(!weakPtr2.IsEmpty());
    TestCheck(!weakPtr2.IsExpired());
    TestCheck(GetIFuture(future2) != nullptr);
    TestCheck(future2 == f1);

    TestCheck(GetIFuture(future1) == GetIFuture(future2));
  }

  TEST_METHOD(FutureWeakPtr_assignment_move) {
    auto f1 = Mso::MakeCompletedFuture(5);

    Mso::WeakPtr<Mso::Future<int>> weakPtr1(f1);
    Mso::WeakPtr<Mso::Future<int>> weakPtr2;
    weakPtr2 = std::move(weakPtr1);

    Mso::Future<int> future1 = weakPtr1.GetFuture();
    TestCheck(weakPtr1.IsEmpty());
    TestCheck(weakPtr1.IsExpired());
    TestCheck(GetIFuture(future1) == nullptr);
    TestCheck(future1 != f1);

    Mso::Future<int> future2 = weakPtr2.GetFuture();
    TestCheck(!weakPtr2.IsEmpty());
    TestCheck(!weakPtr2.IsExpired());
    TestCheck(GetIFuture(future2) != nullptr);
    TestCheck(future2 == f1);

    TestCheck(GetIFuture(future1) != GetIFuture(future2));
  }

  TEST_METHOD(FutureWeakPtr_Swap) {
    auto f1 = Mso::MakeCompletedFuture(5);

    Mso::WeakPtr<Mso::Future<int>> weakPtr1(f1);

    Mso::Future<int> future1 = weakPtr1.GetFuture();
    TestCheck(!weakPtr1.IsEmpty());
    TestCheck(!weakPtr1.IsExpired());
    TestCheck(GetIFuture(future1) != nullptr);
    TestCheck(future1 == f1);

    auto f2 = Mso::MakeCompletedFuture(5);
    Mso::WeakPtr<Mso::Future<int>> weakPtr2(f2);

    Mso::Future<int> future2 = weakPtr2.GetFuture();
    TestCheck(!weakPtr2.IsEmpty());
    TestCheck(!weakPtr2.IsExpired());
    TestCheck(GetIFuture(future2) != nullptr);
    TestCheck(future2 == f2);

    TestCheck(GetIFuture(future1) != GetIFuture(future2));

    weakPtr1.Swap(weakPtr2);

    Mso::Future<int> future3 = weakPtr1.GetFuture();
    TestCheck(!weakPtr1.IsEmpty());
    TestCheck(!weakPtr1.IsExpired());
    TestCheck(GetIFuture(future3) != nullptr);
    TestCheck(future3 == f2);

    Mso::Future<int> future4 = weakPtr2.GetFuture();
    TestCheck(!weakPtr1.IsEmpty());
    TestCheck(!weakPtr1.IsExpired());
    TestCheck(GetIFuture(future4) != nullptr);
    TestCheck(future4 == f1);
  }

  TEST_METHOD(FutureWeakPtr_std_swap) {
    auto f1 = Mso::MakeCompletedFuture(5);

    Mso::WeakPtr<Mso::Future<int>> weakPtr1(f1);

    Mso::Future<int> future1 = weakPtr1.GetFuture();
    TestCheck(!weakPtr1.IsEmpty());
    TestCheck(!weakPtr1.IsExpired());
    TestCheck(GetIFuture(future1) != nullptr);
    TestCheck(future1 == f1);

    auto f2 = Mso::MakeCompletedFuture(5);
    Mso::WeakPtr<Mso::Future<int>> weakPtr2(f2);

    Mso::Future<int> future2 = weakPtr2.GetFuture();
    TestCheck(!weakPtr2.IsEmpty());
    TestCheck(!weakPtr2.IsExpired());
    TestCheck(GetIFuture(future2) != nullptr);
    TestCheck(future2 == f2);

    TestCheck(GetIFuture(future1) != GetIFuture(future2));

    std::swap(weakPtr1, weakPtr2);

    Mso::Future<int> future3 = weakPtr1.GetFuture();
    TestCheck(!weakPtr1.IsEmpty());
    TestCheck(!weakPtr1.IsExpired());
    TestCheck(GetIFuture(future3) != nullptr);
    TestCheck(future3 == f2);

    Mso::Future<int> future4 = weakPtr2.GetFuture();
    TestCheck(!weakPtr1.IsEmpty());
    TestCheck(!weakPtr1.IsExpired());
    TestCheck(GetIFuture(future4) != nullptr);
    TestCheck(future4 == f1);
  }
};

} // namespace FutureTests
