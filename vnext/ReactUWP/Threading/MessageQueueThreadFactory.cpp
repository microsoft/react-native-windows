// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <ReactUWP/InstanceFactory.h>
#include "BatchingUIMessageQueueThread.h"
#include "JSQueueThread.h"
#include "UIMessageQueueThread.h"
#include "WorkerMessageQueueThread.h"

using namespace winrt;
using namespace Windows::UI::Core;

namespace react::uwp {

std::shared_ptr<facebook::react::MessageQueueThread> MakeJSQueueThread() noexcept {
  return CreateAndStartJSQueueThread();
}

std::shared_ptr<facebook::react::MessageQueueThread> MakeUIQueueThread() noexcept {
  return std::make_shared<UIMessageQueueThread>(CoreWindow::GetForCurrentThread().Dispatcher());
}

std::shared_ptr<facebook::react::MessageQueueThread> MakeSerialQueueThread() noexcept {
  return std::make_shared<WorkerMessageQueueThread>();
}

std::shared_ptr<facebook::react::BatchingMessageQueueThread> MakeBatchingQueueThread(
    winrt::Windows::UI::Core::CoreDispatcher const &dispatcher) noexcept {
  return std::make_shared<BatchingUIMessageQueueThread>(dispatcher);
}

} // namespace react::uwp
