// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "SynchronousEventBeat.h"

namespace Microsoft::ReactNative {

SynchronousEventBeat::SynchronousEventBeat(
    facebook::react::EventBeat::SharedOwnerBox const &ownerBox,
    const winrt::Microsoft::ReactNative::ReactContext &context,
    facebook::react::RuntimeExecutor runtimeExecutor,
    std::shared_ptr<facebook::react::RuntimeScheduler> runtimeScheduler)
    : EventBeat(ownerBox),
      m_context(context),
      m_runtimeExecutor(runtimeExecutor),
      m_runtimeScheduler(std::move(runtimeScheduler)) {}

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
