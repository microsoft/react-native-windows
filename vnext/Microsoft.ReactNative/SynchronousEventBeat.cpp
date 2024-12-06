// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "SynchronousEventBeat.h"

namespace Microsoft::ReactNative {

SynchronousEventBeat::SynchronousEventBeat(
    std::shared_ptr<facebook::react::EventBeat::OwnerBox> const ownerBox,
    const winrt::Microsoft::ReactNative::ReactContext &context,
    facebook::react::RuntimeExecutor runtimeExecutor,
    std::shared_ptr<facebook::react::RuntimeScheduler> runtimeScheduler)
    : EventBeat(ownerBox, *runtimeScheduler),
      m_context(context),
      m_runtimeExecutor(runtimeExecutor),
      m_runtimeScheduler(std::move(runtimeScheduler)) {}

void SynchronousEventBeat::beat(facebook::jsi::Runtime &runtime) const {
  if (!this->isRequested_) {
    return;
  }
  isRequested_ = false;
  if (beatCallback_) {
    beatCallback_(runtime);
  }
}

void SynchronousEventBeat::induce() const {
  if (!isRequested_) {
    return;
  }

  if (m_context.JSDispatcher().HasThreadAccess()) {
    lockExecutorAndBeat();
  }
}

void SynchronousEventBeat::lockExecutorAndBeat() const {
  if (!this->isRequested_) {
    return;
  }

  if (m_runtimeScheduler) {
    m_runtimeScheduler->executeNowOnTheSameThread([this](facebook::jsi::Runtime &runtime) { beat(runtime); });
  } else {
    facebook::react::executeSynchronouslyOnSameThread_CAN_DEADLOCK(
        m_runtimeExecutor, [this](facebook::jsi::Runtime &runtime) { beat(runtime); });
  }
}

} // namespace Microsoft::ReactNative
