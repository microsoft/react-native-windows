#include <NativeModules.h>
#include <react/renderer/core/EventBeat.h>
#include <react/renderer/runtimescheduler/RuntimeScheduler.h>

namespace Microsoft::ReactNative {

class AsynchronousEventBeat final : public facebook::react::EventBeat {
 public:
  AsynchronousEventBeat(
      std::shared_ptr<facebook::react::EventBeat::OwnerBox> const ownerBox,
      const winrt::Microsoft::ReactNative::ReactContext &context,
      std::shared_ptr<facebook::react::RuntimeScheduler> runtimeScheduler);

  void induce() const;
  void request() const override;

 private:
  mutable std::atomic<bool> m_isBeatCallbackScheduled{false};
  winrt::Microsoft::ReactNative::ReactContext m_context;
  std::shared_ptr<facebook::react::RuntimeScheduler> m_runtimeScheduler;
};

} // namespace Microsoft::ReactNative
