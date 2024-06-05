#include "AsynchronousEventBeat.h"

namespace Microsoft::ReactNative {

AsynchronousEventBeat::AsynchronousEventBeat(
    facebook::react::EventBeat::SharedOwnerBox const &ownerBox,
    const winrt::Microsoft::ReactNative::ReactContext &context,
    facebook::react::RuntimeExecutor runtimeExecutor)
    : EventBeat(ownerBox), m_context(context), m_runtimeExecutor(runtimeExecutor) {}

void AsynchronousEventBeat::induce() const {
  if (!isRequested_ || m_isBeatCallbackScheduled) {
    isRequested_ = false;
    return;
  }
  isRequested_ = false;
  m_isBeatCallbackScheduled = true;

  m_runtimeExecutor([this, ownerBox = ownerBox_](facebook::jsi::Runtime &runtime) {
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
