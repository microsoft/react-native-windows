// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <NativeModules.h>
#include <ReactCommon/RuntimeExecutor.h>
#include <react/renderer/core/EventBeat.h>
#include <react/renderer/runtimescheduler/RuntimeScheduler.h>

namespace Microsoft::ReactNative {

class SynchronousEventBeat final : public facebook::react::EventBeat {
 public:
  SynchronousEventBeat(
      facebook::react::EventBeat::SharedOwnerBox const &ownerBox,
      const winrt::Microsoft::ReactNative::ReactContext &context,
      facebook::react::RuntimeExecutor runtimeExecutor,
      std::shared_ptr<facebook::react::RuntimeScheduler> runtimeScheduler);

  void induce() const override;

  void lockExecutorAndBeat() const;

 private:
  winrt::Microsoft::ReactNative::ReactContext m_context;
  facebook::react::RuntimeExecutor m_runtimeExecutor;
  std::shared_ptr<facebook::react::RuntimeScheduler> m_runtimeScheduler;
};

} // namespace Microsoft::ReactNative
