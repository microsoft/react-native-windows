#include <NativeModules.h>
#include <ReactCommon/RuntimeExecutor.h>
#include <react/renderer/core/EventBeat.h>

namespace Microsoft::ReactNative {

class AsynchronousEventBeat final : public facebook::react::EventBeat {
 public:
  AsynchronousEventBeat(
      facebook::react::EventBeat::SharedOwnerBox const &ownerBox,
      const winrt::Microsoft::ReactNative::ReactContext &context,
      facebook::react::RuntimeExecutor runtimeExecutor);

  void induce() const override;
  void request() const override;

 private:
  mutable std::atomic<bool> m_isBeatCallbackScheduled{false};
  winrt::Microsoft::ReactNative::ReactContext m_context;
  facebook::react::RuntimeExecutor m_runtimeExecutor;
};

} // namespace Microsoft::ReactNative
