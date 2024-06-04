// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "eventWaitHandle/eventWaitHandle.h"
#include "object/refCountedObject.h"
#include "queueService.h"
#include "taskQueue.h"
#include "winrt/Windows.Foundation.h"

using namespace winrt;
using namespace Windows::Foundation;
#include "winrt/Windows.System.h"

#ifdef USE_WINUI3
#include "winrt/Microsoft.UI.Dispatching.h"
#endif

namespace Mso {

namespace {

using IInspectable = winrt::Windows::Foundation::IInspectable;

// TODO: consider to move it into its own liblet
template <class TKey, class TValue>
struct ThreadSafeMap {
  ThreadSafeMap(std::recursive_mutex &mutex) noexcept : m_mutex{mutex} {}

  std::optional<TValue> Get(TKey const &key) noexcept {
    std::scoped_lock lock{m_mutex};
    auto it = m_map.find(key);
    if (it != m_map.end()) {
      return std::optional<TValue>{it->second};
    } else {
      return std::optional<TValue>{};
    }
  }

  void Set(TKey const &key, TValue &&value) noexcept {
    TValue oldValue; // to destroy outside of lock
    {
      std::scoped_lock lock{m_mutex};
      auto &valueRef = m_map[key];
      oldValue = std::move(valueRef);
      valueRef = std::move(value);
    }
  }

  void Remove(TKey const &key) noexcept {
    TValue value; // to destroy outside of lock
    {
      std::scoped_lock lock{m_mutex};
      auto it = m_map.find(key);
      if (it != m_map.end()) {
        value = std::move(it->second);
        m_map.erase(it);
      }
    }
  }

 private:
  std::recursive_mutex &m_mutex;
  std::map<TKey, TValue, std::less<>> m_map;
};

} // namespace

template <typename TDispatcherTraits>
struct UISchedulerWinRT;

template <typename TDispatcherTraits>
struct DispatcherTraits {};

template <typename TDispatcherTraits>
struct TaskDispatcherHandler;

struct WindowsTypeTag;
using WindowsTaskDispatcherHandler = TaskDispatcherHandler<DispatcherTraits<WindowsTypeTag>>;

template <>
struct DispatcherTraits<WindowsTypeTag> {
  using DispatcherQueueHandler = Windows::System::DispatcherQueueHandler;
  using DispatcherQueue = Windows::System::DispatcherQueue;
  using DispatcherQueue_ShutdownCompleted_revoker = Windows::System::DispatcherQueue::ShutdownCompleted_revoker;
  using TaskDispatcherHandler = WindowsTaskDispatcherHandler;
};
using WindowsDispatcherTraits = DispatcherTraits<WindowsTypeTag>;

#ifdef USE_WINUI3
struct MicrosoftTypeTag;
using MicrosoftTaskDispatcherHandler = TaskDispatcherHandler<DispatcherTraits<MicrosoftTypeTag>>;

template <>
struct DispatcherTraits<MicrosoftTypeTag> {
  using DispatcherQueueHandler = Microsoft::UI::Dispatching::DispatcherQueueHandler;
  using DispatcherQueue = Microsoft::UI::Dispatching::DispatcherQueue;
  using DispatcherQueue_ShutdownCompleted_revoker =
      Microsoft::UI::Dispatching::DispatcherQueue::ShutdownCompleted_revoker;
  using TaskDispatcherHandler = MicrosoftTaskDispatcherHandler;
};
using MicrosoftDispatcherTraits = DispatcherTraits<MicrosoftTypeTag>;

#endif

//! TaskDispatcherHandler is a DispatcherQueueHandler delegate that we pass to DispatcherQueue.
//! We use custom ref counting to avoid extra memory allocations and to handle reference to DispatchTask.
template <typename TDispatcherTraits>
struct TaskDispatcherHandler final : impl::abi_t<typename TDispatcherTraits::DispatcherQueueHandler> {
  TaskDispatcherHandler(UISchedulerWinRT<TDispatcherTraits> *scheduler) noexcept;
  int32_t __stdcall QueryInterface(guid const &id, void **result) noexcept final;
  uint32_t __stdcall AddRef() noexcept final;
  uint32_t __stdcall Release() noexcept final;
  int32_t __stdcall Invoke() noexcept final;

