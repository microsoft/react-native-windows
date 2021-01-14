// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include <thread>
#include "cppExtensions/autoRestore.h"
#include "future/future.h"
#include "future/futureWait.h"
#include "motifCpp/libletAwareMemLeakDetection.h"
#include "testCheck.h"
#include "testExecutor.h"

namespace FutureTests {

TEST_CLASS_EX (WhenAllTest, LibletAwareMemLeakDetection) {
  // MemoryLeakDetectionHook::TrackPerTest m_trackLeakPerTest;

  TEST_METHOD(WhenAll_Init_Three) {
    auto f1 = Mso::PostFuture([]() noexcept { return 1; });
    auto f2 = Mso::PostFuture([]() noexcept { return 3; });
    auto f3 = Mso::PostFuture([]() noexcept { return 5; });

    auto fr = Mso::WhenAll({f1, f2, f3}).Then([](Mso::Async::ArrayView<int> result) noexcept {
      return result[0] + result[1] + result[2];
    });

    TestCheckEqual(9, Mso::FutureWaitAndGetValue(fr));
  }

  TEST_METHOD(WhenAll_Init_Empty) {
    auto fr = Mso::WhenAll<int>({}).Then([](Mso::Async::ArrayView<int> result) noexcept {
      TestCheckEqual(0u, result.Size());
      return 42;
    });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
  }

  TEST_METHOD(WhenAll_Init_Three_Error) {
    auto f1 = Mso::PostFuture([]() noexcept { return 1; });
    auto f2 = Mso::MakeFailedFuture<int>(Mso::CancellationErrorProvider().MakeErrorCode(true));
    auto f3 = Mso::PostFuture([]() noexcept { return 5; });

    auto fr = Mso::WhenAll({f1, f2, f3}).Then([](Mso::Async::ArrayView<int> result) noexcept {
      return result[0] + result[1] + result[2];
    });

    TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(Mso::FutureWaitAndGetError(fr)));
  }

  TEST_METHOD(WhenAll_Init_Two_Double) {
    auto f1 = Mso::PostFuture([]() noexcept { return 1.0; });
    auto f2 = Mso::PostFuture([]() noexcept { return 3.0; });

    auto fr = Mso::WhenAll({f1, f2}).Then([](Mso::Async::ArrayView<double> r) noexcept {
      // Check value alignment
      TestCheckEqual(0u, reinterpret_cast<uintptr_t>(&r[0]) & (sizeof(double) - 1));
      TestCheckEqual(0u, reinterpret_cast<uintptr_t>(&r[1]) & (sizeof(double) - 1));
      return r[0] + r[1];
    });

    TestCheckEqual(4.0, Mso::FutureWaitAndGetValue(fr));
  }

  TEST_METHOD(WhenAll_Init_Three_Double) {
    auto f1 = Mso::PostFuture([]() noexcept { return 1.0; });
    auto f2 = Mso::PostFuture([]() noexcept { return 3.0; });
    auto f3 = Mso::PostFuture([]() noexcept { return 5.0; });

    auto fr = Mso::WhenAll({f1, f2, f3}).Then([](Mso::Async::ArrayView<double> r) noexcept {
      // Check value alignment
      TestCheckEqual(0u, reinterpret_cast<uintptr_t>(&r[0]) & (sizeof(double) - 1));
      TestCheckEqual(0u, reinterpret_cast<uintptr_t>(&r[1]) & (sizeof(double) - 1));
      TestCheckEqual(0u, reinterpret_cast<uintptr_t>(&r[2]) & (sizeof(double) - 1));
      return r[0] + r[1] + r[2];
    });

    TestCheckEqual(9.0, Mso::FutureWaitAndGetValue(fr));
  }

  TEST_METHOD(WhenAll_Array_Three) {
    Mso::Future<int> futures[] = {
        Mso::PostFuture([]() noexcept { return 1; }),
        Mso::PostFuture([]() noexcept { return 3; }),
        Mso::PostFuture([]() noexcept { return 5; }),
    };

    auto fr = Mso::WhenAll(futures).Then([](Mso::Async::ArrayView<int> r) noexcept { return r[0] + r[1] + r[2]; });

    TestCheckEqual(9, Mso::FutureWaitAndGetValue(fr));
  }

