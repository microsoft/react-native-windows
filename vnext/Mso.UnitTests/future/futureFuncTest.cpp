// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "dispatchQueue/dispatchQueue.h"
#include "future/cancellationToken.h"
#include "future/future.h"
#include "future/futureWait.h"
#include "motifCpp/libletAwareMemLeakDetection.h"
#include "testCheck.h"
#include "testExecutor.h"

namespace FutureTests {

TEST_CLASS_EX (FutureFuncTest, LibletAwareMemLeakDetection) {
  // MemoryLeakDetectionHook::TrackPerTest m_trackLeakPerTest;

  TEST_METHOD(PostFutureDefaultExecutor) {
    auto future = Mso::PostFuture([]() noexcept { return 5; });
    TestCheckEqual(5, Mso::FutureWaitAndGetValue(future));
  }

  TEST_METHOD(PostFutureDefaultExecutor_mutable) {
    auto future = Mso::PostFuture([]() mutable noexcept { return 5; });
    TestCheckEqual(5, Mso::FutureWaitAndGetValue(future));
  }

  TEST_METHOD(PostFuture) {
    auto future = Mso::PostFuture(Mso::Executors::Concurrent{}, []() noexcept { return 5; });
    TestCheckEqual(5, Mso::FutureWaitAndGetValue(future));
  }

  TEST_METHOD(PostFuture_mutable) {
    auto future = Mso::PostFuture(Mso::Executors::Concurrent{}, []() mutable noexcept { return 5; });
    TestCheckEqual(5, Mso::FutureWaitAndGetValue(future));
  }

  TEST_METHOD(PostFuture_DispatchQueue) {
    auto future = Mso::PostFuture(Mso::DispatchQueue::ConcurrentQueue(), []() noexcept { return 5; });
    TestCheckEqual(5, Mso::FutureWaitAndGetValue(future));
  }

  TEST_METHOD(PostFuture_DispatchQueue_mutable) {
    auto future = Mso::PostFuture(Mso::DispatchQueue::ConcurrentQueue(), []() mutable noexcept { return 5; });
    TestCheckEqual(5, Mso::FutureWaitAndGetValue(future));
  }

  TEST_METHOD(PostFuture_DispatchQueue_mutable_WaitOnly) {
    auto future = Mso::PostFuture(Mso::DispatchQueue::ConcurrentQueue(), []() mutable noexcept { return 5; });
    Mso::FutureWait(future);
  }

  TEST_METHOD(PostFutureDefaultExecutor_void) {
    auto future = Mso::PostFuture([]() noexcept {});
    Mso::FutureWait(future);
  }

  TEST_METHOD(PostFutureDefaultExecutor_void_mutable) {
    auto future = Mso::PostFuture([]() mutable noexcept {});
    Mso::FutureWait(future);
  }

  TEST_METHOD(PostFuture_void) {
    auto future = Mso::PostFuture(Mso::Executors::Concurrent{}, []() noexcept {});
    Mso::FutureWait(future);
  }

  TEST_METHOD(PostFuture_void_mutable) {
    auto future = Mso::PostFuture(Mso::Executors::Concurrent{}, []() mutable noexcept {});
    Mso::FutureWait(future);
  }

  TEST_METHOD(MakeCompletedFuture) {
    auto future = Mso::MakeCompletedFuture(5);
    TestCheckEqual(5, Mso::FutureWaitAndGetValue(future));
  }

  TEST_METHOD(MakeCompletedFutureValue) {
    auto str = std::wstring(L"Hello");
    auto future = Mso::MakeCompletedFuture(str);
    TestCheckEqual(str, Mso::FutureWaitAndGetValue(future));
  }

  TEST_METHOD(MakeCompletedFutureConst) {
    const auto str = std::wstring(L"Hello");
    auto future = Mso::MakeCompletedFuture(str);
    TestCheckEqual(str, Mso::FutureWaitAndGetValue(future));
  }

  TEST_METHOD(MakeCompletedFutureRvalueRef) {
    auto future = Mso::MakeCompletedFuture(std::wstring(L"Hello"));
    TestCheckEqual(std::wstring(L"Hello"), Mso::FutureWaitAndGetValue(future));
  }

  TEST_METHOD(MakeCompletedFuture_void) {
    auto future = Mso::MakeCompletedFuture();
    Mso::FutureWait(future);
  }

  TEST_METHOD(MakeCompletedFutureEmplaced) {
    auto future = Mso::MakeCompletedFutureEmplaced<std::unique_ptr<int>>(new int(5));
    auto result = Mso::FutureWaitAndGetValue(future);
    TestCheckEqual(5, *result);
  }

  TEST_METHOD(MakeCompletedFutureEmplaced_InitializationList) {
    auto future = Mso::MakeCompletedFutureEmplaced<std::vector<int>>({1, 2, 3, 4});
    auto result = Mso::FutureWaitAndGetValue(future);
    TestCheckEqual(4u, result.size());
    TestCheckEqual(2, result[1]);
  }
// TODO: implement Mso::PotsTimer and Mso::WhenDoneOrTimeout
#if 0
  TEST_METHOD(WhenDoneOrTimeout_TimeOut_int) {
    Mso::ManualResetEvent finished;

    auto future = Mso::PostFuture([&finished]() noexcept {
      finished.Wait();
      return 3;
    });

    auto result = Mso::FutureWait(Mso::WhenDoneOrTimeout(future, std::chrono::milliseconds(20)));
    finished.Set();

    TestCheck(result.IsError());
    TestCheck(result.GetError().As(Mso::Async::TimeoutError()));
  }

	TEST_METHOD(WhenDoneOrTimeout_NoTimeOut_int)
	{
		auto future = Mso::PostFuture([]() noexcept
		{
			return 3;
		});

		auto result = Mso::FutureWait(Mso::WhenDoneOrTimeout(future, std::chrono::milliseconds(100)));
		TestCheckEqual(3, result.GetValue());

		// Wait for timer to finish to make sure that unit test does not show memory leak
		Mso::Promise<void> timerPromise;
		Mso::Async::PostTimer(120, [timerPromise]() noexcept
		{
			timerPromise.SetValue();
		});
		Mso::FutureWait(timerPromise.AsFuture());
	}

	TEST_METHOD(WhenDoneOrTimeout_Shutdown_int)
	{
		// This test applies only to win32!
		if (!Mso::Async::IsBlockingShutdown())
			return;

		Mso::Promise<int> promise;
		auto futureWithTimeout = Mso::WhenDoneOrTimeout(promise.AsFuture(), std::chrono::seconds(10));

		RestartLiblets();

		auto result = Mso::FutureWait(futureWithTimeout);
		TestCheck(result.IsError());
		TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(result.GetError()));

		promise.SetValue(3);
	}

