// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "eventWaitHandle/eventWaitHandle.h"
#include "object/refCountedObject.h"
#include "queueService.h"
#include "taskQueue.h"
#include "winrt/Windows.ApplicationModel.Core.h"
#include "winrt/Windows.UI.Core.h"

using namespace winrt;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;

namespace Mso {

struct MainUISchdulerWinRT;

//! TaskDispatchedHandler is a DispatchedHandler delegate that we pass to CoreDispatcher.
//! We use custom ref counting to avoid extra memory allocations and to handle reference to DispatchTask.
struct TaskDispatchedHandler final : impl::abi_t<DispatchedHandler> {
  TaskDispatchedHandler(MainUISchdulerWinRT *scheduler) noexcept;
  int32_t __stdcall QueryInterface(guid const &id, void **result) noexcept final;
  uint32_t __stdcall AddRef() noexcept final;
  uint32_t __stdcall Release() noexcept final;
  int32_t __stdcall Invoke() noexcept final;

 private:
  MainUISchdulerWinRT *m_scheduler;
};

struct MainUISchdulerWinRT : Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IDispatchQueueScheduler> {
  MainUISchdulerWinRT() noexcept;
  ~MainUISchdulerWinRT() noexcept override;

  uint32_t AddHandlerRef() noexcept;
  uint32_t ReleaseHandlerRef() noexcept;

  DispatchedHandler MakeDispatchedHandler() noexcept;
  bool TryTakeTask(Mso::CntPtr<IDispatchQueueService> &queue, DispatchTask &task) noexcept;

 public: // IDispatchQueueScheduler
  void IntializeScheduler(Mso::WeakPtr<IDispatchQueueService> &&queue) noexcept override;
  bool HasThreadAccess() noexcept override;
  bool IsSerial() noexcept override;
  void Post() noexcept override;
  void Shutdown() noexcept override;
  void AwaitTermination() noexcept override;

 private:
  struct CleanupContext {
    CleanupContext(MainUISchdulerWinRT *scheduler) noexcept;
    ~CleanupContext() noexcept;
    void CheckShutdown() noexcept;
    void CheckTermination() noexcept;

   private:
    MainUISchdulerWinRT *m_scheduler;
    bool m_isShutdown{false};
    bool m_isTerminated{false};
  };