 private:
  UISchedulerWinRT<TDispatcherTraits> *m_scheduler;
};

using WindowsTaskDispatcherHandler = TaskDispatcherHandler<DispatcherTraits<WindowsTypeTag>>;

template <typename TDispatcherTraits>
struct UISchedulerWinRT : Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IDispatchQueueScheduler> {
  UISchedulerWinRT(typename TDispatcherTraits::DispatcherQueue &&dispatcher) noexcept;
  ~UISchedulerWinRT() noexcept override;

  uint32_t AddHandlerRef() noexcept;
  uint32_t ReleaseHandlerRef() noexcept;

  typename TDispatcherTraits::DispatcherQueueHandler MakeDispatcherQueueHandler() noexcept;
  bool TryTakeTask(Mso::CntPtr<IDispatchQueueService> &queue, DispatchTask &task) noexcept;

  static DispatchQueue GetOrCreateUIThreadQueue() noexcept;
  using DispatchQueueRegistry = ThreadSafeMap<std::thread::id, Mso::WeakPtr<IDispatchQueueService>>;
  static DispatchQueueRegistry &GetDispatchQueueRegistry() noexcept;

 public: // IDispatchQueueScheduler
  void InitializeScheduler(Mso::WeakPtr<IDispatchQueueService> &&queue) noexcept override;
  bool HasThreadAccess() noexcept override;
  bool IsSerial() noexcept override;
  void Post() noexcept override;
  void Shutdown() noexcept override;
  void AwaitTermination() noexcept override;

 private:
  friend DispatchQueueStatic;

  struct CleanupContext {
    CleanupContext(UISchedulerWinRT *scheduler) noexcept;
    ~CleanupContext() noexcept;
    void CheckShutdown() noexcept;
    void CheckTermination() noexcept;

   private:
    UISchedulerWinRT *m_scheduler;
    bool m_isShutdown{false};
    bool m_isTerminated{false};
  };

