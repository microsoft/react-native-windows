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

  void request() const override;

 private:
  winrt::Microsoft::ReactNative::ReactContext m_context;
};

} // namespace Microsoft::ReactNative
