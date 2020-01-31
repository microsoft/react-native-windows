// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <ReactUWP/InstanceFactory.h>
#include "BatchingQueueThread.h"
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
    std::shared_ptr<facebook::react::MessageQueueThread> const &queueThread) noexcept {
  return std::make_shared<BatchingQueueThread>(queueThread);
}

REACTWINDOWS_API_(std::shared_ptr<facebook::react::MessageQueueThread>)
CreateWorkerMessageQueue() {
  return std::make_shared<WorkerMessageQueueThread>();
}

} // namespace react::uwp
