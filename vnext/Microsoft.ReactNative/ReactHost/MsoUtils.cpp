// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "MsoUtils.h"
#include "object/refCountedObject.h"

namespace Mso {
namespace {

// A special error provider to indicate that WhenAllCompletedState slot is not initialized yet.
MSO_CLASS_GUID(CompletionGuardErrorProviderGuid, "e7d1526a-efcf-4e62-a906-84fdcd4b6371")
ErrorProvider<bool, CompletionGuardErrorProviderGuid> s_completionGuardErrorProvider;

struct WhenAllCompletedState : Mso::RefCountedObjectNoVTable<WhenAllCompletedState> {
 public:
  WhenAllCompletedState(size_t taskCount) noexcept
      : m_taskResults{taskCount, s_completionGuardErrorProvider.MakeMaybe<void>()}, m_waitCount{taskCount} {
    if (m_taskResults.empty()) {
      m_result.SetValue();
    }
  }

  Mso::Future<void> AsFuture() const noexcept {
    return m_result.AsFuture();
  }

  void SetResult(size_t index, Mso::Maybe<void> &&result) noexcept {
    VerifyElseCrashSzTag(
        index < m_taskResults.size(), "Index must be less than m_taskResults.size()", 0x0281e3d4 /* tag_c64pu */);
    auto &resultSlot = m_taskResults[index];
    VerifyElseCrashSzTag(
        resultSlot.IsError() && resultSlot.GetError().As(s_completionGuardErrorProvider),
        "The result slot must be unassigned",
        0x0281e3d5 /* tag_c64pv */);
    resultSlot = std::move(result);
    if (--m_waitCount == 0) {
      // All tasks are completed.
      // If we have an error, then propage only the first error in the list.
      for (auto &taskResult : m_taskResults) {
        if (taskResult.IsError()) {
          m_result.SetMaybe(std::move(taskResult));
          return;
        }
      }

      m_result.SetValue();
    }
  }

 private:
  Mso::Promise<void> m_result;
  std::vector<Mso::Maybe<void>> m_taskResults;
  std::atomic<size_t> m_waitCount{0};
};

} // namespace

Mso::Future<void> WhenAllCompleted(const std::vector<Mso::Future<void>> &futures) noexcept {
  auto whenAllCompleted = Mso::Make<WhenAllCompletedState>(futures.size());
  Mso::ForEach(futures, [whenAllCompleted](const Mso::Future<void> &future, size_t index) noexcept {
    future.Then<Mso::Executors::Inline>([ whenAllCompleted, index ](Mso::Maybe<void> && result) mutable noexcept {
      whenAllCompleted->SetResult(index, std::move(result));
    });
  });
  return whenAllCompleted->AsFuture();
}

Mso::Future<void> MakeCanceledFuture() noexcept {
  return Mso::MakeFailedFuture<void>(Mso::CancellationErrorProvider().MakeErrorCode(true));
}

void SetPromiseValue(Mso::Promise<void> &&promise, const Mso::Future<void> &valueSource) noexcept {
  if (promise) {
    valueSource.Then<Mso::Executors::Inline>([promise = std::move(promise)](Mso::Maybe<void> && value) noexcept {
      promise.SetValue(std::move(value));
    });
  }
}

} // namespace Mso