  TEST_METHOD(WhenAll_Array_Three_Error) {
    Mso::Future<int> futures[] = {
        Mso::PostFuture([]() noexcept { return 1; }),
        Mso::MakeFailedFuture<int>(Mso::CancellationErrorProvider().MakeErrorCode(true)),
        Mso::PostFuture([]() noexcept { return 5; }),
    };

    auto fr = Mso::WhenAll(futures).Then([](Mso::Async::ArrayView<int> r) noexcept { return r[0] + r[1] + r[2]; });

    TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(Mso::FutureWaitAndGetError(fr)));
  }

  TEST_METHOD(WhenAll_Vector_Three) {
    auto futures = std::vector<Mso::Future<int>>{
        Mso::PostFuture([]() noexcept { return 1; }),
        Mso::PostFuture([]() noexcept { return 3; }),
        Mso::PostFuture([]() noexcept { return 5; }),
    };

    auto fr = Mso::WhenAll(futures).Then([](Mso::Async::ArrayView<int> r) noexcept { return r[0] + r[1] + r[2]; });

    TestCheckEqual(9, Mso::FutureWaitAndGetValue(fr));
  }

  TEST_METHOD(WhenAll_Vector_Empty) {
    auto fr = Mso::WhenAll(std::vector<Mso::Future<int>>()).Then([](Mso::Async::ArrayView<int> r) noexcept {
      TestCheckEqual(0u, r.Size());
      return 42;
    });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
  }

  TEST_METHOD(WhenAll_Vector_Three_Error) {
    auto futures = std::vector<Mso::Future<int>>{
        Mso::PostFuture([]() noexcept { return 1; }),
        Mso::MakeFailedFuture<int>(Mso::CancellationErrorProvider().MakeErrorCode(true)),
        Mso::PostFuture([]() noexcept { return 5; }),
    };

    auto fr = Mso::WhenAll(futures).Then([](Mso::Async::ArrayView<int> r) noexcept { return r[0] + r[1] + r[2]; });

    TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(Mso::FutureWaitAndGetError(fr)));
  }

  TEST_METHOD(WhenAll_Init_Void_Three) {
    int r1 = 0;
    int r2 = 0;
    int r3 = 0;
    auto f1 = Mso::PostFuture([&]() noexcept { r1 = 1; });
    auto f2 = Mso::PostFuture([&]() noexcept { r2 = 3; });
    auto f3 = Mso::PostFuture([&]() noexcept { r3 = 5; });

    auto fr = Mso::WhenAll({f1, f2, f3}).Then([&]() noexcept { return r1 + r2 + r3; });

    TestCheckEqual(9, Mso::FutureWaitAndGetValue(fr));
  }

  TEST_METHOD(WhenAll_Init_Void_Empty) {
    auto fr = Mso::WhenAll({}).Then([&]() noexcept { return 42; });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
  }

  TEST_METHOD(WhenAll_Init_Void_Three_Error) {
    int r1 = 0;
    int r2 = 0;
    int r3 = 0;
    auto f1 = Mso::PostFuture([&]() noexcept { r1 = 1; });
    auto f2 = Mso::MakeFailedFuture<void>(Mso::CancellationErrorProvider().MakeErrorCode(true));
    auto f3 = Mso::PostFuture([&]() noexcept { r3 = 5; });

    auto fr = Mso::WhenAll({f1, f2, f3}).Then([&]() noexcept { return r1 + r2 + r3; });

    TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(Mso::FutureWaitAndGetError(fr)));
  }

  TEST_METHOD(WhenAll_Array_Void_Three) {
    int r1 = 0;
    int r2 = 0;
    int r3 = 0;
    Mso::Future<void> futures[] = {
        Mso::PostFuture([&]() noexcept { r1 = 1; }),
        Mso::PostFuture([&]() noexcept { r2 = 3; }),
        Mso::PostFuture([&]() noexcept { r3 = 5; }),
    };

    auto fr = Mso::WhenAll(futures).Then([&]() noexcept { return r1 + r2 + r3; });

    TestCheckEqual(9, Mso::FutureWaitAndGetValue(fr));
  }

  TEST_METHOD(WhenAll_Array_Void_Three_Error) {
    int r1 = 0;
    int r2 = 0;
    int r3 = 0;
    Mso::Future<void> futures[] = {
        Mso::PostFuture([&]() noexcept { r1 = 1; }),
        Mso::MakeFailedFuture<void>(Mso::CancellationErrorProvider().MakeErrorCode(true)),
        Mso::PostFuture([&]() noexcept { r3 = 5; }),
    };

    auto fr = Mso::WhenAll(futures).Then([&]() noexcept { return r1 + r2 + r3; });

    TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(Mso::FutureWaitAndGetError(fr)));
  }

  TEST_METHOD(WhenAll_Vector_Void_Three) {
    int r1 = 0;
    int r2 = 0;
    int r3 = 0;
    auto futures = std::vector<Mso::Future<void>>{
        Mso::PostFuture([&]() noexcept { r1 = 1; }),
        Mso::PostFuture([&]() noexcept { r2 = 3; }),
        Mso::PostFuture([&]() noexcept { r3 = 5; }),
    };

    auto fr = Mso::WhenAll(futures).Then([&]() noexcept { return r1 + r2 + r3; });

    TestCheckEqual(9, Mso::FutureWaitAndGetValue(fr));
  }

  TEST_METHOD(WhenAll_Vector_Void_Empty) {
    auto fr = Mso::WhenAll(std::vector<Mso::Future<void>>()).Then([&]() noexcept { return 42; });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
  }

  TEST_METHOD(WhenAll_Vector_Void_Three_Error) {
    int r1 = 0;
    int r2 = 0;
    int r3 = 0;
    auto futures = std::vector<Mso::Future<void>>{
        Mso::PostFuture([&]() noexcept { r1 = 1; }),
        Mso::MakeFailedFuture<void>(Mso::CancellationErrorProvider().MakeErrorCode(true)),
        Mso::PostFuture([&]() noexcept { r3 = 5; }),
    };

    auto fr = Mso::WhenAll(futures).Then([&]() noexcept { return r1 + r2 + r3; });

    TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(Mso::FutureWaitAndGetError(fr)));
  }

  TEST_METHOD(WhenAll_Tuple_Three) {
    auto f1 = Mso::PostFuture([]() noexcept { return 47; });
    auto f2 = Mso::PostFuture([]() noexcept -> std::string { return "82"; });
    auto f3 = Mso::PostFuture([]() noexcept -> double { return 32.5; });

    auto fr = Mso::WhenAll(f1, f2, f3).Then([](std::tuple<int, std::string, double> &&result) noexcept {
      TestCheck(std::get<0>(result) == 47);
      TestCheck(std::stoi(std::get<1>(result)) == 82);
      TestCheck(int(std::get<2>(result)) == 32);
      return std::get<0>(result) + std::stoi(std::get<1>(result)) + int(std::get<2>(result));
    });

    TestCheckEqual(161, Mso::FutureWaitAndGetValue(fr));
  }

  TEST_METHOD(WhenAll_Tuple_Two_Error) {
    Mso::ManualResetEvent complete;
    auto f1 = Mso::MakeFailedFuture<int>(Mso::CancellationErrorProvider().MakeErrorCode(true));
    auto f2 = Mso::PostFuture([&]() noexcept -> std::string {
      complete.Wait();
      return "82";
    });

    auto fr = Mso::WhenAll(f1, f2).Then([](const std::tuple<int, std::string> &&) noexcept {});
    TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(Mso::FutureWaitAndGetError(fr)));

    complete.Set();
  }
};

} // namespace FutureTests
