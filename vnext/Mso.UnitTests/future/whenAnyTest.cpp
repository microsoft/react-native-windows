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

TEST_CLASS_EX (WhenAnyTest, LibletAwareMemLeakDetection) {
  // MemoryLeakDetectionHook::TrackPerTest m_trackLeakPerTest;

  TEST_METHOD(WhenAny_Init_Three) {
    std::atomic<int32_t> finishCount{0};
    Mso::ManualResetEvent finished13;
    auto f1 = Mso::PostFuture([&]() noexcept {
      finished13.Wait();
      ++finishCount;
      return 1;
    });
    auto f2 = Mso::PostFuture([&]() noexcept {
      ++finishCount;
      return 3;
    });
    auto f3 = Mso::PostFuture([&]() noexcept {
      finished13.Wait();
      ++finishCount;
      return 5;
    });

    auto fr = Mso::WhenAny({f1, f2, f3}).Then([](int r) noexcept->int { return r; });

    TestCheckEqual(3, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();

    while (finishCount != 3) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  TESTMETHOD_REQUIRES_SEH(WhenAny_Init_Empty) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestCheckCrash(Mso::WhenAny({}));
  }

  TEST_METHOD(WhenAny_Init_Three_Error) {
    std::atomic<int32_t> finishCount{0};
    Mso::ManualResetEvent finished13;
    auto f1 = Mso::PostFuture([&]() noexcept {
      finished13.Wait();
      ++finishCount;
      return 1;
    });
    auto f2 = Mso::MakeFailedFuture<int>(Mso::CancellationErrorProvider().MakeErrorCode(true));
    auto f3 = Mso::PostFuture([&]() noexcept {
      finished13.Wait();
      ++finishCount;
      return 5;
    });

    auto fr = Mso::WhenAny({f1, f2, f3}).Then([](Mso::Maybe<int> && result) noexcept {
      TestCheck(result.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(result.GetError()));
      return 42;
    });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();

    while (finishCount != 2) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  TEST_METHOD(WhenAny_Array_Three) {
    std::atomic<int32_t> finishCount{0};
    Mso::ManualResetEvent finished13;
    Mso::Future<int> futures[] = {Mso::PostFuture([&]() noexcept {
                                    finished13.Wait();
                                    ++finishCount;
                                    return 1;
                                  }),
                                  Mso::PostFuture([&]() noexcept {
                                    ++finishCount;
                                    return 3;
                                  }),
                                  Mso::PostFuture([&]() noexcept {
                                    finished13.Wait();
                                    ++finishCount;
                                    return 5;
                                  })};

    auto fr = Mso::WhenAny(futures).Then([](int r) noexcept { return r; });

    TestCheckEqual(3, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();

    while (finishCount != 3) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  TEST_METHOD(WhenAny_Array_Three_Error) {
    std::atomic<int32_t> finishCount{0};
    Mso::ManualResetEvent finished13;
    Mso::Future<int> futures[] = {Mso::PostFuture([&]() noexcept {
                                    finished13.Wait();
                                    ++finishCount;
                                    return 1;
                                  }),
                                  Mso::MakeFailedFuture<int>(Mso::CancellationErrorProvider().MakeErrorCode(true)),
                                  Mso::PostFuture([&]() noexcept {
                                    finished13.Wait();
                                    ++finishCount;
                                    return 5;
                                  })};

    auto fr = Mso::WhenAny(futures).Then([](Mso::Maybe<int> && result) noexcept {
      TestCheck(result.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(result.GetError()));
      return 42;
    });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();

    while (finishCount != 2) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  TEST_METHOD(WhenAny_Vector_Three) {
    std::atomic<int32_t> finishCount{0};
    Mso::ManualResetEvent finished13;
    auto futures = std::vector<Mso::Future<int>>{Mso::PostFuture([&]() noexcept {
                                                   finished13.Wait();
                                                   ++finishCount;
                                                   return 1;
                                                 }),
                                                 Mso::PostFuture([&]() noexcept {
                                                   ++finishCount;
                                                   return 3;
                                                 }),
                                                 Mso::PostFuture([&]() noexcept {
                                                   finished13.Wait();
                                                   ++finishCount;
                                                   return 5;
                                                 })};

    auto fr = Mso::WhenAny(futures).Then([](int r) noexcept->int { return r; });

    TestCheckEqual(3, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();

    while (finishCount != 3) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  TESTMETHOD_REQUIRES_SEH(WhenAny_Vector_Empty) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestCheckCrash(Mso::WhenAny(std::vector<Mso::Future<int>>()));
  }

  TEST_METHOD(WhenAny_Vector_Three_Error) {
    std::atomic<int32_t> finishCount{0};
    Mso::ManualResetEvent finished13;
    auto futures =
        std::vector<Mso::Future<int>>{Mso::PostFuture([&]() noexcept {
                                        finished13.Wait();
                                        ++finishCount;
                                        return 1;
                                      }),
                                      Mso::MakeFailedFuture<int>(Mso::CancellationErrorProvider().MakeErrorCode(true)),
                                      Mso::PostFuture([&]() noexcept {
                                        finished13.Wait();
                                        ++finishCount;
                                        return 5;
                                      })};

    auto fr = Mso::WhenAny(futures).Then([](Mso::Maybe<int> && result) noexcept {
      TestCheck(result.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(result.GetError()));
      return 42;
    });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();

    while (finishCount != 2) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  TEST_METHOD(WhenAny_Init_Void_Three) {
    std::atomic<int32_t> finishCount{0};
    Mso::ManualResetEvent finished13;

    int r1 = 0;
    int r2 = 0;
    int r3 = 0;
    auto f1 = Mso::PostFuture([&]() noexcept {
      finished13.Wait();
      ++finishCount;
      r1 = 1;
    });
    auto f2 = Mso::PostFuture([&]() noexcept {
      ++finishCount;
      r2 = 3;
    });
    auto f3 = Mso::PostFuture([&]() noexcept {
      finished13.Wait();
      ++finishCount;
      r3 = 5;
    });

    auto fr = Mso::WhenAny({f1, f2, f3}).Then([&]() noexcept { return 42; });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
    TestCheckEqual(3, r2);
    finished13.Set();

    while (finishCount != 3) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  TESTMETHOD_REQUIRES_SEH(WhenAny_Init_Void_Empty) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestCheckCrash(Mso::WhenAny({}));
  }

  TEST_METHOD(WhenAny_Init_Void_Three_Error) {
    std::atomic<int32_t> finishCount{0};
    Mso::ManualResetEvent finished13;

    int r1 = 0;
    int r3 = 0;
    auto f1 = Mso::PostFuture([&]() noexcept {
      finished13.Wait();
      ++finishCount;
      r1 = 1;
    });
    auto f2 = Mso::MakeFailedFuture<void>(Mso::CancellationErrorProvider().MakeErrorCode(true));
    auto f3 = Mso::PostFuture([&]() noexcept {
      finished13.Wait();
      ++finishCount;
      r3 = 5;
    });

    auto fr = Mso::WhenAny({f1, f2, f3}).Then([&](Mso::Maybe<void> && result) noexcept {
      TestCheck(result.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(result.GetError()));
      return 42;
    });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();

    while (finishCount != 2) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  TEST_METHOD(WhenAny_Array_Void_Three) {
    std::atomic<int32_t> finishCount{0};
    Mso::ManualResetEvent finished13;

    int r1 = 0;
    int r2 = 0;
    int r3 = 0;
    Mso::Future<void> futures[] = {Mso::PostFuture([&]() noexcept {
                                     finished13.Wait();
                                     ++finishCount;
                                     r1 = 1;
                                   }),
                                   Mso::PostFuture([&]() noexcept {
                                     ++finishCount;
                                     r2 = 3;
                                   }),
                                   Mso::PostFuture([&]() noexcept {
                                     finished13.Wait();
                                     ++finishCount;
                                     r3 = 5;
                                   })};

    auto fr = Mso::WhenAny(futures).Then([&]() noexcept { return 42; });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
    TestCheckEqual(3, r2);
    finished13.Set();

    while (finishCount != 3) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  TEST_METHOD(WhenAny_Array_Void_Three_Error) {
    std::atomic<int32_t> finishCount{0};
    Mso::ManualResetEvent finished13;

    int r1 = 0;
    int r3 = 0;
    Mso::Future<void> futures[] = {Mso::PostFuture([&]() noexcept {
                                     finished13.Wait();
                                     ++finishCount;
                                     r1 = 1;
                                   }),
                                   Mso::MakeFailedFuture<void>(Mso::CancellationErrorProvider().MakeErrorCode(true)),
                                   Mso::PostFuture([&]() noexcept {
                                     finished13.Wait();
                                     ++finishCount;
                                     r3 = 5;
                                   })};

    auto fr = Mso::WhenAny(futures).Then([&](Mso::Maybe<void> && result) noexcept {
      TestCheck(result.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(result.GetError()));
      return 42;
    });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();

    while (finishCount != 2) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  TEST_METHOD(WhenAny_Vector_Void_Three) {
    std::atomic<int32_t> finishCount{0};
    Mso::ManualResetEvent finished13;

    int r1 = 0;
    int r2 = 0;
    int r3 = 0;
    auto futures = std::vector<Mso::Future<void>>{Mso::PostFuture([&]() noexcept {
                                                    finished13.Wait();
                                                    ++finishCount;
                                                    r1 = 1;
                                                  }),
                                                  Mso::PostFuture([&]() noexcept {
                                                    ++finishCount;
                                                    r2 = 3;
                                                  }),
                                                  Mso::PostFuture([&]() noexcept {
                                                    finished13.Wait();
                                                    ++finishCount;
                                                    r3 = 5;
                                                  })};

    auto fr = Mso::WhenAny(futures).Then([&]() noexcept {});

    Mso::FutureWait(fr);
    TestCheckEqual(3, r2);
    finished13.Set();

    while (finishCount != 3) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  TESTMETHOD_REQUIRES_SEH(WhenAny_Vector_Void_Empty) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestCheckCrash(Mso::WhenAny(std::vector<Mso::Future<void>>()));
  }

  TEST_METHOD(WhenAny_Vector_Void_Three_Error) {
    std::atomic<int32_t> finishCount{0};
    Mso::ManualResetEvent finished13;

    int r1 = 0;
    int r3 = 0;
    auto futures = std::vector<Mso::Future<void>>{
        Mso::PostFuture([&]() noexcept {
          finished13.Wait();
          ++finishCount;
          r1 = 1;
        }),
        Mso::MakeFailedFuture<void>(Mso::CancellationErrorProvider().MakeErrorCode(true)),
        Mso::PostFuture([&]() noexcept {
          finished13.Wait();
          ++finishCount;
          r3 = 5;
        })};

    auto fr = Mso::WhenAny(futures).Then([&](Mso::Maybe<void> && result) noexcept {
      TestCheck(result.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(result.GetError()));
      return 42;
    });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();

    while (finishCount != 2) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
};

} // namespace FutureTests
