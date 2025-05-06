#include "AsynchronousEventBeat.h"

namespace Microsoft::ReactNative {

AsynchronousEventBeat::AsynchronousEventBeat(
    std::shared_ptr<facebook::react::EventBeat::OwnerBox> const ownerBox,
    const winrt::Microsoft::ReactNative::ReactContext &context,
    std::shared_ptr<facebook::react::RuntimeScheduler> runtimeScheduler)
    : EventBeat(ownerBox, *runtimeScheduler), m_context(context) {}

void AsynchronousEventBeat::request() const {
  bool alreadyRequested = isEventBeatRequested_;
  EventBeat::request();
  if (!alreadyRequested) {
    auto uiDispatcher = m_context.UIDispatcher();
    if (uiDispatcher.HasThreadAccess()) {
      induce();
    } else {
      uiDispatcher.Post([this, ownerBox = ownerBox_]() {
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