 private:
  typename TDispatcherTraits::DispatcherQueue m_dispatcher{nullptr};
  typename TDispatcherTraits::TaskDispatcherHandler m_dispatcherHandler{this};
  ManualResetEvent m_terminationEvent;
  ThreadMutex m_mutex;
  Mso::WeakPtr<IDispatchQueueService> m_queue;
  Mso::CntPtr<UISchedulerWinRT> m_self;
  uint32_t m_handlerRefCount{0};
  uint32_t m_taskCount{0};
  bool m_isShutdown{false};
  std::thread::id m_threadId{std::this_thread::get_id()};
  typename TDispatcherTraits::DispatcherQueue_ShutdownCompleted_revoker m_shutdownCompletedRevoker;
};

//=============================================================================
// TaskDispatcherHandler implementation
//=============================================================================

template <typename TDispatcherTraits>
TaskDispatcherHandler<TDispatcherTraits>::TaskDispatcherHandler(UISchedulerWinRT<TDispatcherTraits> *scheduler) noexcept
    : m_scheduler{scheduler} {}

template <typename TDispatcherTraits>
int32_t __stdcall TaskDispatcherHandler<TDispatcherTraits>::QueryInterface(guid const &id, void **result) noexcept {
  if (is_guid_of<typename TDispatcherTraits::DispatcherQueueHandler>(id) ||
      is_guid_of<Windows::Foundation::IUnknown>(id) || is_guid_of<IAgileObject>(id)) {
    *result = static_cast<impl::abi_t<typename TDispatcherTraits::DispatcherQueueHandler> *>(this);
    AddRef();
    return impl::error_ok;
  }

  if (is_guid_of<IMarshal>(id)) {
    return make_marshaler(this, result);
  }

  *result = nullptr;
  return impl::error_no_interface;
}

template <typename TDispatcherTraits>
uint32_t __stdcall TaskDispatcherHandler<TDispatcherTraits>::AddRef() noexcept {
  return m_scheduler->AddHandlerRef();
}

template <typename TDispatcherTraits>
uint32_t __stdcall TaskDispatcherHandler<TDispatcherTraits>::Release() noexcept {
  return m_scheduler->ReleaseHandlerRef();
}

template <typename TDispatcherTraits>
int32_t __stdcall TaskDispatcherHandler<TDispatcherTraits>::Invoke() noexcept {
  Mso::CntPtr<IDispatchQueueService> queue;
  DispatchTask task;
  if (m_scheduler->TryTakeTask(queue, task)) {
    queue->InvokeTask(std::move(task), std::chrono::steady_clock::now() + std::chrono::milliseconds(1000 / 60));
  }

  return impl::error_ok;
}

//=============================================================================
// UISchedulerWinRT implementation
//=============================================================================

template <typename TDispatcherTraits>
UISchedulerWinRT<TDispatcherTraits>::UISchedulerWinRT(typename TDispatcherTraits::DispatcherQueue &&dispatcher) noexcept
    : m_dispatcher{std::move(dispatcher)} {
  m_shutdownCompletedRevoker = m_dispatcher.ShutdownCompleted(
      winrt::auto_revoke, [](typename TDispatcherTraits::DispatcherQueue const &, IInspectable const &) noexcept {
        GetDispatchQueueRegistry().Remove(std::this_thread::get_id());
      });
}

template <typename TDispatcherTraits>
UISchedulerWinRT<TDispatcherTraits>::~UISchedulerWinRT() noexcept {
  GetDispatchQueueRegistry().Remove(m_threadId);
  AwaitTermination();
}

template <typename TDispatcherTraits>
uint32_t UISchedulerWinRT<TDispatcherTraits>::AddHandlerRef() noexcept {
  std::lock_guard lock{m_mutex};
  return ++m_handlerRefCount;
}

template <typename TDispatcherTraits>
uint32_t UISchedulerWinRT<TDispatcherTraits>::ReleaseHandlerRef() noexcept {
  Mso::CntPtr<UISchedulerWinRT> self;
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

template <typename TDispatcherTraits>
bool UISchedulerWinRT<TDispatcherTraits>::TryTakeTask(
    Mso::CntPtr<IDispatchQueueService> &queue,
    DispatchTask &task) noexcept {
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

template <typename TDispatcherTraits>
typename TDispatcherTraits::DispatcherQueueHandler
UISchedulerWinRT<TDispatcherTraits>::MakeDispatcherQueueHandler() noexcept {
  VerifyElseCrash(m_mutex.IsLockedByMe());

  if (m_handlerRefCount == 0) {
    m_self = this; // Keep reference to self while DispatcherQueue owns DispatcherQueueHandler.
  }

  ++m_handlerRefCount;
  return {static_cast<void *>(&m_dispatcherHandler), take_ownership_from_abi};
}

template <typename TDispatcherTraits>
void UISchedulerWinRT<TDispatcherTraits>::InitializeScheduler(Mso::WeakPtr<IDispatchQueueService> &&queue) noexcept {
  m_queue = std::move(queue);
}

template <typename TDispatcherTraits>
bool UISchedulerWinRT<TDispatcherTraits>::HasThreadAccess() noexcept {
  // m_dispatcher.HasThreadAccess() is implemented only in Windows 19H1.
  // We must use an alternative implementation.
  return m_threadId == std::this_thread::get_id();
}

template <typename TDispatcherTraits>
bool UISchedulerWinRT<TDispatcherTraits>::IsSerial() noexcept {
  return true;
}

template <typename TDispatcherTraits>
void UISchedulerWinRT<TDispatcherTraits>::Post() noexcept {
  typename TDispatcherTraits::DispatcherQueueHandler handler;
  {
    std::lock_guard lock{m_mutex};
    if (!m_isShutdown) {
      ++m_taskCount;
      handler = MakeDispatcherQueueHandler();
    }
  }

  if (handler) {
    m_dispatcher.TryEnqueue(handler);
  }
}

template <typename TDispatcherTraits>
void UISchedulerWinRT<TDispatcherTraits>::Shutdown() noexcept {
  CleanupContext context{this};
  {
    std::lock_guard lock{m_mutex};
    m_isShutdown = true;
    context.CheckTermination();
  }
}

template <typename TDispatcherTraits>
void UISchedulerWinRT<TDispatcherTraits>::AwaitTermination() noexcept {
  Shutdown();
  if (m_threadId != std::this_thread::get_id()) {
    m_terminationEvent.Wait();
  }
}

template <typename TDispatcherTraits>
/*static*/ DispatchQueue UISchedulerWinRT<TDispatcherTraits>::GetOrCreateUIThreadQueue() noexcept {
  std::thread::id threadId{std::this_thread::get_id()};
  std::optional<Mso::WeakPtr<IDispatchQueueService>> weakQueue = GetDispatchQueueRegistry().Get(threadId);
  DispatchQueue queue{weakQueue.value_or(nullptr).GetStrongPtr()};
  if (queue) {
    return queue;
  }

  decltype(TDispatcherTraits::DispatcherQueue::GetForCurrentThread()) dispatcher{nullptr};
  try {
    dispatcher = TDispatcherTraits::DispatcherQueue::GetForCurrentThread();
  } catch (winrt::hresult_error const &) {
  }

  if (!dispatcher) {
    return queue;
  }

  queue =
      DispatchQueue(Mso::Make<QueueService, IDispatchQueueService>(Mso::Make<UISchedulerWinRT>(std::move(dispatcher))));
  GetDispatchQueueRegistry().Set(threadId, Mso::WeakPtr{*GetRawState(queue)});
  return queue;
}

template <typename TDispatcherTraits>
/*static*/ typename UISchedulerWinRT<TDispatcherTraits>::DispatchQueueRegistry &
UISchedulerWinRT<TDispatcherTraits>::GetDispatchQueueRegistry() noexcept {
  static std::recursive_mutex mutex;
  static DispatchQueueRegistry registry{mutex};
  return registry;
}

//=============================================================================
// UISchedulerWinRT::CleanupContext implementation
//=============================================================================

template <typename TDispatcherTraits>
UISchedulerWinRT<TDispatcherTraits>::CleanupContext::CleanupContext(
    UISchedulerWinRT<TDispatcherTraits> *scheduler) noexcept
    : m_scheduler{scheduler} {}

template <typename TDispatcherTraits>
UISchedulerWinRT<TDispatcherTraits>::CleanupContext::~CleanupContext() noexcept {
  if (m_isTerminated) {
    m_scheduler->m_terminationEvent.Set();
  }

  if (m_isShutdown) {
    if (auto queue = m_scheduler->m_queue.GetStrongPtr()) {
      queue->Shutdown(PendingTaskAction::Cancel);
    }
  }
}

template <typename TDispatcherTraits>
void UISchedulerWinRT<TDispatcherTraits>::CleanupContext::CheckShutdown() noexcept {
  // See if dispatcher queue released all handlers without invoking them.
  if (m_scheduler->m_taskCount != 0 && m_scheduler->m_handlerRefCount == 0) {
    m_isShutdown = true;
    m_scheduler->m_taskCount = 0;
    m_scheduler->m_isShutdown = true;
  }
}

template <typename TDispatcherTraits>
void UISchedulerWinRT<TDispatcherTraits>::CleanupContext::CheckTermination() noexcept {
  m_isTerminated = m_scheduler->m_isShutdown && (m_scheduler->m_handlerRefCount == 0);
}

//=============================================================================
// DispatchQueueStatic::GetCurrentUIThreadQueue implementation
//=============================================================================

DispatchQueue DispatchQueueStatic::GetCurrentUIThreadQueue() noexcept {
  DispatchQueue queue{nullptr};
#if USE_WINUI3
  queue = UISchedulerWinRT<MicrosoftDispatcherTraits>::GetOrCreateUIThreadQueue();
  if (!queue) {
#endif
    queue = UISchedulerWinRT<WindowsDispatcherTraits>::GetOrCreateUIThreadQueue();
#if USE_WINUI3
  }
#endif
  return queue;
}

} // namespace Mso