	TEST_METHOD(WhenDoneOrTimeout_TimeOut_void)
	{
		Mso::ManualResetEvent finished;
		Mso::ManualResetEvent futureCompleted;
		int value = 0;
		auto future = Mso::PostFuture([&finished, &futureCompleted, &value]() noexcept
		{
			finished.Wait();
			value = 3;
			futureCompleted.Set();
		});

		auto result = Mso::FutureWait(Mso::WhenDoneOrTimeout(future, std::chrono::milliseconds(20)));
		finished.Set();

		TestCheck(result.IsError());
		TestCheck(result.GetError().As(Mso::Async::TimeoutError()));
		futureCompleted.Wait();
	}

	TEST_METHOD(WhenDoneOrTimeout_NoTimeOut_void)
	{
		int value = 0;
		auto future = Mso::PostFuture([&value]() noexcept
		{
			value = 3;
		});

		Mso::FutureWait(Mso::WhenDoneOrTimeout(future, std::chrono::milliseconds(100)));
		TestCheckEqual(3, value);

		// Wait for timer to finish to make sure that unit test does not show memory leak
		Mso::Promise<void> timerPromise;
		Mso::Async::PostTimer(120, [timerPromise]() noexcept
		{
			timerPromise.SetValue();
		});
		Mso::FutureWait(timerPromise.AsFuture());
	}

	TEST_METHOD(WhenDoneOrTimeout_Shutdown_void)
	{
		// This test applies only to win32!
		if (!Mso::Async::IsBlockingShutdown())
			return;

		Mso::Promise<void> promise;
		auto futureWithTimeout = Mso::WhenDoneOrTimeout(promise.AsFuture(), std::chrono::seconds(10));

		RestartLiblets();

		auto result = Mso::FutureWait(futureWithTimeout);
		TestCheck(result.IsError());
		TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(result.GetError()));

		promise.SetValue();
	}

	TEST_METHOD(WhenDoneOrTimeout_TimeOut_Maybe)
	{
		Mso::HandleHolder finished = CreateEvent(nullptr, false, false, nullptr);
		auto future = Mso::PostFuture([&finished]() noexcept
		{
			WaitForSingleObject(finished, INFINITE);
			return Mso::Maybe<int>(3);
		});

		auto result = Mso::FutureWait(Mso::WhenDoneOrTimeout(future, std::chrono::milliseconds(20)));
		SetEvent(finished);

		TestCheck(result.IsError());
		TestCheck(result.GetError().As(Mso::Async::TimeoutError()));
	}

	TEST_METHOD(WhenDoneOrTimeout_NoTimeOut_Maybe)
	{
		auto future = Mso::PostFuture([]() noexcept
		{
			return Mso::Maybe<int>(3);
		});

		auto result = Mso::FutureWait(Mso::WhenDoneOrTimeout(future, std::chrono::milliseconds(100)));
		TestCheckEqual(3, result.GetValue());

		// Wait for timer to finish to make sure that unit test does not show memory leak
		Mso::Promise<void> timerPromise;
		Mso::Async::PostTimer(120, [timerPromise]() noexcept
		{
			timerPromise.SetValue();
		});
		Mso::FutureWait(timerPromise.AsFuture());
	}

	TEST_METHOD(WhenDoneOrTimeout_NoTimeOut_Maybe_Error)
	{
		auto future = Mso::PostFuture([]() noexcept
		{
			return Mso::CancellationErrorProvider().MakeMaybe<int>();
		});

		auto result = Mso::FutureWait(Mso::WhenDoneOrTimeout(future, std::chrono::milliseconds(100)));
		TestCheck(result.IsError());
		TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(result.GetError()));

		// Wait for timer to finish to make sure that unit test does not show memory leak
		Mso::Promise<void> timerPromise;
		Mso::Async::PostTimer(120, [timerPromise]() noexcept
		{
			timerPromise.SetValue();
		});
		Mso::FutureWait(timerPromise.AsFuture());
	}

	TEST_METHOD(WhenDoneOrTimeout_Shutdown_Maybe)
	{
		// This test applies only to win32!
		if (!Mso::Async::IsBlockingShutdown())
			return;

		Mso::Promise<int> promise;
		auto futureWithTimeout = Mso::WhenDoneOrTimeout(promise.AsFuture(), std::chrono::seconds(10));

		RestartLiblets();

		auto result = Mso::FutureWait(futureWithTimeout);
		TestCheck(result.IsError());
		TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(result.GetError()));

		promise.SetValue(3);
	}
#endif
};

} // namespace FutureTests
