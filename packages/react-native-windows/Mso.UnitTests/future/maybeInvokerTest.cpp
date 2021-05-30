// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "future/details/maybeInvoker.h"
#include "motifCpp/libletAwareMemLeakDetection.h"
#include "testCheck.h"

namespace FutureTests {

TEST_CLASS_EX (MaybeInvokerTest, LibletAwareMemLeakDetection) {
  // MemoryLeakDetectionHook::TrackPerTest m_trackLeakPerTest;

  TEST_METHOD(MaybeInvoker_int_nothrow) {
    Mso::Maybe<int> result = Mso::Futures::MaybeInvoker<int, false>::Invoke([](int x, int y) { return x + y; }, 2, 3);

    TestCheckEqual(5, result.GetValue());
  }

  TEST_METHOD(MaybeInvoker_int_nothrow_noexcept) {
    Mso::Maybe<int> result =
        Mso::Futures::MaybeInvoker<int, true>::Invoke([](int x, int y) noexcept { return x + y; }, 2, 3);

    TestCheckEqual(5, result.GetValue());
  }

  TEST_METHOD(MaybeInvoker_int_throw) {
    Mso::Maybe<int> result = Mso::Futures::MaybeInvoker<int, false>::Invoke(
        [](int, int) -> int { throw std::runtime_error("Expected exception"); }, 2, 3);

    TestCheck(Mso::ExceptionErrorProvider().IsOwnedErrorCode(result.GetError()));
  }

  TEST_METHOD(MaybeInvoker_void_nothrow) {
    int sum = 0;
    Mso::Maybe<void> result = Mso::Futures::MaybeInvoker<void, false>::Invoke([&](int x, int y) { sum = x + y; }, 2, 3);

    TestCheck(result.IsValue());
    TestCheckEqual(5, sum);
  }

  TEST_METHOD(MaybeInvoker_void_nothrow_noexcept) {
    int sum = 0;
    Mso::Maybe<void> result =
        Mso::Futures::MaybeInvoker<void, true>::Invoke([&](int x, int y) noexcept { sum = x + y; }, 2, 3);

    TestCheck(result.IsValue());
    TestCheckEqual(5, sum);
  }

  TEST_METHOD(MaybeInvoker_void_throw) {
    Mso::Maybe<void> result = Mso::Futures::MaybeInvoker<void, false>::Invoke(
        [](int, int) { throw std::runtime_error("Expected exception"); }, 2, 3);

    TestCheck(Mso::ExceptionErrorProvider().IsOwnedErrorCode(result.GetError()));
  }

  TEST_METHOD(MaybeInvoker_Maybe_int_nothrow) {
    Mso::Maybe<int> result = Mso::Futures::MaybeInvoker<Mso::Maybe<int>, false>::Invoke(
        [](int x, int y) { return Mso::Maybe<int>(x + y); }, 2, 3);

    TestCheckEqual(5, result.GetValue());
  }

  TEST_METHOD(MaybeInvoker_Maybe_int_nothrow_noexcept) {
    Mso::Maybe<int> result = Mso::Futures::MaybeInvoker<Mso::Maybe<int>, true>::Invoke(
        [](int x, int y) noexcept { return Mso::Maybe<int>(x + y); }, 2, 3);

    TestCheckEqual(5, result.GetValue());
  }

  TEST_METHOD(MaybeInvoker_Maybe_int_throw) {
    Mso::Maybe<int> result = Mso::Futures::MaybeInvoker<Mso::Maybe<int>, false>::Invoke(
        [](int, int) -> Mso::Maybe<int> { throw std::runtime_error("Expected exception"); }, 2, 3);

    TestCheck(Mso::ExceptionErrorProvider().IsOwnedErrorCode(result.GetError()));
  }

  TEST_METHOD(MaybeInvoker_Maybe_void_nothrow) {
    int sum = 0;
    Mso::Maybe<void> result = Mso::Futures::MaybeInvoker<Mso::Maybe<void>, false>::Invoke(
        [&](int x, int y) {
          sum = x + y;
          return Mso::Maybe<void>();
        },
        2,
        3);

    TestCheck(result.IsValue());
    TestCheckEqual(5, sum);
  }

  TEST_METHOD(MaybeInvoker_Maybe_void_nothrow_noexcept) {
    int sum = 0;
    Mso::Maybe<void> result = Mso::Futures::MaybeInvoker<Mso::Maybe<void>, true>::Invoke(
        [&](int x, int y) noexcept {
          sum = x + y;
          return Mso::Maybe<void>();
        },
        2,
        3);

    TestCheck(result.IsValue());
    TestCheckEqual(5, sum);
  }

  TEST_METHOD(MaybeInvoker_Maybe_void_throw) {
    Mso::Maybe<void> result = Mso::Futures::MaybeInvoker<Mso::Maybe<void>, false>::Invoke(
        [](int, int) -> Mso::Maybe<void> { throw std::runtime_error("Expected exception"); }, 2, 3);

    TestCheck(Mso::ExceptionErrorProvider().IsOwnedErrorCode(result.GetError()));
  }
};

} // namespace FutureTests
