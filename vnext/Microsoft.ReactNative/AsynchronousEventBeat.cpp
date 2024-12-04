#include "AsynchronousEventBeat.h"

namespace Microsoft::ReactNative {

AsynchronousEventBeat::AsynchronousEventBeat(
    std::shared_ptr<facebook::react::EventBeat::OwnerBox> const ownerBox,
    const winrt::Microsoft::ReactNative::ReactContext &context,
    std::shared_ptr<facebook::react::RuntimeScheduler> runtimeScheduler)
    : EventBeat(ownerBox, *runtimeScheduler), m_context(context), m_runtimeScheduler(std::move(runtimeScheduler)) {}

void AsynchronousEventBeat::induce() const {
  if (!isRequested_ || m_isBeatCallbackScheduled) {
    isRequested_ = false;
    return;
  }
  isRequested_ = false;
  m_isBeatCallbackScheduled = true;

  facebook::react::RuntimeScheduler &schedulerRef = *m_runtimeScheduler.get();
  schedulerRef.scheduleWork([this, ownerBox = ownerBox_](facebook::jsi::Runtime &runtime) {
    auto owner = ownerBox->owner.lock();
    if (!owner) {
      return;
    }

    m_isBeatCallbackScheduled = false;
    if (beatCallback_) {
      beatCallback_(runtime);
    }
  });
}

void AsynchronousEventBeat::request() const {
  bool alreadyRequested = isRequested_.exchange(true);
  if (!alreadyRequested) {
    if (m_context.UIDispatcher().HasThreadAccess()) {
      induce();
    } else {
      m_context.UIDispatcher().Post([this, ownerBox = ownerBox_]() {
        auto owner = ownerBox->owner.lock();
        if (!owner) {
          return;
        }
        induce();
      });
    }
  }
}

} // namespace Microsoft::ReactNative