 private:
  CoreDispatcher m_coreDispatcher{nullptr};
  TaskDispatchedHandler m_dispatchedHandler{this};
  ManualResetEvent m_terminationEvent;
  ThreadMutex m_mutex;
  Mso::WeakPtr<IDispatchQueueService> m_queue;
  Mso::CntPtr<MainUISchdulerWinRT> m_self;
  uint32_t m_handlerRefCount{0};
  uint32_t m_taskCount{0};
  bool m_isShutdown{false};
};

//=============================================================================
// TaskDispatchedHandler implementation
//=============================================================================

TaskDispatchedHandler::TaskDispatchedHandler(MainUISchdulerWinRT *scheduler) noexcept : m_scheduler{scheduler} {}

int32_t __stdcall TaskDispatchedHandler::QueryInterface(guid const &id, void **result) noexcept {
  if (is_guid_of<DispatchedHandler>(id) || is_guid_of<Windows::Foundation::IUnknown>(id) ||
      is_guid_of<IAgileObject>(id)) {
    *result = static_cast<impl::abi_t<DispatchedHandler> *>(this);
    AddRef();
    return impl::error_ok;
  }

  if (is_guid_of<IMarshal>(id)) {
    return make_marshaler(this, result);
  }

  *result = nullptr;
  return impl::error_no_interface;
}

uint32_t __stdcall TaskDispatchedHandler::AddRef() noexcept {
  return m_scheduler->AddHandlerRef();
}

uint32_t __stdcall TaskDispatchedHandler::Release() noexcept {
  return m_scheduler->ReleaseHandlerRef();
}

int32_t __stdcall TaskDispatchedHandler::Invoke() noexcept {
  Mso::CntPtr<IDispatchQueueService> queue;
  DispatchTask task;
  if (m_scheduler->TryTakeTask(queue, task)) {
    queue->InvokeTask(std::move(task), std::chrono::steady_clock::now() + std::chrono::milliseconds(1000 / 60));
  }

  return impl::error_ok;
}

//=============================================================================
// MainUISchdulerWinRT implementation
//=============================================================================

MainUISchdulerWinRT::MainUISchdulerWinRT() noexcept
    : m_coreDispatcher{CoreApplication::MainView().CoreWindow().Dispatcher()} {}

MainUISchdulerWinRT::~MainUISchdulerWinRT() noexcept {
  AwaitTermination();
}

uint32_t MainUISchdulerWinRT::AddHandlerRef() noexcept {
  std::lock_guard lock{m_mutex};
  return ++m_handlerRefCount;
}

uint32_t MainUISchdulerWinRT::ReleaseHandlerRef() noexcept {
  Mso::CntPtr<MainUISchdulerWinRT> self;
  CleanupContext context{this};

  {
    std::lock_guard lock{m_mutex};
    VerifyElseCrashSz(m_handlerRefCount, "Ref count cannot be negative");

    uint32_t refCount = --m_handlerRefCount;
    if (refCount == 0) {
      self = std::move(m_self); // Release pointer to self outside of lock.
      context.CheckShutdown();
      context.CheckTermination();
    }

    return refCount;
  }
}

bool MainUISchdulerWinRT::TryTakeTask(Mso::CntPtr<IDispatchQueueService> &queue, DispatchTask &task) noexcept {
  {
    std::lock_guard lock{m_mutex};
    VerifyElseCrashSz(m_taskCount, "Task count cannot be negative");
    --m_taskCount;
  }

  if (queue = m_queue.GetStrongPtr()) {
    return queue->TryDequeTask(task);
  }

  return false;
}

DispatchedHandler MainUISchdulerWinRT::MakeDispatchedHandler() noexcept {
  VerifyElseCrash(m_mutex.IsLockedByMe());

  if (m_handlerRefCount == 0) {
    m_self = this; // Keep reference to self while CoreDispatcher owns DispatchedHandler.
  }

  ++m_handlerRefCount;
  return {static_cast<void *>(&m_dispatchedHandler), take_ownership_from_abi};
}

void MainUISchdulerWinRT::IntializeScheduler(Mso::WeakPtr<IDispatchQueueService> &&queue) noexcept {
  m_queue = std::move(queue);
}

bool MainUISchdulerWinRT::HasThreadAccess() noexcept {
  return m_coreDispatcher.HasThreadAccess();
}

bool MainUISchdulerWinRT::IsSerial() noexcept {
  return true;
}

void MainUISchdulerWinRT::Post() noexcept {
  DispatchedHandler handler;
  {
    std::lock_guard lock{m_mutex};
    if (!m_isShutdown) {
      ++m_taskCount;
      handler = MakeDispatchedHandler();
    }
  }

  if (handler) {
    m_coreDispatcher.RunAsync(CoreDispatcherPriority::Normal, std::move(handler));
  }
}

void MainUISchdulerWinRT::Shutdown() noexcept {
  CleanupContext context{this};
  {
    std::lock_guard lock{m_mutex};
    m_isShutdown = true;
    context.CheckTermination();
  }
}

void MainUISchdulerWinRT::AwaitTermination() noexcept {
  Shutdown();
  m_terminationEvent.Wait();
}

//=============================================================================
// MainUISchdulerWinRT::CleanupContext implementation
//=============================================================================

MainUISchdulerWinRT::CleanupContext::CleanupContext(MainUISchdulerWinRT *scheduler) noexcept : m_scheduler{scheduler} {}

MainUISchdulerWinRT::CleanupContext::~CleanupContext() noexcept {
  if (m_isTerminated) {
    m_scheduler->m_terminationEvent.Set();
  }

  if (m_isShutdown) {
    if (auto queue = m_scheduler->m_queue.GetStrongPtr()) {
      queue->Shutdown(PendingTaskAction::Cancel);
    }
  }
}

void MainUISchdulerWinRT::CleanupContext::CheckShutdown() noexcept {
  // See if core dispatcher released all handlers without invoking them.
  if (m_scheduler->m_taskCount != 0 && m_scheduler->m_handlerRefCount == 0) {
    m_isShutdown = true;
    m_scheduler->m_taskCount = 0;
    m_scheduler->m_isShutdown = true;
  }
}

void MainUISchdulerWinRT::CleanupContext::CheckTermination() noexcept {
  m_isTerminated = m_scheduler->m_isShutdown && (m_scheduler->m_handlerRefCount == 0);
}

//=============================================================================
// DispatchQueueStatic::MakeThreadPoolScheduler implementation
//=============================================================================

/*static*/ Mso::CntPtr<IDispatchQueueScheduler> DispatchQueueStatic::MakeMainUIScheduler() noexcept {
  return Mso::Make<MainUISchdulerWinRT, IDispatchQueueScheduler>();
}

} // namespace Mso
