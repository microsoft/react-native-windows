// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "future/cancellationToken.h"
#include "motifCpp/libletAwareMemLeakDetection.h"
#include "object/refCountedObject.h"
#include "testCheck.h"
#include "testExecutor.h"

namespace FutureTests {

struct DestroyCheck : Mso::RefCountedObjectNoVTable<DestroyCheck> {
  DestroyCheck(bool &isDestroyed) noexcept : m_isDestroyed(isDestroyed) {}

  ~DestroyCheck() noexcept {
    m_isDestroyed = true;
  }

 private:
  bool &m_isDestroyed;
};

TEST_CLASS_EX (CancellationTokenTest, LibletAwareMemLeakDetection) {
  static Mso::CancellationToken GetEmptyCancellationToken() noexcept {
    Mso::CancellationTokenSource tokenSource;
    Mso::CancellationToken token = tokenSource.GetToken();
    token.Clear();
    return token;
  }

  TEST_METHOD(CancellationTokenSource_ctor_Default) {
    Mso::CancellationTokenSource t1;
    TestCheck(t1);
  }

  TEST_METHOD(CancellationTokenSource_ctor_copy) {
    Mso::CancellationTokenSource t1;
    Mso::CancellationTokenSource t2(t1);
    TestCheck(t1);
    TestCheck(t1 == t2);
  }

  TEST_METHOD(CancellationTokenSource_ctor_move) {
    Mso::CancellationTokenSource t1;
    Mso::CancellationTokenSource t2(t1); // copy of t1
    Mso::CancellationTokenSource t3(std::move(t1));
    TestCheck(!t1);
    TestCheck(t3);
    TestCheck(t3 == t2);
  }

  TEST_METHOD(CancellationTokenSource_Assign_copy) {
    Mso::CancellationTokenSource t1;
    Mso::CancellationTokenSource t2;
    TestCheck(t1 != t2);
    Mso::CancellationTokenSource t3(t1); // copy of t1
    t1 = t2;
    TestCheck(t2);
    TestCheck(t1 == t2);
    TestCheck(t1 != t3);
  }

  TEST_METHOD(CancellationTokenSource_Assign_move) {
    Mso::CancellationTokenSource t1;
    Mso::CancellationTokenSource t2;
    Mso::CancellationTokenSource t3(t1); // copy of t1
    Mso::CancellationTokenSource t4(t2); // copy of t2
    t1 = std::move(t2);
    TestCheck(t1);
    TestCheck(!t2);
    TestCheck(t1 != t3);
    TestCheck(t1 == t4);
  }

  TEST_METHOD(CancellationTokenSource_Swap) {
    Mso::CancellationTokenSource t1;
    Mso::CancellationTokenSource t2;
    Mso::CancellationTokenSource t3(t1); // copy of t1
    Mso::CancellationTokenSource t4(t2); // copy of t2
    t1.Swap(t2);
    TestCheck(t1 == t4);
    TestCheck(t2 == t3);
  }

  TEST_METHOD(CancellationTokenSource_std_swap) {
    using std::swap; // The typical pattern how to call the swap method.
    Mso::CancellationTokenSource t1;
    Mso::CancellationTokenSource t2;
    Mso::CancellationTokenSource t3(t1); // copy of t1
    Mso::CancellationTokenSource t4(t2); // copy of t2
    swap(t1, t2); // Never use the std::swap call. The swap can be overridden in
                  // the custom namespace for a type T.
    TestCheck(t1 == t4);
    TestCheck(t2 == t3);
  }

  TEST_METHOD(CancellationTokenSource_GetIFuture) {
    Mso::CancellationTokenSource t1;
    Mso::CancellationTokenSource t2;
    t2.Clear();
    TestCheck(Mso::GetIFuture(t1) != nullptr);
    TestCheck(Mso::GetIFuture(t2) == nullptr);
  }

  TEST_METHOD(CancellationTokenSource_Clear) {
    Mso::CancellationTokenSource t1;
    TestCheck(Mso::GetIFuture(t1) != nullptr);
    t1.Clear();
    TestCheck(Mso::GetIFuture(t1) == nullptr);
  }

  TEST_METHOD(CancellationTokenSource_operator_bool) {
    Mso::CancellationTokenSource t1;
    Mso::CancellationTokenSource t2;
    t2.Clear();
    TestCheck((bool)t1);
    TestCheck(!(bool)t2);
  }

  TEST_METHOD(CancellationTokenSource_operator_Equal) {
    Mso::CancellationTokenSource t1;
    Mso::CancellationTokenSource t2(t1); // copy
    Mso::CancellationTokenSource t3;
    Mso::CancellationTokenSource t4;
    t4.Clear();
    TestCheck(t1 == t2);
    TestCheck(t2 == t1);
    TestCheck(t1 != t3);
    TestCheck(t3 != t1);
    TestCheck(t4 != t1);
    TestCheck(t1 != t4);
    TestCheck(t4 == nullptr);
    TestCheck(nullptr == t4);
    TestCheck(t1 != nullptr);
    TestCheck(nullptr != t1);
  }

  TEST_METHOD(CancellationTokenSource_GetToken) {
    Mso::CancellationTokenSource ts1;
    Mso::CancellationToken t1 = ts1.GetToken();
    TestCheck(t1);
  }

  TESTMETHOD_REQUIRES_SEH(CancellationTokenSource_GetToken_Empty) {
    Mso::CancellationTokenSource ts1;
    ts1.Clear();
    TestCheckCrash(ts1.GetToken());
  }

  TEST_METHOD(CancellationTokenSource_Cancel) {
    Mso::CancellationTokenSource ts1;
    ts1.Cancel();
    Mso::CancellationToken t1 = ts1.GetToken();
    TestCheck(t1.IsCanceled());
    TestCheck(Mso::GetIFuture(ts1)->IsSucceeded());
    TestCheck(Mso::GetIFuture(t1)->IsSucceeded());
  }

  TESTMETHOD_REQUIRES_SEH(CancellationTokenSource_Cancel_Empty) {
    Mso::CancellationTokenSource ts1;
    ts1.Clear();
    TestCheckCrash(ts1.Cancel());
  }

  TEST_METHOD(CancellationTokenSource_Abandon) {
    Mso::CancellationTokenSource ts1;
    ts1.Abandon();
    Mso::CancellationToken t1 = ts1.GetToken();
    TestCheck(!t1.IsCanceled());
    TestCheck(Mso::GetIFuture(ts1)->IsFailed());
    TestCheck(Mso::GetIFuture(t1)->IsSucceeded());
  }

  TESTMETHOD_REQUIRES_SEH(CancellationTokenSource_Abandon_Empty) {
    Mso::CancellationTokenSource ts1;
    ts1.Clear();
    TestCheckCrash(ts1.Abandon());
  }

  TEST_METHOD(CancellationTokenSource_dtor) {
    // When CancellationTokenSource is destroyed, we complete the cancellation
    // token with value false.
    Mso::CancellationToken t1(GetEmptyCancellationToken());
    {
      Mso::CancellationTokenSource ts1;
      t1 = ts1.GetToken();
    }
    TestCheck(!t1.IsCanceled());
    TestCheck(Mso::GetIFuture(t1)->IsSucceeded());
  }

  TEST_METHOD(CancellationToken_ctor_default) {
    Mso::CancellationToken t1;
    TestCheck(!t1);
  }

  TEST_METHOD(CancellationToken_ctor_nullptr) {
    Mso::CancellationToken t1{nullptr};
    TestCheck(!t1);
  }

  TEST_METHOD(CancellationToken_ctor_copy) {
    Mso::CancellationTokenSource ts1;
    Mso::CancellationToken t1 = ts1.GetToken();
    Mso::CancellationToken t2(t1);
    TestCheck(t1);
    TestCheck(t1 == t2);
  }

  TEST_METHOD(CancellationToken_ctor_move) {
    Mso::CancellationTokenSource ts1;
    Mso::CancellationToken t1 = ts1.GetToken();
    Mso::CancellationToken t2(t1); // copy of t1
    Mso::CancellationToken t3(std::move(t1));
    TestCheck(!t1);
    TestCheck(t3);
    TestCheck(t3 == t2);
  }

  TEST_METHOD(CancellationToken_Assign_copy) {
    Mso::CancellationTokenSource ts1;
    Mso::CancellationToken t1 = ts1.GetToken();
    Mso::CancellationTokenSource ts2;
    Mso::CancellationToken t2 = ts2.GetToken();
    TestCheck(t1 != t2);
    Mso::CancellationToken t3(t1); // copy of t1
    t1 = t2;
    TestCheck(t2);
    TestCheck(t1 == t2);
    TestCheck(t1 != t3);
  }

  TEST_METHOD(CancellationToken_Assign_move) {
    Mso::CancellationTokenSource ts1;
    Mso::CancellationToken t1 = ts1.GetToken();
    Mso::CancellationTokenSource ts2;
    Mso::CancellationToken t2 = ts2.GetToken();
    Mso::CancellationToken t3(t1); // copy of t1
    Mso::CancellationToken t4(t2); // copy of t2
    t1 = std::move(t2);
    TestCheck(t1);
    TestCheck(!t2);
    TestCheck(t1 != t3);
    TestCheck(t1 == t4);
  }

  TEST_METHOD(CancellationToken_Swap) {
    Mso::CancellationTokenSource ts1;
    Mso::CancellationToken t1 = ts1.GetToken();
    Mso::CancellationTokenSource ts2;
    Mso::CancellationToken t2 = ts2.GetToken();
    Mso::CancellationToken t3(t1); // copy of t1
    Mso::CancellationToken t4(t2); // copy of t2
    t1.Swap(t2);
    TestCheck(t1 == t4);
    TestCheck(t2 == t3);
  }

  TEST_METHOD(CancellationToken_std_swap) {
    using std::swap; // The typical pattern how to call the swap method.
    Mso::CancellationTokenSource ts1;
    Mso::CancellationToken t1 = ts1.GetToken();
    Mso::CancellationTokenSource ts2;
    Mso::CancellationToken t2 = ts2.GetToken();
    Mso::CancellationToken t3(t1); // copy of t1
    Mso::CancellationToken t4(t2); // copy of t2
    swap(t1, t2); // Never use the std::swap call. The swap can be overridden in
                  // the custom namespace for a type T.
    TestCheck(t1 == t4);
    TestCheck(t2 == t3);
  }

  TEST_METHOD(CancellationToken_GetIFuture) {
    Mso::CancellationTokenSource ts1;
    Mso::CancellationToken t1 = ts1.GetToken();
    Mso::CancellationToken t2(GetEmptyCancellationToken());
    TestCheck(Mso::GetIFuture(t1) != nullptr);
    TestCheck(Mso::GetIFuture(t2) == nullptr);
  }

  TEST_METHOD(CancellationToken_Clear) {
    Mso::CancellationTokenSource ts1;
    Mso::CancellationToken t1 = ts1.GetToken();
    TestCheck(Mso::GetIFuture(t1) != nullptr);
    t1.Clear();
    TestCheck(Mso::GetIFuture(t1) == nullptr);
  }

  TEST_METHOD(CancellationToken_operator_bool) {
    Mso::CancellationTokenSource ts1;
    Mso::CancellationToken t1 = ts1.GetToken();
    Mso::CancellationToken t2(GetEmptyCancellationToken());
    TestCheck((bool)t1);
    TestCheck(!(bool)t2);
  }

  TEST_METHOD(CancellationToken_operator_Equal) {
    Mso::CancellationTokenSource ts1;
    Mso::CancellationToken t1 = ts1.GetToken();
    Mso::CancellationToken t2(t1); // copy
    Mso::CancellationTokenSource ts3;
    Mso::CancellationToken t3 = ts3.GetToken();
    Mso::CancellationToken t4(GetEmptyCancellationToken());
    TestCheck(t1 == t2);
    TestCheck(t2 == t1);
    TestCheck(t1 != t3);
    TestCheck(t3 != t1);
    TestCheck(t4 != t1);
    TestCheck(t1 != t4);
    TestCheck(t4 == nullptr);
    TestCheck(nullptr == t4);
    TestCheck(t1 != nullptr);
    TestCheck(nullptr != t1);
  }

  TEST_METHOD(CancellationToken_WhenChanged_Cancel) {
    Mso::CancellationTokenSource ts1;
    bool isExecuted = false;
    bool isLambdaDestroyed = false;
    {
      auto dc = Mso::Make<DestroyCheck>(isLambdaDestroyed);
      ts1.GetToken().WhenChanged().Then(Mso::Executors::Inline{}, [&](bool isCanceled) noexcept {
        TestCheck(isCanceled);
        isExecuted = true;
      });
    }
    ts1.Cancel();
    TestCheck(isExecuted);
    TestCheck(isLambdaDestroyed);
  }

  TEST_METHOD(CancellationToken_WhenChanged_Abandon) {
    Mso::CancellationTokenSource ts1;
    bool isExecuted = false;
    bool isLambdaDestroyed = false;
    {
      auto dc = Mso::Make<DestroyCheck>(isLambdaDestroyed);
      ts1.GetToken().WhenChanged().Then(Mso::Executors::Inline{}, [&](bool isCanceled) noexcept {
        TestCheck(!isCanceled);
        isExecuted = true;
      });
    }
    ts1.Abandon();
    TestCheck(isExecuted);
    TestCheck(isLambdaDestroyed);
  }

  TEST_METHOD(CancellationToken_WhenChanged_dtor) {
    bool isExecuted = false;
    bool isLambdaDestroyed = false;
    Mso::CancellationToken t1(GetEmptyCancellationToken()); // To show that CancellationToken is a
                                                            // weak pointer
    {
      Mso::CancellationTokenSource ts1;
      auto dc = Mso::Make<DestroyCheck>(isLambdaDestroyed);
      t1 = ts1.GetToken();
      t1.WhenChanged().Then(Mso::Executors::Inline{}, [&](bool isCanceled) noexcept {
        TestCheck(!isCanceled);
        isExecuted = true;
      });
    }
    TestCheck(isExecuted);
    TestCheck(isLambdaDestroyed);
  }

  TEST_METHOD(CancellationToken_WhenCanceled_Cancel) {
    Mso::CancellationTokenSource ts1;
    bool isExecuted = false;
    bool isLambdaDestroyed = false;
    {
      auto dc = Mso::Make<DestroyCheck>(isLambdaDestroyed);
      ts1.GetToken().WhenCanceled([&]() noexcept { isExecuted = true; });
    }
    ts1.Cancel();
    TestCheck(isExecuted);
    TestCheck(isLambdaDestroyed);
  }

  TEST_METHOD(CancellationToken_WhenCanceled_Abandon) {
    Mso::CancellationTokenSource ts1;
    bool isExecuted = false;
    bool isLambdaDestroyed = false;
    {
      auto dc = Mso::Make<DestroyCheck>(isLambdaDestroyed);
      ts1.GetToken().WhenCanceled([&]() noexcept { isExecuted = true; });
    }
    ts1.Abandon();
    TestCheck(!isExecuted);
    TestCheck(isLambdaDestroyed);
  }

  TEST_METHOD(CancellationToken_WhenCanceled_dtor) {
    bool isExecuted = false;
    bool isLambdaDestroyed = false;
    Mso::CancellationToken t1(GetEmptyCancellationToken()); // To show that CancellationToken is a
                                                            // weak pointer
    {
      Mso::CancellationTokenSource ts1;
      auto dc = Mso::Make<DestroyCheck>(isLambdaDestroyed);
      t1 = ts1.GetToken();
      t1.WhenCanceled([&]() noexcept { isExecuted = true; });
    }
    TestCheck(!isExecuted);
    TestCheck(isLambdaDestroyed);
  }
};

} // namespace FutureTests
