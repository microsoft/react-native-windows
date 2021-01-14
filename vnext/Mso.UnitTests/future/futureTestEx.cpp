// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "future/future.h"
#include "future/futureWait.h"
#include "motifCpp/libletAwareMemLeakDetection.h"
#include "testCheck.h"
#include "testExecutor.h"

namespace FutureTests {

// An executor that immediately fails posted future instead of invoking it.
struct FailingExecutor {
  void Post(Mso::VoidFunctor &&callback) noexcept {
    query_cast<Mso::ICancellationListener &>(*callback.Get()).OnCancel();
  }

  template <class TCallback, class... TArgs>
  auto Invoke(TCallback &&callback, TArgs &&... args) noexcept {
    return callback(std::forward<TArgs>(args)...);
  }
};

TEST_CLASS_EX (FutureTestEx, LibletAwareMemLeakDetection) {
  // MemoryLeakDetectionHook::TrackPerTest m_trackLeakPerTest;

  static Mso::Future<std::vector<int>> CreateVectorFuture() noexcept {
    Mso::Promise<std::vector<int>> p1;
    std::vector<int> vec{1, 2, 3};
    p1.SetValue(vec);
    return p1.AsFuture();
  }

  static Mso::Future<void> CreateVoidFuture() noexcept {
    Mso::Promise<void> p1;
    p1.SetValue();
    return p1.AsFuture();
  }

  static Mso::Future<std::vector<int>> CreateErrorVectorFuture() noexcept {
    Mso::Promise<std::vector<int>> p1;
    p1.SetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
    return p1.AsFuture();
  }

  static Mso::Future<int> CreateErrorIntFuture() noexcept {
    Mso::Promise<int> p1;
    p1.SetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
    return p1.AsFuture();
  }

  static Mso::Future<void> CreateErrorVoidFuture() noexcept {
    Mso::Promise<void> p1;
    p1.SetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
    return p1.AsFuture();
  }

  static Mso::Future<std::vector<int>> CreateVectorFutureFuture() noexcept {
    Mso::Promise<void> p1;
    p1.SetValue();
    return p1.AsFuture().Then([]() noexcept {
      Mso::Promise<std::vector<int>> p2;
      p2.SetValue(std::vector<int>{1, 2, 3});
      return p2.AsFuture();
    });
  }

  static Mso::Future<void> CreateVoidFutureFuture() noexcept {
    Mso::Promise<void> p1;
    p1.SetValue();
    return p1.AsFuture().Then([]() noexcept {
      Mso::Promise<void> p2;
      p2.SetValue();
      return p2.AsFuture();
    });
  }

  static Mso::Future<std::vector<int>> CreateErrorVectorFutureFuture() noexcept {
    Mso::Promise<void> p1;
    p1.SetValue();
    return p1.AsFuture().Then([]() noexcept {
      Mso::Promise<std::vector<int>> p2;
      p2.SetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
      return p2.AsFuture();
    });
  }

  static Mso::Future<void> CreateErrorVoidFutureFuture() noexcept {
    Mso::Promise<void> p1;
    p1.SetValue();
    return p1.AsFuture().Then([]() noexcept {
      Mso::Promise<void> p2;
      p2.SetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
      return p2.AsFuture();
    });
  }

  static Mso::Future<std::vector<int>> CreateFailingVectorFutureFuture() noexcept {
    Mso::Promise<void> p1;
    p1.SetValue();
    return p1.AsFuture().Then(FailingExecutor(), []() noexcept {
      Mso::Promise<std::vector<int>> p2;
      p2.SetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
      return p2.AsFuture();
    });
  }

  static Mso::Future<void> CreateFailingVoidFutureFuture() noexcept {
    Mso::Promise<void> p1;
    p1.SetValue();
    return p1.AsFuture().Then(FailingExecutor(), []() noexcept {
      Mso::Promise<void> p2;
      p2.SetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
      return p2.AsFuture();
    });
  }

  static Mso::Future<std::vector<int>> CreateMaybeVectorFutureFuture() noexcept {
    Mso::Promise<void> p1;
    p1.SetValue();
    return p1.AsFuture().Then([]() noexcept {
      Mso::Promise<std::vector<int>> p2;
      p2.SetValue(std::vector<int>{1, 2, 3});
      return Mso::Maybe<Mso::Future<std::vector<int>>>(p2.AsFuture());
    });
  }

  static Mso::Future<void> CreateMaybeVoidFutureFuture() noexcept {
    Mso::Promise<void> p1;
    p1.SetValue();
    return p1.AsFuture().Then([]() noexcept {
      Mso::Promise<void> p2;
      p2.SetValue();
      return Mso::Maybe<Mso::Future<void>>(p2.AsFuture());
    });
  }

  static Mso::Future<std::vector<int>> CreateMaybeErrorVectorFutureFuture() noexcept {
    Mso::Promise<void> p1;
    p1.SetValue();
    return p1.AsFuture().Then([]() noexcept {
      Mso::Promise<std::vector<int>> p2;
      p2.SetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
      return Mso::Maybe<Mso::Future<std::vector<int>>>(p2.AsFuture());
    });
  }

  static Mso::Future<void> CreateMaybeErrorVoidFutureFuture() noexcept {
    Mso::Promise<void> p1;
    p1.SetValue();
    return p1.AsFuture().Then([]() noexcept {
      Mso::Promise<void> p2;
      p2.SetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
      return Mso::Maybe<Mso::Future<void>>(p2.AsFuture());
    });
  }

  static Mso::Future<std::vector<int>> CreateErrorMaybeVectorFutureFuture() noexcept {
    Mso::Promise<void> p1;
    p1.SetValue();
    return p1.AsFuture().Then([]() noexcept {
      return Mso::Maybe<Mso::Future<std::vector<int>>>(Mso::CancellationErrorProvider().MakeErrorCode(true));
    });
  }

  static Mso::Future<void> CreateErrorMaybeVoidFutureFuture() noexcept {
    Mso::Promise<void> p1;
    p1.SetValue();
    return p1.AsFuture().Then(
        []() noexcept { return Mso::Maybe<Mso::Future<void>>(Mso::CancellationErrorProvider().MakeErrorCode(true)); });
  }

  static Mso::Future<std::vector<int>> CreateMaybeFailingVectorFutureFuture() noexcept {
    Mso::Promise<void> p1;
    p1.SetValue();
    return p1.AsFuture().Then(FailingExecutor(), []() noexcept {
      Mso::Promise<std::vector<int>> p2;
      p2.SetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
      return Mso::Maybe<Mso::Future<std::vector<int>>>(p2.AsFuture());
    });
  }

  static Mso::Future<void> CreateMaybeFailingVoidFutureFuture() noexcept {
    Mso::Promise<void> p1;
    p1.SetValue();
    return p1.AsFuture().Then(FailingExecutor(), []() noexcept {
      Mso::Promise<void> p2;
      p2.SetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
      return Mso::Maybe<Mso::Future<void>>(p2.AsFuture());
    });
  }

  TEST_METHOD(Future_Then_RValue) {
    bool invoked{false};
    auto future = CreateVectorFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &&value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_ConstRef) {
    bool invoked{false};
    auto future = CreateVectorFuture().Then(Mso::Executors::Inline{}, [&](const std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Ref) {
    bool invoked{false};
    auto future = CreateVectorFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
      value[2] = 5; // to avoid OACR error about non-const parameter.
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Maybe) {
    bool invoked{false};
    auto future =
        CreateVectorFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
          invoked = true;
          TestCheck(value.IsValue());
          TestCheckEqual(3u, value.GetValue().size());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_MaybeRef) {
    bool invoked{false};
    auto future =
        CreateVectorFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &value) noexcept {
          invoked = true;
          TestCheck(value.IsValue());
          TestCheckEqual(3u, value.TakeValue().size());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Void) {
    bool invoked{false};
    auto future = CreateVoidFuture().Then(Mso::Executors::Inline{}, [&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_MaybeVoid) {
    bool invoked{false};
    auto future = CreateVoidFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_MaybeVoidRef) {
    bool invoked{false};
    auto future = CreateVoidFuture().Then(Mso::Executors::Inline{}, [&](const Mso::Maybe<void> &value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Error_Then_RValue) {
    bool invoked{false};
    auto future = CreateErrorVectorFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &&value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Error_Then_ConstRef) {
    bool invoked{false};
    auto future = CreateErrorVectorFuture().Then(Mso::Executors::Inline{}, [&](const std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Error_Then_Ref) {
    bool invoked{false};
    auto future = CreateErrorVectorFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
      value[2] = 5; // to avoid OACR error about non-const parameter.
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Error_Then_Maybe) {
    bool invoked{false};
    auto future =
        CreateErrorVectorFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Error_Then_MaybeRef) {
    bool invoked{false};
    auto future =
        CreateErrorVectorFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Error_Then_Void) {
    bool invoked{false};
    auto future = CreateErrorVoidFuture().Then(Mso::Executors::Inline{}, [&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Error_Then_MaybeVoid) {
    bool invoked{false};
    auto future = CreateErrorVoidFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Error_Then_MaybeVoidRef) {
    bool invoked{false};
    auto future = CreateErrorVoidFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_RValue) {
    bool invoked{false};
    auto future = CreateVectorFuture().Then([&](std::vector<int> &&value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_ConstRef) {
    bool invoked{false};
    auto future = CreateVectorFuture().Then([&](const std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Ref) {
    bool invoked{false};
    auto future = CreateVectorFuture().Then([&](std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
      value[2] = 5; // to avoid OACR error about non-const parameter.
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Maybe) {
    bool invoked{false};
    auto future = CreateVectorFuture().Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
      TestCheckEqual(3u, value.GetValue().size());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_MaybeRef) {
    bool invoked{false};
    auto future = CreateVectorFuture().Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
      TestCheckEqual(3u, value.TakeValue().size());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Void) {
    bool invoked{false};
    auto future = CreateVoidFuture().Then([&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_MaybeVoid) {
    bool invoked{false};
    auto future = CreateVoidFuture().Then([&](Mso::Maybe<void> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_MaybeVoidRef) {
    bool invoked{false};
    auto future = CreateVoidFuture().Then([&](const Mso::Maybe<void> &value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Error_ThenDefault_RValue) {
    bool invoked{false};
    auto future = CreateErrorVectorFuture().Then([&](std::vector<int> &&value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Error_ThenDefault_ConstRef) {
    bool invoked{false};
    auto future = CreateErrorVectorFuture().Then([&](const std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Error_ThenDefault_Ref) {
    bool invoked{false};
    auto future = CreateErrorVectorFuture().Then([&](std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
      value[2] = 5; // to avoid OACR error about non-const parameter.
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Error_ThenDefault_Maybe) {
    bool invoked{false};
    auto future = CreateErrorVectorFuture().Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Error_ThenDefault_MaybeRef) {
    bool invoked{false};
    auto future = CreateErrorVectorFuture().Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Error_ThenDefault_Void) {
    bool invoked{false};
    auto future = CreateErrorVoidFuture().Then([&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Error_ThenDefault_MaybeVoid) {
    bool invoked{false};
    auto future = CreateErrorVoidFuture().Then([&](Mso::Maybe<void> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Error_ThenDefault_MaybeVoidRef) {
    bool invoked{false};
    auto future = CreateErrorVoidFuture().Then([&](Mso::Maybe<void> &value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch) {
    bool invoked{false};
    auto future = CreateVectorFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
      return std::vector<int>();
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Catch_Void) {
    bool invoked{false};
    auto future = CreateVoidFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Error_Catch) {
    bool invoked{false};
    auto future = CreateErrorVectorFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
      return std::vector<int>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Error_Catch_Void) {
    bool invoked{false};
    auto future = CreateErrorVoidFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault) {
    bool invoked{false};
    auto future = CreateVectorFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
      return std::vector<int>();
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_CatchDefault_Void) {
    bool invoked{false};
    auto future = CreateVoidFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Error_CatchDefault) {
    bool invoked{false};
    auto future = CreateErrorVectorFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
      return std::vector<int>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Error_CatchDefault_Void) {
    bool invoked{false};
    auto future = CreateErrorVoidFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Catch_Then_RValue) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](std::vector<int> &&value) noexcept {
                            isInvoked[0] = true;
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return std::vector<int>();
                          })
                      .Then(Mso::Executors::Inline{}, [&](std::vector<int> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(3u, value.size());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(!isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Then_Catch_Then_ConstRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](const std::vector<int> &value) noexcept {
                            isInvoked[0] = true;
                            return value;
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return std::vector<int>();
                          })
                      .Then(Mso::Executors::Inline{}, [&](const std::vector<int> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(3u, value.size());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(!isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Then_Catch_Then_Ref) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](std::vector<int> &value) noexcept {
                            isInvoked[0] = true;
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return std::vector<int>();
                          })
                      .Then(Mso::Executors::Inline{}, [&](std::vector<int> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(3u, value.size());
                        value[2] = 5; // to avoid OACR error about non-const parameter.
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(!isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Then_Catch_Then_Maybe) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                            isInvoked[0] = true;
                            TestCheck(value.IsValue());
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return std::vector<int>();
                          })
                      .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                        TestCheckEqual(3u, value.GetValue().size());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(!isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Then_Catch_Then_MaybeRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](Mso::Maybe<std::vector<int>> &value) noexcept {
                            isInvoked[0] = true;
                            TestCheck(value.IsValue());
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return std::vector<int>();
                          })
                      .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                        TestCheckEqual(3u, value.TakeValue().size());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(!isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Then_Catch_Then_Void) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future =
        CreateVoidFuture()
            .Then(Mso::Executors::Inline{}, [&]() noexcept { isInvoked[0] = true; })
            .Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode & /*error*/) noexcept { isInvoked[1] = true; })
            .Then(Mso::Executors::Inline{}, [&]() noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(!isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Then_Catch_Then_MaybeVoid) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future =
        CreateVoidFuture()
            .Then(
                Mso::Executors::Inline{},
                [&](Mso::Maybe<void> &&value) noexcept {
                  isInvoked[0] = true;
                  TestCheck(value.IsValue());
                  return std::move(value);
                })
            .Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode & /*error*/) noexcept { isInvoked[1] = true; })
            .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &&value) noexcept {
              isInvoked[2] = true;
              TestCheck(value.IsValue());
            });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(!isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Then_Catch_Then_MaybeVoidRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future =
        CreateVoidFuture()
            .Then(
                Mso::Executors::Inline{},
                [&](Mso::Maybe<void> &value) noexcept {
                  isInvoked[0] = true;
                  TestCheck(value.IsValue());
                  return std::move(value);
                })
            .Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode & /*error*/) noexcept { isInvoked[1] = true; })
            .Then(Mso::Executors::Inline{}, [&](const Mso::Maybe<void> &value) noexcept {
              isInvoked[2] = true;
              TestCheck(value.IsValue());
            });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(!isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Then_RValue) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](std::vector<int> &&value) noexcept {
                            isInvoked[0] = true;
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return std::vector<int>{1, 2};
                          })
                      .Then(Mso::Executors::Inline{}, [&](std::vector<int> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(2u, value.size());
                      });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Then_ConstRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](const std::vector<int> &value) noexcept {
                            isInvoked[0] = true;
                            return value;
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return std::vector<int>{1, 2};
                          })
                      .Then(Mso::Executors::Inline{}, [&](const std::vector<int> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(2u, value.size());
                      });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Then_Ref) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](std::vector<int> &value) noexcept {
                            isInvoked[0] = true;
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return std::vector<int>{1, 2};
                          })
                      .Then(Mso::Executors::Inline{}, [&](std::vector<int> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(2u, value.size());
                        value[1] = 5; // to avoid OACR error about non-const parameter.
                      });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Then_Maybe) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                            isInvoked[0] = true;
                            TestCheck(value.IsError());
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return std::vector<int>{1, 2};
                          })
                      .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                        TestCheckEqual(2u, value.GetValue().size());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Then_MaybeRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](Mso::Maybe<std::vector<int>> &value) noexcept {
                            isInvoked[0] = true;
                            TestCheck(value.IsError());
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return std::vector<int>{1, 2};
                          })
                      .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                        TestCheckEqual(2u, value.TakeValue().size());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Then_Void) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future =
        CreateErrorVoidFuture()
            .Then(Mso::Executors::Inline{}, [&]() noexcept { isInvoked[0] = true; })
            .Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode & /*error*/) noexcept { isInvoked[1] = true; })
            .Then(Mso::Executors::Inline{}, [&]() noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Then_MaybeVoid) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future =
        CreateErrorVoidFuture()
            .Then(
                Mso::Executors::Inline{},
                [&](Mso::Maybe<void> &&value) noexcept {
                  isInvoked[0] = true;
                  TestCheck(value.IsError());
                  return std::move(value);
                })
            .Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode & /*error*/) noexcept { isInvoked[1] = true; })
            .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &&value) noexcept {
              isInvoked[2] = true;
              TestCheck(value.IsValue());
            });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Then_MaybeVoidRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future =
        CreateErrorVoidFuture()
            .Then(
                Mso::Executors::Inline{},
                [&](Mso::Maybe<void> &value) noexcept {
                  isInvoked[0] = true;
                  TestCheck(value.IsError());
                  return std::move(value);
                })
            .Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode & /*error*/) noexcept { isInvoked[1] = true; })
            .Then(Mso::Executors::Inline{}, [&](const Mso::Maybe<void> &value) noexcept {
              isInvoked[2] = true;
              TestCheck(value.IsValue());
            });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Error_Then_RValue) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future =
        CreateErrorVectorFuture()
            .Then(
                Mso::Executors::Inline{},
                [&](std::vector<int> &&value) noexcept {
                  isInvoked[0] = true;
                  return std::move(value);
                })
            .Catch(
                Mso::Executors::Inline{},
                [&](const Mso::ErrorCode &error) noexcept {
                  isInvoked[1] = true;
                  return error;
                })
            .Then(Mso::Executors::Inline{}, [&](std::vector<int> && /*value*/) noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(!isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Error_Then_ConstRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future =
        CreateErrorVectorFuture()
            .Then(
                Mso::Executors::Inline{},
                [&](const std::vector<int> &value) noexcept {
                  isInvoked[0] = true;
                  return value;
                })
            .Catch(
                Mso::Executors::Inline{},
                [&](const Mso::ErrorCode &error) noexcept {
                  isInvoked[1] = true;
                  return error;
                })
            .Then(Mso::Executors::Inline{}, [&](const std::vector<int> & /*value*/) noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(!isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Error_Then_Ref) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](std::vector<int> &value) noexcept {
                            isInvoked[0] = true;
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode &error) noexcept {
                            isInvoked[1] = true;
                            return error;
                          })
                      .Then(Mso::Executors::Inline{}, [&](std::vector<int> &value) noexcept {
                        isInvoked[2] = true;
                        value[1] = 5; // to avoid OACR error about non-const parameter.
                      });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(!isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Error_Then_Maybe) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                            isInvoked[0] = true;
                            TestCheck(value.IsError());
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode &error) noexcept {
                            isInvoked[1] = true;
                            return error;
                          })
                      .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsError());
                        TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Error_Then_MaybeRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](Mso::Maybe<std::vector<int>> &value) noexcept {
                            isInvoked[0] = true;
                            TestCheck(value.IsError());
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](Mso::ErrorCode &&error) noexcept {
                            isInvoked[1] = true;
                            return std::move(error);
                          })
                      .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsError());
                        TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Error_Then_Void) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then(Mso::Executors::Inline{}, [&]() noexcept { isInvoked[0] = true; })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode &error) noexcept {
                            isInvoked[1] = true;
                            return error;
                          })
                      .Then(Mso::Executors::Inline{}, [&]() noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(!isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Error_Then_MaybeVoid) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](Mso::Maybe<void> &&value) noexcept {
                            isInvoked[0] = true;
                            TestCheck(value.IsError());
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode &error) noexcept {
                            isInvoked[1] = true;
                            return error;
                          })
                      .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsError());
                        TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Error_Then_MaybeVoidRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](Mso::Maybe<void> &value) noexcept {
                            isInvoked[0] = true;
                            TestCheck(value.IsError());
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode &error) noexcept {
                            isInvoked[1] = true;
                            return error;
                          })
                      .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsError());
                        TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Maybe_Then_RValue) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](std::vector<int> &&value) noexcept {
                            isInvoked[0] = true;
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return Mso::Maybe<std::vector<int>>(std::vector<int>{1, 2});
                          })
                      .Then(Mso::Executors::Inline{}, [&](std::vector<int> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(2u, value.size());
                      });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Maybe_Then_ConstRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](const std::vector<int> &value) noexcept {
                            isInvoked[0] = true;
                            return value;
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return Mso::Maybe<std::vector<int>>(std::vector<int>{1, 2});
                          })
                      .Then(Mso::Executors::Inline{}, [&](const std::vector<int> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(2u, value.size());
                      });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Maybe_Then_Ref) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](std::vector<int> &value) noexcept {
                            isInvoked[0] = true;
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return Mso::Maybe<std::vector<int>>(std::vector<int>{1, 2});
                          })
                      .Then(Mso::Executors::Inline{}, [&](std::vector<int> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(2u, value.size());
                        value[1] = 5; // to avoid OACR error about non-const parameter.
                      });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Maybe_Then_Maybe) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                            isInvoked[0] = true;
                            TestCheck(value.IsError());
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return Mso::Maybe<std::vector<int>>(std::vector<int>{1, 2});
                          })
                      .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                        TestCheckEqual(2u, value.GetValue().size());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Maybe_Then_MaybeRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](Mso::Maybe<std::vector<int>> &value) noexcept {
                            isInvoked[0] = true;
                            TestCheck(value.IsError());
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return Mso::Maybe<std::vector<int>>(std::vector<int>{1, 2});
                          })
                      .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                        TestCheckEqual(2u, value.TakeValue().size());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Maybe_Then_Void) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then(Mso::Executors::Inline{}, [&]() noexcept { isInvoked[0] = true; })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return Mso::Maybe<void>();
                          })
                      .Then(Mso::Executors::Inline{}, [&]() noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Maybe_Then_MaybeVoid) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](Mso::Maybe<void> &&value) noexcept {
                            isInvoked[0] = true;
                            TestCheck(value.IsError());
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return Mso::Maybe<void>();
                          })
                      .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_Maybe_Then_MaybeVoidRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](Mso::Maybe<void> &value) noexcept {
                            isInvoked[0] = true;
                            TestCheck(value.IsError());
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode & /*error*/) noexcept {
                            isInvoked[1] = true;
                            return Mso::Maybe<void>();
                          })
                      .Then(Mso::Executors::Inline{}, [&](const Mso::Maybe<void> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_MaybeError_Then_RValue) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future =
        CreateErrorVectorFuture()
            .Then(
                Mso::Executors::Inline{},
                [&](std::vector<int> &&value) noexcept {
                  isInvoked[0] = true;
                  return std::move(value);
                })
            .Catch(
                Mso::Executors::Inline{},
                [&](const Mso::ErrorCode &error) noexcept {
                  isInvoked[1] = true;
                  return Mso::Maybe<std::vector<int>>(error);
                })
            .Then(Mso::Executors::Inline{}, [&](std::vector<int> && /*value*/) noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(!isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_MaybeError_Then_ConstRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future =
        CreateErrorVectorFuture()
            .Then(
                Mso::Executors::Inline{},
                [&](const std::vector<int> &value) noexcept {
                  isInvoked[0] = true;
                  return value;
                })
            .Catch(
                Mso::Executors::Inline{},
                [&](const Mso::ErrorCode &error) noexcept {
                  isInvoked[1] = true;
                  return Mso::Maybe<std::vector<int>>(error);
                })
            .Then(Mso::Executors::Inline{}, [&](const std::vector<int> & /*value*/) noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(!isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_MaybeError_Then_Ref) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](std::vector<int> &value) noexcept {
                            isInvoked[0] = true;
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode &error) noexcept {
                            isInvoked[1] = true;
                            return Mso::Maybe<std::vector<int>>(error);
                          })
                      .Then(Mso::Executors::Inline{}, [&](std::vector<int> &value) noexcept {
                        isInvoked[2] = true;
                        value[1] = 5; // to avoid OACR error about non-const parameter.
                      });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(!isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_MaybeError_Then_Maybe) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                            isInvoked[0] = true;
                            TestCheck(value.IsError());
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode &error) noexcept {
                            isInvoked[1] = true;
                            return Mso::Maybe<std::vector<int>>(error);
                          })
                      .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsError());
                        TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_MaybeError_Then_MaybeRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](Mso::Maybe<std::vector<int>> &value) noexcept {
                            isInvoked[0] = true;
                            TestCheck(value.IsError());
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode &error) noexcept {
                            isInvoked[1] = true;
                            return Mso::Maybe<std::vector<int>>(error);
                          })
                      .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsError());
                        TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_MaybeError_Then_Void) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then(Mso::Executors::Inline{}, [&]() noexcept { isInvoked[0] = true; })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode &error) noexcept {
                            isInvoked[1] = true;
                            return Mso::Maybe<void>(error);
                          })
                      .Then(Mso::Executors::Inline{}, [&]() noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(!isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_MaybeError_Then_MaybeVoid) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](Mso::Maybe<void> &&value) noexcept {
                            isInvoked[0] = true;
                            TestCheck(value.IsError());
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode &error) noexcept {
                            isInvoked[1] = true;
                            return Mso::Maybe<void>(error);
                          })
                      .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsError());
                        TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_Then_Catch_MaybeError_Then_MaybeVoidRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then(
                          Mso::Executors::Inline{},
                          [&](Mso::Maybe<void> &value) noexcept {
                            isInvoked[0] = true;
                            TestCheck(value.IsError());
                            return std::move(value);
                          })
                      .Catch(
                          Mso::Executors::Inline{},
                          [&](const Mso::ErrorCode &error) noexcept {
                            isInvoked[1] = true;
                            return Mso::Maybe<void>(error);
                          })
                      .Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsError());
                        TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_ThenDefault_Catch_Then_RValue) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateVectorFuture()
                      .Then([&](std::vector<int> &&value) noexcept {
                        isInvoked[0] = true;
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return std::vector<int>();
                      })
                      .Then([&](std::vector<int> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(3u, value.size());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(!isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_ThenDefault_Catch_Then_ConstRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateVectorFuture()
                      .Then([&](const std::vector<int> &value) noexcept {
                        isInvoked[0] = true;
                        return value;
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return std::vector<int>();
                      })
                      .Then([&](const std::vector<int> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(3u, value.size());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(!isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_ThenDefault_Catch_Then_Ref) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateVectorFuture()
                      .Then([&](std::vector<int> &value) noexcept {
                        isInvoked[0] = true;
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return std::vector<int>();
                      })
                      .Then([&](std::vector<int> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(3u, value.size());
                        value[2] = 5; // to avoid OACR error about non-const parameter.
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(!isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_ThenDefault_Catch_Then_Maybe) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateVectorFuture()
                      .Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsValue());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return std::vector<int>();
                      })
                      .Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                        TestCheckEqual(3u, value.GetValue().size());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(!isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_ThenDefault_Catch_Then_MaybeRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateVectorFuture()
                      .Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsValue());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return std::vector<int>();
                      })
                      .Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                        TestCheckEqual(3u, value.TakeValue().size());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(!isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_ThenDefault_Catch_Then_Void) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateVoidFuture()
                      .Then([&]() noexcept { isInvoked[0] = true; })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept { isInvoked[1] = true; })
                      .Then([&]() noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(!isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_ThenDefault_Catch_Then_MaybeVoid) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateVoidFuture()
                      .Then([&](Mso::Maybe<void> &&value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsValue());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept { isInvoked[1] = true; })
                      .Then([&](Mso::Maybe<void> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(!isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_ThenDefault_Catch_Then_MaybeVoidRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateVoidFuture()
                      .Then([&](Mso::Maybe<void> &value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsValue());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept { isInvoked[1] = true; })
                      .Then([&](const Mso::Maybe<void> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(!isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Then_RValue) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](std::vector<int> &&value) noexcept {
                        isInvoked[0] = true;
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return std::vector<int>{1, 2};
                      })
                      .Then([&](std::vector<int> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(2u, value.size());
                      });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Then_ConstRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](const std::vector<int> &value) noexcept {
                        isInvoked[0] = true;
                        return value;
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return std::vector<int>{1, 2};
                      })
                      .Then([&](const std::vector<int> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(2u, value.size());
                      });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Then_Ref) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](std::vector<int> &value) noexcept {
                        isInvoked[0] = true;
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return std::vector<int>{1, 2};
                      })
                      .Then([&](std::vector<int> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(2u, value.size());
                        value[1] = 5; // to avoid OACR error about non-const parameter.
                      });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Then_Maybe) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsError());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return std::vector<int>{1, 2};
                      })
                      .Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                        TestCheckEqual(2u, value.GetValue().size());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Then_MaybeRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsError());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return std::vector<int>{1, 2};
                      })
                      .Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                        TestCheckEqual(2u, value.TakeValue().size());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Then_Void) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then([&]() noexcept { isInvoked[0] = true; })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept { isInvoked[1] = true; })
                      .Then([&]() noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Then_MaybeVoid) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then([&](Mso::Maybe<void> &&value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsError());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept { isInvoked[1] = true; })
                      .Then([&](Mso::Maybe<void> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Then_MaybeVoidRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then([&](Mso::Maybe<void> &value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsError());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept { isInvoked[1] = true; })
                      .Then([&](const Mso::Maybe<void> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Error_Then_RValue) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](std::vector<int> &&value) noexcept {
                        isInvoked[0] = true;
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode &error) noexcept {
                        isInvoked[1] = true;
                        return error;
                      })
                      .Then([&](std::vector<int> && /*value*/) noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(!isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Error_Then_ConstRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](const std::vector<int> &value) noexcept {
                        isInvoked[0] = true;
                        return value;
                      })
                      .Catch([&](const Mso::ErrorCode &error) noexcept {
                        isInvoked[1] = true;
                        return error;
                      })
                      .Then([&](const std::vector<int> & /*value*/) noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(!isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Error_Then_Ref) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](std::vector<int> &value) noexcept {
                        isInvoked[0] = true;
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode &error) noexcept {
                        isInvoked[1] = true;
                        return error;
                      })
                      .Then([&](std::vector<int> &value) noexcept {
                        isInvoked[2] = true;
                        value[1] = 5; // to avoid OACR error about non-const parameter.
                      });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(!isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Error_Then_Maybe) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsError());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode &error) noexcept {
                        isInvoked[1] = true;
                        return error;
                      })
                      .Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsError());
                        TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Error_Then_MaybeRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsError());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode &error) noexcept {
                        isInvoked[1] = true;
                        return error;
                      })
                      .Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsError());
                        TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Error_Then_Void) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then([&]() noexcept { isInvoked[0] = true; })
                      .Catch([&](const Mso::ErrorCode &error) noexcept {
                        isInvoked[1] = true;
                        return error;
                      })
                      .Then([&]() noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(!isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Error_Then_MaybeVoid) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then([&](Mso::Maybe<void> &&value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsError());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode &error) noexcept {
                        isInvoked[1] = true;
                        return error;
                      })
                      .Then([&](Mso::Maybe<void> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsError());
                        TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Error_Then_MaybeVoidRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then([&](Mso::Maybe<void> &value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsError());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode &error) noexcept {
                        isInvoked[1] = true;
                        return error;
                      })
                      .Then([&](const Mso::Maybe<void> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsError());
                        TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Maybe_Then_RValue) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](std::vector<int> &&value) noexcept {
                        isInvoked[0] = true;
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return Mso::Maybe<std::vector<int>>(std::vector<int>{1, 2});
                      })
                      .Then([&](std::vector<int> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(2u, value.size());
                      });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Maybe_Then_ConstRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](const std::vector<int> &value) noexcept {
                        isInvoked[0] = true;
                        return value;
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return Mso::Maybe<std::vector<int>>(std::vector<int>{1, 2});
                      })
                      .Then([&](const std::vector<int> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(2u, value.size());
                      });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Maybe_Then_Ref) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](std::vector<int> &value) noexcept {
                        isInvoked[0] = true;
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return Mso::Maybe<std::vector<int>>(std::vector<int>{1, 2});
                      })
                      .Then([&](std::vector<int> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheckEqual(2u, value.size());
                        value[1] = 5; // to avoid OACR error about non-const parameter.
                      });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Maybe_Then_Maybe) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsError());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return Mso::Maybe<std::vector<int>>(std::vector<int>{1, 2});
                      })
                      .Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                        TestCheckEqual(2u, value.GetValue().size());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Maybe_Then_MaybeRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsError());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return Mso::Maybe<std::vector<int>>(std::vector<int>{1, 2});
                      })
                      .Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                        TestCheckEqual(2u, value.TakeValue().size());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Maybe_Then_Void) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then([&]() noexcept { isInvoked[0] = true; })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return Mso::Maybe<void>();
                      })
                      .Then([&]() noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Maybe_Then_MaybeVoid) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then([&](Mso::Maybe<void> &&value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsError());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return Mso::Maybe<void>();
                      })
                      .Then([&](Mso::Maybe<void> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_Maybe_Then_MaybeVoidRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then([&](Mso::Maybe<void> &value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsError());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
                        isInvoked[1] = true;
                        return Mso::Maybe<void>();
                      })
                      .Then([&](const Mso::Maybe<void> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsValue());
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_MaybeError_Then_RValue) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](std::vector<int> &&value) noexcept {
                        isInvoked[0] = true;
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode &error) noexcept {
                        isInvoked[1] = true;
                        return Mso::Maybe<std::vector<int>>(error);
                      })
                      .Then([&](std::vector<int> && /*value*/) noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(!isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_MaybeError_Then_ConstRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](const std::vector<int> &value) noexcept {
                        isInvoked[0] = true;
                        return value;
                      })
                      .Catch([&](const Mso::ErrorCode &error) noexcept {
                        isInvoked[1] = true;
                        return Mso::Maybe<std::vector<int>>(error);
                      })
                      .Then([&](const std::vector<int> & /*value*/) noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(!isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_MaybeError_Then_Ref) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](std::vector<int> &value) noexcept {
                        isInvoked[0] = true;
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode &error) noexcept {
                        isInvoked[1] = true;
                        return Mso::Maybe<std::vector<int>>(error);
                      })
                      .Then([&](std::vector<int> &value) noexcept {
                        isInvoked[2] = true;
                        value[1] = 5; // to avoid OACR error about non-const parameter.
                      });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(!isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_MaybeError_Then_Maybe) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsError());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode &error) noexcept {
                        isInvoked[1] = true;
                        return Mso::Maybe<std::vector<int>>(error);
                      })
                      .Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsError());
                        TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_MaybeError_Then_MaybeRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVectorFuture()
                      .Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsError());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode &error) noexcept {
                        isInvoked[1] = true;
                        return Mso::Maybe<std::vector<int>>(error);
                      })
                      .Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsError());
                        TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_MaybeError_Then_Void) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then([&]() noexcept { isInvoked[0] = true; })
                      .Catch([&](const Mso::ErrorCode &error) noexcept {
                        isInvoked[1] = true;
                        return Mso::Maybe<void>(error);
                      })
                      .Then([&]() noexcept { isInvoked[2] = true; });

    Mso::FutureWait(future);
    TestCheck(!isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(!isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_MaybeError_Then_MaybeVoid) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then([&](Mso::Maybe<void> &&value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsError());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode &error) noexcept {
                        isInvoked[1] = true;
                        return Mso::Maybe<void>(error);
                      })
                      .Then([&](Mso::Maybe<void> &&value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsError());
                        TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Error_ThenDefault_Catch_MaybeError_Then_MaybeVoidRef) {
    // In successful case Catch is not invoked
    bool isInvoked[] = {false, false, false};

    auto future = CreateErrorVoidFuture()
                      .Then([&](Mso::Maybe<void> &value) noexcept {
                        isInvoked[0] = true;
                        TestCheck(value.IsError());
                        return std::move(value);
                      })
                      .Catch([&](const Mso::ErrorCode &error) noexcept {
                        isInvoked[1] = true;
                        return Mso::Maybe<void>(error);
                      })
                      .Then([&](Mso::Maybe<void> &value) noexcept {
                        isInvoked[2] = true;
                        TestCheck(value.IsError());
                        TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
                      });

    Mso::FutureWait(future);
    TestCheck(isInvoked[0]);
    TestCheck(isInvoked[1]);
    TestCheck(isInvoked[2]);
  }

  TEST_METHOD(Future_Future_Then_RValue) {
    bool invoked{false};
    auto future = CreateVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &&value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Then_ConstRef) {
    bool invoked{false};
    auto future =
        CreateVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](const std::vector<int> &value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Then_Ref) {
    bool invoked{false};
    auto future = CreateVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
      value[2] = 5; // to avoid OACR error about non-const parameter.
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Then_Maybe) {
    bool invoked{false};
    auto future =
        CreateVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
          invoked = true;
          TestCheck(value.IsValue());
          TestCheckEqual(3u, value.GetValue().size());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Then_MaybeRef) {
    bool invoked{false};
    auto future =
        CreateVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &value) noexcept {
          invoked = true;
          TestCheck(value.IsValue());
          TestCheckEqual(3u, value.TakeValue().size());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Then_Void) {
    bool invoked{false};
    auto future = CreateVoidFutureFuture().Then(Mso::Executors::Inline{}, [&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Then_MaybeVoid) {
    bool invoked{false};
    auto future = CreateVoidFutureFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Then_MaybeVoidRef) {
    bool invoked{false};
    auto future = CreateVoidFutureFuture().Then(Mso::Executors::Inline{}, [&](const Mso::Maybe<void> &value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Error_Then_RValue) {
    bool invoked{false};
    auto future =
        CreateErrorVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &&value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
        });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Error_Then_ConstRef) {
    bool invoked{false};
    auto future =
        CreateErrorVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](const std::vector<int> &value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
        });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Error_Then_Ref) {
    bool invoked{false};
    auto future = CreateErrorVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
      value[2] = 5; // to avoid OACR error about non-const parameter.
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Error_Then_Maybe) {
    bool invoked{false};
    auto future = CreateErrorVectorFutureFuture().Then(
        Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Error_Then_MaybeRef) {
    bool invoked{false};
    auto future = CreateErrorVectorFutureFuture().Then(
        Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Error_Then_Void) {
    bool invoked{false};
    auto future = CreateErrorVoidFutureFuture().Then(Mso::Executors::Inline{}, [&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Error_Then_MaybeVoid) {
    bool invoked{false};
    auto future = CreateErrorVoidFutureFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Error_Then_MaybeVoidRef) {
    bool invoked{false};
    auto future = CreateErrorVoidFutureFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Failing_Then_RValue) {
    bool invoked{false};
    auto future =
        CreateFailingVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &&value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
        });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Failing_Then_ConstRef) {
    bool invoked{false};
    auto future =
        CreateFailingVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](const std::vector<int> &value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
        });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Failing_Then_Ref) {
    bool invoked{false};
    auto future =
        CreateFailingVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
          value[2] = 5; // to avoid OACR error about non-const parameter.
        });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Failing_Then_Maybe) {
    bool invoked{false};
    auto future = CreateFailingVectorFutureFuture().Then(
        Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Failing_Then_MaybeRef) {
    bool invoked{false};
    auto future = CreateFailingVectorFutureFuture().Then(
        Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Failing_Then_Void) {
    bool invoked{false};
    auto future = CreateFailingVoidFutureFuture().Then(Mso::Executors::Inline{}, [&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Failing_Then_MaybeVoid) {
    bool invoked{false};
    auto future =
        CreateFailingVoidFutureFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &&value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Failing_Then_MaybeVoidRef) {
    bool invoked{false};
    auto future =
        CreateFailingVoidFutureFuture().Then(Mso::Executors::Inline{}, [&](const Mso::Maybe<void> &value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Then_RValue) {
    bool invoked{false};
    auto future =
        CreateMaybeVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &&value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Then_ConstRef) {
    bool invoked{false};
    auto future =
        CreateMaybeVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](const std::vector<int> &value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Then_Ref) {
    bool invoked{false};
    auto future = CreateMaybeVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
      value[2] = 5; // to avoid OACR error about non-const parameter.
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Then_Maybe) {
    bool invoked{false};
    auto future = CreateMaybeVectorFutureFuture().Then(
        Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
          invoked = true;
          TestCheck(value.IsValue());
          TestCheckEqual(3u, value.GetValue().size());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Then_MaybeRef) {
    bool invoked{false};
    auto future = CreateMaybeVectorFutureFuture().Then(
        Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &value) noexcept {
          invoked = true;
          TestCheck(value.IsValue());
          TestCheckEqual(3u, value.TakeValue().size());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Then_Void) {
    bool invoked{false};
    auto future = CreateMaybeVoidFutureFuture().Then(Mso::Executors::Inline{}, [&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Then_MaybeVoid) {
    bool invoked{false};
    auto future = CreateMaybeVoidFutureFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Then_MaybeVoidRef) {
    bool invoked{false};
    auto future =
        CreateMaybeVoidFutureFuture().Then(Mso::Executors::Inline{}, [&](const Mso::Maybe<void> &value) noexcept {
          invoked = true;
          TestCheck(value.IsValue());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_Then_RValue) {
    bool invoked{false};
    auto future =
        CreateMaybeErrorVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &&value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
        });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_Then_ConstRef) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Then(
        Mso::Executors::Inline{}, [&](const std::vector<int> &value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
        });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_Then_Ref) {
    bool invoked{false};
    auto future =
        CreateMaybeErrorVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
          value[2] = 5; // to avoid OACR error about non-const parameter.
        });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_Then_Maybe) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Then(
        Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_Then_MaybeRef) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Then(
        Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_Then_Void) {
    bool invoked{false};
    auto future = CreateMaybeErrorVoidFutureFuture().Then(Mso::Executors::Inline{}, [&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_Then_MaybeVoid) {
    bool invoked{false};
    auto future =
        CreateMaybeErrorVoidFutureFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &&value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_Then_MaybeVoidRef) {
    bool invoked{false};
    auto future =
        CreateMaybeErrorVoidFutureFuture().Then(Mso::Executors::Inline{}, [&](const Mso::Maybe<void> &value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_Then_RValue) {
    bool invoked{false};
    auto future =
        CreateMaybeErrorVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &&value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
        });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_Then_ConstRef) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Then(
        Mso::Executors::Inline{}, [&](const std::vector<int> &value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
        });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_Then_Ref) {
    bool invoked{false};
    auto future =
        CreateMaybeErrorVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
          value[2] = 5; // to avoid OACR error about non-const parameter.
        });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_Then_Maybe) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Then(
        Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_Then_MaybeRef) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Then(
        Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_Then_Void) {
    bool invoked{false};
    auto future = CreateMaybeErrorVoidFutureFuture().Then(Mso::Executors::Inline{}, [&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_Then_MaybeVoid) {
    bool invoked{false};
    auto future =
        CreateMaybeErrorVoidFutureFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &&value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_Then_MaybeVoidRef) {
    bool invoked{false};
    auto future =
        CreateMaybeErrorVoidFutureFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_Then_RValue) {
    bool invoked{false};
    auto future =
        CreateMaybeFailingVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &&value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
        });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_Then_ConstRef) {
    bool invoked{false};
    auto future = CreateMaybeFailingVectorFutureFuture().Then(
        Mso::Executors::Inline{}, [&](const std::vector<int> &value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
        });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_Then_Ref) {
    bool invoked{false};
    auto future =
        CreateMaybeFailingVectorFutureFuture().Then(Mso::Executors::Inline{}, [&](std::vector<int> &value) noexcept {
          invoked = true;
          TestCheckEqual(3u, value.size());
          value[2] = 5; // to avoid OACR error about non-const parameter.
        });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_Then_Maybe) {
    bool invoked{false};
    auto future = CreateMaybeFailingVectorFutureFuture().Then(
        Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &&value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_Then_MaybeRef) {
    bool invoked{false};
    auto future = CreateMaybeFailingVectorFutureFuture().Then(
        Mso::Executors::Inline{}, [&](Mso::Maybe<std::vector<int>> &value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_Then_Void) {
    bool invoked{false};
    auto future =
        CreateMaybeFailingVoidFutureFuture().Then(Mso::Executors::Inline{}, [&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_Then_MaybeVoid) {
    bool invoked{false};
    auto future =
        CreateMaybeFailingVoidFutureFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &&value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_Then_MaybeVoidRef) {
    bool invoked{false};
    auto future =
        CreateMaybeFailingVoidFutureFuture().Then(Mso::Executors::Inline{}, [&](Mso::Maybe<void> &value) noexcept {
          invoked = true;
          TestCheck(value.IsError());
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_ThenDefault_RValue) {
    bool invoked{false};
    auto future = CreateVectorFutureFuture().Then([&](std::vector<int> &&value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_ThenDefault_ConstRef) {
    bool invoked{false};
    auto future = CreateVectorFutureFuture().Then([&](const std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_ThenDefault_Ref) {
    bool invoked{false};
    auto future = CreateVectorFutureFuture().Then([&](std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
      value[2] = 5; // to avoid OACR error about non-const parameter.
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_ThenDefault_Maybe) {
    bool invoked{false};
    auto future = CreateVectorFutureFuture().Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
      TestCheckEqual(3u, value.GetValue().size());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_ThenDefault_MaybeRef) {
    bool invoked{false};
    auto future = CreateVectorFutureFuture().Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
      TestCheckEqual(3u, value.TakeValue().size());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_ThenDefault_Void) {
    bool invoked{false};
    auto future = CreateVoidFutureFuture().Then([&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_ThenDefault_MaybeVoid) {
    bool invoked{false};
    auto future = CreateVoidFutureFuture().Then([&](Mso::Maybe<void> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_ThenDefault_MaybeVoidRef) {
    bool invoked{false};
    auto future = CreateVoidFutureFuture().Then([&](const Mso::Maybe<void> &value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Error_ThenDefault_RValue) {
    bool invoked{false};
    auto future = CreateErrorVectorFutureFuture().Then([&](std::vector<int> &&value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Error_ThenDefault_ConstRef) {
    bool invoked{false};
    auto future = CreateErrorVectorFutureFuture().Then([&](const std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Error_ThenDefault_Ref) {
    bool invoked{false};
    auto future = CreateErrorVectorFutureFuture().Then([&](std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
      value[2] = 5; // to avoid OACR error about non-const parameter.
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Error_ThenDefault_Maybe) {
    bool invoked{false};
    auto future = CreateErrorVectorFutureFuture().Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Error_ThenDefault_MaybeRef) {
    bool invoked{false};
    auto future = CreateErrorVectorFutureFuture().Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Error_ThenDefault_Void) {
    bool invoked{false};
    auto future = CreateErrorVoidFutureFuture().Then([&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Error_ThenDefault_MaybeVoid) {
    bool invoked{false};
    auto future = CreateErrorVoidFutureFuture().Then([&](Mso::Maybe<void> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Error_ThenDefault_MaybeVoidRef) {
    bool invoked{false};
    auto future = CreateErrorVoidFutureFuture().Then([&](Mso::Maybe<void> &value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Failing_ThenDefault_RValue) {
    bool invoked{false};
    auto future = CreateFailingVectorFutureFuture().Then([&](std::vector<int> &&value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Failing_ThenDefault_ConstRef) {
    bool invoked{false};
    auto future = CreateFailingVectorFutureFuture().Then([&](const std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Failing_ThenDefault_Ref) {
    bool invoked{false};
    auto future = CreateFailingVectorFutureFuture().Then([&](std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
      value[2] = 5; // to avoid OACR error about non-const parameter.
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Failing_ThenDefault_Maybe) {
    bool invoked{false};
    auto future = CreateFailingVectorFutureFuture().Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Failing_ThenDefault_MaybeRef) {
    bool invoked{false};
    auto future = CreateFailingVectorFutureFuture().Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Failing_ThenDefault_Void) {
    bool invoked{false};
    auto future = CreateFailingVoidFutureFuture().Then([&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Failing_ThenDefault_MaybeVoid) {
    bool invoked{false};
    auto future = CreateFailingVoidFutureFuture().Then([&](Mso::Maybe<void> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Failing_ThenDefault_MaybeVoidRef) {
    bool invoked{false};
    auto future = CreateFailingVoidFutureFuture().Then([&](Mso::Maybe<void> &value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_ThenDefault_RValue) {
    bool invoked{false};
    auto future = CreateMaybeVectorFutureFuture().Then([&](std::vector<int> &&value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_ThenDefault_ConstRef) {
    bool invoked{false};
    auto future = CreateMaybeVectorFutureFuture().Then([&](const std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_ThenDefault_Ref) {
    bool invoked{false};
    auto future = CreateMaybeVectorFutureFuture().Then([&](std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
      value[2] = 5; // to avoid OACR error about non-const parameter.
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_ThenDefault_Maybe) {
    bool invoked{false};
    auto future = CreateMaybeVectorFutureFuture().Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
      TestCheckEqual(3u, value.GetValue().size());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_ThenDefault_MaybeRef) {
    bool invoked{false};
    auto future = CreateMaybeVectorFutureFuture().Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
      TestCheckEqual(3u, value.TakeValue().size());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_ThenDefault_Void) {
    bool invoked{false};
    auto future = CreateMaybeVoidFutureFuture().Then([&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_ThenDefault_MaybeVoid) {
    bool invoked{false};
    auto future = CreateMaybeVoidFutureFuture().Then([&](Mso::Maybe<void> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_ThenDefault_MaybeVoidRef) {
    bool invoked{false};
    auto future = CreateMaybeVoidFutureFuture().Then([&](const Mso::Maybe<void> &value) noexcept {
      invoked = true;
      TestCheck(value.IsValue());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_ThenDefault_RValue) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Then([&](std::vector<int> &&value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_ThenDefault_ConstRef) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Then([&](const std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_ThenDefault_Ref) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Then([&](std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
      value[2] = 5; // to avoid OACR error about non-const parameter.
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_ThenDefault_Maybe) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_ThenDefault_MaybeRef) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_ThenDefault_Void) {
    bool invoked{false};
    auto future = CreateMaybeErrorVoidFutureFuture().Then([&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_ThenDefault_MaybeVoid) {
    bool invoked{false};
    auto future = CreateMaybeErrorVoidFutureFuture().Then([&](Mso::Maybe<void> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_ThenDefault_MaybeVoidRef) {
    bool invoked{false};
    auto future = CreateMaybeErrorVoidFutureFuture().Then([&](Mso::Maybe<void> &value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_ThenDefault_RValue) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Then([&](std::vector<int> &&value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_ThenDefault_ConstRef) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Then([&](const std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_ThenDefault_Ref) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Then([&](std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
      value[2] = 5; // to avoid OACR error about non-const parameter.
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_ThenDefault_Maybe) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_ThenDefault_MaybeRef) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_ThenDefault_Void) {
    bool invoked{false};
    auto future = CreateMaybeErrorVoidFutureFuture().Then([&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_ThenDefault_MaybeVoid) {
    bool invoked{false};
    auto future = CreateMaybeErrorVoidFutureFuture().Then([&](Mso::Maybe<void> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_ThenDefault_MaybeVoidRef) {
    bool invoked{false};
    auto future = CreateMaybeErrorVoidFutureFuture().Then([&](Mso::Maybe<void> &value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_ThenDefault_RValue) {
    bool invoked{false};
    auto future = CreateMaybeFailingVectorFutureFuture().Then([&](std::vector<int> &&value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_ThenDefault_ConstRef) {
    bool invoked{false};
    auto future = CreateMaybeFailingVectorFutureFuture().Then([&](const std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_ThenDefault_Ref) {
    bool invoked{false};
    auto future = CreateMaybeFailingVectorFutureFuture().Then([&](std::vector<int> &value) noexcept {
      invoked = true;
      TestCheckEqual(3u, value.size());
      value[2] = 5; // to avoid OACR error about non-const parameter.
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_ThenDefault_Maybe) {
    bool invoked{false};
    auto future = CreateMaybeFailingVectorFutureFuture().Then([&](Mso::Maybe<std::vector<int>> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_ThenDefault_MaybeRef) {
    bool invoked{false};
    auto future = CreateMaybeFailingVectorFutureFuture().Then([&](Mso::Maybe<std::vector<int>> &value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_ThenDefault_Void) {
    bool invoked{false};
    auto future = CreateMaybeFailingVoidFutureFuture().Then([&]() noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_ThenDefault_MaybeVoid) {
    bool invoked{false};
    auto future = CreateMaybeFailingVoidFutureFuture().Then([&](Mso::Maybe<void> &&value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.GetError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_ThenDefault_MaybeVoidRef) {
    bool invoked{false};
    auto future = CreateMaybeFailingVoidFutureFuture().Then([&](Mso::Maybe<void> &value) noexcept {
      invoked = true;
      TestCheck(value.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(value.TakeError()));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Catch) {
    bool invoked{false};
    auto future = CreateVectorFutureFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
      return std::vector<int>();
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Catch_Void) {
    bool invoked{false};
    auto future = CreateVoidFutureFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Error_Catch) {
    bool invoked{false};
    auto future =
        CreateErrorVectorFutureFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
          invoked = true;
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
          return std::vector<int>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Error_Catch_Void) {
    bool invoked{false};
    auto future =
        CreateErrorVoidFutureFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
          invoked = true;
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Failing_Catch) {
    bool invoked{false};
    auto future =
        CreateFailingVectorFutureFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
          invoked = true;
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
          return std::vector<int>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Failing_Catch_Void) {
    bool invoked{false};
    auto future =
        CreateFailingVoidFutureFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
          invoked = true;
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Catch) {
    bool invoked{false};
    auto future =
        CreateMaybeVectorFutureFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
          invoked = true;
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
          return std::vector<int>();
        });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Catch_Void) {
    bool invoked{false};
    auto future =
        CreateMaybeVoidFutureFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
          invoked = true;
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
        });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_Catch) {
    bool invoked{false};
    auto future =
        CreateErrorMaybeVectorFutureFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
          invoked = true;
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
          return std::vector<int>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_Catch_Void) {
    bool invoked{false};
    auto future =
        CreateErrorMaybeVoidFutureFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
          invoked = true;
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_Catch) {
    bool invoked{false};
    auto future =
        CreateMaybeErrorVectorFutureFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
          invoked = true;
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
          return std::vector<int>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_Catch_Void) {
    bool invoked{false};
    auto future =
        CreateMaybeErrorVoidFutureFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
          invoked = true;
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_Catch) {
    bool invoked{false};
    auto future = CreateMaybeFailingVectorFutureFuture().Catch(
        Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
          invoked = true;
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
          return std::vector<int>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_Catch_Void) {
    bool invoked{false};
    auto future =
        CreateMaybeFailingVoidFutureFuture().Catch(Mso::Executors::Inline{}, [&](const Mso::ErrorCode &error) noexcept {
          invoked = true;
          TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_CatchDefault) {
    bool invoked{false};
    auto future = CreateVectorFutureFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
      return std::vector<int>();
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_CatchDefault_Void) {
    bool invoked{false};
    auto future = CreateVoidFutureFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_Future_Error_CatchDefault) {
    bool invoked{false};
    auto future = CreateErrorVectorFutureFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
      return std::vector<int>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Error_CatchDefault_Void) {
    bool invoked{false};
    auto future = CreateErrorVoidFutureFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Failing_CatchDefault) {
    bool invoked{false};
    auto future = CreateFailingVectorFutureFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
      return std::vector<int>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Future_Failing_CatchDefault_Void) {
    bool invoked{false};
    auto future = CreateFailingVoidFutureFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_CatchDefault) {
    bool invoked{false};
    auto future = CreateMaybeVectorFutureFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
      return std::vector<int>();
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_MaybeFuture_CatchDefault_Void) {
    bool invoked{false};
    auto future = CreateMaybeVoidFutureFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
    });

    Mso::FutureWait(future);
    TestCheck(!invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_CatchDefault) {
    bool invoked{false};
    auto future = CreateErrorMaybeVectorFutureFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
      return std::vector<int>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ErrorMaybeFuture_CatchDefault_Void) {
    bool invoked{false};
    auto future = CreateErrorMaybeVoidFutureFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_CatchDefault) {
    bool invoked{false};
    auto future = CreateMaybeErrorVectorFutureFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
      return std::vector<int>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Error_CatchDefault_Void) {
    bool invoked{false};
    auto future = CreateMaybeErrorVoidFutureFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_CatchDefault) {
    bool invoked{false};
    auto future = CreateMaybeFailingVectorFutureFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
      return std::vector<int>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_MaybeFuture_Failing_CatchDefault_Void) {
    bool invoked{false};
    auto future = CreateMaybeFailingVoidFutureFuture().Catch([&](const Mso::ErrorCode &error) noexcept {
      invoked = true;
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(error));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_Vector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> &&value) noexcept {
          invoked = true;
          return std::move(value);
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_Int) {
    bool invoked{false};
    Mso::Future<int> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> && /*value*/) noexcept {
          invoked = true;
          return 5;
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_Void) {
    bool invoked{false};
    Mso::Future<void> future = CreateVectorFuture().Then(
        Mso::Executors::Inline(), [&](std::vector<int> && /*value*/) noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_MaybeVector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> &&value) noexcept {
          invoked = true;
          return Mso::Maybe<std::vector<int>>(std::move(value));
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_MaybeInt) {
    bool invoked{false};
    Mso::Future<int> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> && /*value*/) noexcept {
          invoked = true;
          return Mso::Maybe<int>(5);
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_MaybeVoid) {
    bool invoked{false};
    Mso::Future<void> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> && /*value*/) noexcept {
          invoked = true;
          return Mso::Maybe<void>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_MaybeVectorError) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> && /*value*/) noexcept {
          invoked = true;
          return Mso::CancellationErrorProvider().MakeMaybe<std::vector<int>>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_MaybeIntError) {
    bool invoked{false};
    Mso::Future<int> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> && /*value*/) noexcept {
          invoked = true;
          return Mso::CancellationErrorProvider().MakeMaybe<int>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_MaybeVoidError) {
    bool invoked{false};
    Mso::Future<void> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> && /*value*/) noexcept {
          invoked = true;
          return Mso::CancellationErrorProvider().MakeMaybe<void>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_FutureVector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> && /*value*/) noexcept {
          invoked = true;
          Mso::Promise<std::vector<int>> p1;
          p1.SetValue(std::vector<int>{1, 2, 3});
          return p1.AsFuture();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_FutureInt) {
    bool invoked{false};
    Mso::Future<int> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> && /*value*/) noexcept {
          invoked = true;
          Mso::Promise<int> p1;
          p1.SetValue(5);
          return p1.AsFuture();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_FutureVoid) {
    bool invoked{false};
    Mso::Future<void> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> && /*value*/) noexcept {
          invoked = true;
          Mso::Promise<void> p1;
          p1.SetValue();
          return p1.AsFuture();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_MaybeFutureVector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> && /*value*/) noexcept {
          invoked = true;
          Mso::Promise<std::vector<int>> p1;
          p1.SetValue(std::vector<int>{1, 2, 3});
          return Mso::Maybe<Mso::Future<std::vector<int>>>(p1.AsFuture());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_MaybeFutureInt) {
    bool invoked{false};
    Mso::Future<int> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> && /*value*/) noexcept {
          invoked = true;
          Mso::Promise<int> p1;
          p1.SetValue(5);
          return Mso::Maybe<Mso::Future<int>>(p1.AsFuture());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_MaybeFutureVoid) {
    bool invoked{false};
    Mso::Future<void> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> && /*value*/) noexcept {
          invoked = true;
          Mso::Promise<void> p1;
          p1.SetValue();
          return Mso::Maybe<Mso::Future<void>>(p1.AsFuture());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_MaybeErrorFutureVector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> && /*value*/) noexcept {
          invoked = true;
          return Mso::CancellationErrorProvider().MakeMaybe<Mso::Future<std::vector<int>>>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_MaybeErrorFutureInt) {
    bool invoked{false};
    Mso::Future<int> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> && /*value*/) noexcept {
          invoked = true;
          return Mso::CancellationErrorProvider().MakeMaybe<Mso::Future<int>>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Then_Returns_MaybeErrorFutureVoid) {
    bool invoked{false};
    Mso::Future<void> future =
        CreateVectorFuture().Then(Mso::Executors::Inline(), [&](std::vector<int> && /*value*/) noexcept {
          invoked = true;
          return Mso::CancellationErrorProvider().MakeMaybe<Mso::Future<void>>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_Vector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future = CreateVectorFuture().Then([&](std::vector<int> &&value) noexcept {
      invoked = true;
      return std::move(value);
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_Int) {
    bool invoked{false};
    Mso::Future<int> future = CreateVectorFuture().Then([&](std::vector<int> && /*value*/) noexcept {
      invoked = true;
      return 5;
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_Void) {
    bool invoked{false};
    Mso::Future<void> future =
        CreateVectorFuture().Then([&](std::vector<int> && /*value*/) noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_MaybeVector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future = CreateVectorFuture().Then([&](std::vector<int> &&value) noexcept {
      invoked = true;
      return Mso::Maybe<std::vector<int>>(std::move(value));
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_MaybeInt) {
    bool invoked{false};
    Mso::Future<int> future = CreateVectorFuture().Then([&](std::vector<int> && /*value*/) noexcept {
      invoked = true;
      return Mso::Maybe<int>(5);
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_MaybeVoid) {
    bool invoked{false};
    Mso::Future<void> future = CreateVectorFuture().Then([&](std::vector<int> && /*value*/) noexcept {
      invoked = true;
      return Mso::Maybe<void>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_MaybeVectorError) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future = CreateVectorFuture().Then([&](std::vector<int> && /*value*/) noexcept {
      invoked = true;
      return Mso::CancellationErrorProvider().MakeMaybe<std::vector<int>>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_MaybeIntError) {
    bool invoked{false};
    Mso::Future<int> future = CreateVectorFuture().Then([&](std::vector<int> && /*value*/) noexcept {
      invoked = true;
      return Mso::CancellationErrorProvider().MakeMaybe<int>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_MaybeVoidError) {
    bool invoked{false};
    Mso::Future<void> future = CreateVectorFuture().Then([&](std::vector<int> && /*value*/) noexcept {
      invoked = true;
      return Mso::CancellationErrorProvider().MakeMaybe<void>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_FutureVector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future = CreateVectorFuture().Then([&](std::vector<int> && /*value*/) noexcept {
      invoked = true;
      Mso::Promise<std::vector<int>> p1;
      p1.SetValue(std::vector<int>{1, 2, 3});
      return p1.AsFuture();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_FutureInt) {
    bool invoked{false};
    Mso::Future<int> future = CreateVectorFuture().Then([&](std::vector<int> && /*value*/) noexcept {
      invoked = true;
      Mso::Promise<int> p1;
      p1.SetValue(5);
      return p1.AsFuture();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_FutureVoid) {
    bool invoked{false};
    Mso::Future<void> future = CreateVectorFuture().Then([&](std::vector<int> && /*value*/) noexcept {
      invoked = true;
      Mso::Promise<void> p1;
      p1.SetValue();
      return p1.AsFuture();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_MaybeFutureVector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future = CreateVectorFuture().Then([&](std::vector<int> && /*value*/) noexcept {
      invoked = true;
      Mso::Promise<std::vector<int>> p1;
      p1.SetValue(std::vector<int>{1, 2, 3});
      return Mso::Maybe<Mso::Future<std::vector<int>>>(p1.AsFuture());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_MaybeFutureInt) {
    bool invoked{false};
    Mso::Future<int> future = CreateVectorFuture().Then([&](std::vector<int> && /*value*/) noexcept {
      invoked = true;
      Mso::Promise<int> p1;
      p1.SetValue(5);
      return Mso::Maybe<Mso::Future<int>>(p1.AsFuture());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_MaybeFutureVoid) {
    bool invoked{false};
    Mso::Future<void> future = CreateVectorFuture().Then([&](std::vector<int> && /*value*/) noexcept {
      invoked = true;
      Mso::Promise<void> p1;
      p1.SetValue();
      return Mso::Maybe<Mso::Future<void>>(p1.AsFuture());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_MaybeErrorFutureVector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future = CreateVectorFuture().Then([&](std::vector<int> && /*value*/) noexcept {
      invoked = true;
      return Mso::CancellationErrorProvider().MakeMaybe<Mso::Future<std::vector<int>>>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_MaybeErrorFutureInt) {
    bool invoked{false};
    Mso::Future<int> future = CreateVectorFuture().Then([&](std::vector<int> && /*value*/) noexcept {
      invoked = true;
      return Mso::CancellationErrorProvider().MakeMaybe<Mso::Future<int>>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_ThenDefault_Returns_MaybeErrorFutureVoid) {
    bool invoked{false};
    Mso::Future<void> future = CreateVectorFuture().Then([&](std::vector<int> && /*value*/) noexcept {
      invoked = true;
      return Mso::CancellationErrorProvider().MakeMaybe<Mso::Future<void>>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_Vector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateErrorVectorFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          return std::vector<int>{1, 2, 3};
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_Int) {
    bool invoked{false};
    Mso::Future<int> future =
        CreateErrorIntFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          return 5;
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_Void) {
    bool invoked{false};
    Mso::Future<void> future = CreateErrorVoidFuture().Catch(
        Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_MaybeVector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateErrorVectorFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          return Mso::Maybe<std::vector<int>>(std::vector<int>{1, 2, 3});
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_MaybeInt) {
    bool invoked{false};
    Mso::Future<int> future =
        CreateErrorIntFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          return Mso::Maybe<int>(5);
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_MaybeVoid) {
    bool invoked{false};
    Mso::Future<void> future =
        CreateErrorVoidFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          return Mso::Maybe<void>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_MaybeVectorError) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateErrorVectorFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          return Mso::CancellationErrorProvider().MakeMaybe<std::vector<int>>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_MaybeIntError) {
    bool invoked{false};
    Mso::Future<int> future =
        CreateErrorIntFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          return Mso::CancellationErrorProvider().MakeMaybe<int>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_MaybeVoidError) {
    bool invoked{false};
    Mso::Future<void> future =
        CreateErrorVoidFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          return Mso::CancellationErrorProvider().MakeMaybe<void>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_FutureVector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateErrorVectorFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          Mso::Promise<std::vector<int>> p1;
          p1.SetValue(std::vector<int>{1, 2, 3});
          return p1.AsFuture();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_FutureInt) {
    bool invoked{false};
    Mso::Future<int> future =
        CreateErrorIntFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          Mso::Promise<int> p1;
          p1.SetValue(5);
          return p1.AsFuture();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_FutureVoid) {
    bool invoked{false};
    Mso::Future<void> future =
        CreateErrorVoidFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          Mso::Promise<void> p1;
          p1.SetValue();
          return p1.AsFuture();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_MaybeFutureVector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateErrorVectorFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          Mso::Promise<std::vector<int>> p1;
          p1.SetValue(std::vector<int>{1, 2, 3});
          return Mso::Maybe<Mso::Future<std::vector<int>>>(p1.AsFuture());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_MaybeFutureInt) {
    bool invoked{false};
    Mso::Future<int> future =
        CreateErrorIntFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          Mso::Promise<int> p1;
          p1.SetValue(5);
          return Mso::Maybe<Mso::Future<int>>(p1.AsFuture());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_MaybeFutureVoid) {
    bool invoked{false};
    Mso::Future<void> future =
        CreateErrorVoidFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          Mso::Promise<void> p1;
          p1.SetValue();
          return Mso::Maybe<Mso::Future<void>>(p1.AsFuture());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_MaybeErrorFutureVector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateErrorVectorFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          return Mso::CancellationErrorProvider().MakeMaybe<Mso::Future<std::vector<int>>>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_MaybeErrorFutureInt) {
    bool invoked{false};
    Mso::Future<int> future =
        CreateErrorIntFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          return Mso::CancellationErrorProvider().MakeMaybe<Mso::Future<int>>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_Catch_Returns_MaybeErrorFutureVoid) {
    bool invoked{false};
    Mso::Future<void> future =
        CreateErrorVoidFuture().Catch(Mso::Executors::Inline(), [&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          return Mso::CancellationErrorProvider().MakeMaybe<Mso::Future<void>>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_Vector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateErrorVectorFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          return std::vector<int>{1, 2, 3};
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_Int) {
    bool invoked{false};
    Mso::Future<int> future = CreateErrorIntFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
      invoked = true;
      return 5;
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_Void) {
    bool invoked{false};
    Mso::Future<void> future =
        CreateErrorVoidFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept { invoked = true; });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_MaybeVector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateErrorVectorFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          return Mso::Maybe<std::vector<int>>(std::vector<int>{1, 2, 3});
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_MaybeInt) {
    bool invoked{false};
    Mso::Future<int> future = CreateErrorIntFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
      invoked = true;
      return Mso::Maybe<int>(5);
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_MaybeVoid) {
    bool invoked{false};
    Mso::Future<void> future = CreateErrorVoidFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
      invoked = true;
      return Mso::Maybe<void>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_MaybeVectorError) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateErrorVectorFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          return Mso::CancellationErrorProvider().MakeMaybe<std::vector<int>>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_MaybeIntError) {
    bool invoked{false};
    Mso::Future<int> future = CreateErrorIntFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
      invoked = true;
      return Mso::CancellationErrorProvider().MakeMaybe<int>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_MaybeVoidError) {
    bool invoked{false};
    Mso::Future<void> future = CreateErrorVoidFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
      invoked = true;
      return Mso::CancellationErrorProvider().MakeMaybe<void>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_FutureVector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateErrorVectorFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          Mso::Promise<std::vector<int>> p1;
          p1.SetValue(std::vector<int>{1, 2, 3});
          return p1.AsFuture();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_FutureInt) {
    bool invoked{false};
    Mso::Future<int> future = CreateErrorIntFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
      invoked = true;
      Mso::Promise<int> p1;
      p1.SetValue(5);
      return p1.AsFuture();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_FutureVoid) {
    bool invoked{false};
    Mso::Future<void> future = CreateErrorVoidFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
      invoked = true;
      Mso::Promise<void> p1;
      p1.SetValue();
      return p1.AsFuture();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_MaybeFutureVector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateErrorVectorFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          Mso::Promise<std::vector<int>> p1;
          p1.SetValue(std::vector<int>{1, 2, 3});
          return Mso::Maybe<Mso::Future<std::vector<int>>>(p1.AsFuture());
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_MaybeFutureInt) {
    bool invoked{false};
    Mso::Future<int> future = CreateErrorIntFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
      invoked = true;
      Mso::Promise<int> p1;
      p1.SetValue(5);
      return Mso::Maybe<Mso::Future<int>>(p1.AsFuture());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_MaybeFutureVoid) {
    bool invoked{false};
    Mso::Future<void> future = CreateErrorVoidFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
      invoked = true;
      Mso::Promise<void> p1;
      p1.SetValue();
      return Mso::Maybe<Mso::Future<void>>(p1.AsFuture());
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_MaybeErrorFutureVector) {
    bool invoked{false};
    Mso::Future<std::vector<int>> future =
        CreateErrorVectorFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
          invoked = true;
          return Mso::CancellationErrorProvider().MakeMaybe<Mso::Future<std::vector<int>>>();
        });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_MaybeErrorFutureInt) {
    bool invoked{false};
    Mso::Future<int> future = CreateErrorIntFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
      invoked = true;
      return Mso::CancellationErrorProvider().MakeMaybe<Mso::Future<int>>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }

  TEST_METHOD(Future_CatchDefault_Returns_MaybeErrorFutureVoid) {
    bool invoked{false};
    Mso::Future<void> future = CreateErrorVoidFuture().Catch([&](const Mso::ErrorCode & /*error*/) noexcept {
      invoked = true;
      return Mso::CancellationErrorProvider().MakeMaybe<Mso::Future<void>>();
    });

    Mso::FutureWait(future);
    TestCheck(invoked);
  }
};

} // namespace FutureTests
