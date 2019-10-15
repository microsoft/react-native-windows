// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "AsyncWorkQueue_Priv.h"

#include <wrl.h>

namespace react {
namespace uwp {

HRESULT CreateAsyncWorkQueue(_Outptr_ IAsyncWorkQueue **ppAsyncWorkQueue) {
  return AsyncWorkQueue::CreateInstance(ppAsyncWorkQueue);
}

HRESULT AsyncWorkQueue::CreateInstance(
    _Outptr_ IAsyncWorkQueue **ppAsyncWorkQueue) {
  Microsoft::WRL::ComPtr<AsyncWorkQueue> workQueue(new AsyncWorkQueue());

  // TODO: Are we using throwing new?
  if (workQueue == nullptr)
    return E_OUTOFMEMORY;

  // Create threadpool work callback
  workQueue->m_tpWork.reset(::CreateThreadpoolWork(
      AsyncWorkQueue::s_WorkCallBack, workQueue.Get(), NULL));
  if (workQueue->m_tpWork.get() == nullptr)
    return GetLastError();

  // Transfer ownership to ppAsyncWorkQueue
  *ppAsyncWorkQueue = workQueue.Detach();

  return S_OK;
}

AsyncWorkQueue::AsyncWorkQueue()
    : m_pProcessingWorkItem(NULL),
      m_processingThreadId(0),
      m_permanentlyShutDown(FALSE),
      m_workSubmitted(FALSE) {}

// AsyncWorkQueue::~AsyncWorkQueue()
//{
// ASSERTs: The clients are required to call ShutDown() before they destroy the
// AsyncWorkQueue object. If the clients fail to call to ShutDown() an
// outstanding worker thread may access the freed AsyncWorkQueue object and
// cause an access violation.
//
// We can't call ShutDown for the clients, as that would cause a dead lock. If
// we did and the client doesn't call ShutDown() before it releases the
// AsyncWorkQueue, we would get into the situation that the IAsyncCallback holds
// the last reference to the client object which holds the last reference back
// to the AsyncWorkQueue. When the IAsyncCallback Release() is processed,
// AsyncWorkQueue is going to be destroyed on the  worker thread. Thus calling
// ShutDown here causes WaitForThreadpoolWorkCallbacks on a worker thread,
// that's the dead lock of "wait for itself to finish".

// TODO: Asserts
// ASSERT(m_permanentlyShutDown);
// ASSERT(m_workItems.empty());
// ASSERT(!m_workSubmitted);
//}

COM_DECLSPEC_NOTHROW STDMETHODIMP_(ULONG) AsyncWorkQueue::AddRef() {
  return ++m_cRef;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP_(ULONG) AsyncWorkQueue::Release() {
  ULONG cRef = --m_cRef;

  if (cRef == 0) {
    delete this;
  }

  return cRef;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP
AsyncWorkQueue::QueryInterface(REFIID riid, _Outptr_ void **ppvObject) {
  if (IsEqualIID(riid, __uuidof(IAsyncWorkQueue))) {
    *ppvObject = static_cast<IAsyncWorkQueue *>(this);
    ((IAsyncWorkQueue *)(*ppvObject))->AddRef();
  } else if (IsEqualIID(riid, __uuidof(IUnknown))) {
    *ppvObject = static_cast<IUnknown *>(this);
    ((IUnknown *)(*ppvObject))->AddRef();
  } else {
    *ppvObject = NULL;
    return E_NOINTERFACE;
  }

  return S_OK;
}

VOID AsyncWorkQueue::s_WorkCallBack(
    _In_ PTP_CALLBACK_INSTANCE pInstance,
    _In_ PVOID pContext,
    _In_ PTP_WORK pWork) {
  UNREFERENCED_PARAMETER(pInstance);
  UNREFERENCED_PARAMETER(pWork);

  AsyncWorkQueue *pWorkQueue = (AsyncWorkQueue *)pContext;
  // TODO: Asserts
  // ASSERT(NULL != pWorkQueue);

  // We must handle a subtle case when the IAsyncCallback implementation holds a
  // reference to a client object which holds a reference back to the
  // AsyncWorkQueue:
  //
  //   IAsyncCallback object --> client object --> AsyncWorkQueue
  //
  // The IAsyncCallback object will be released after processed. At that time if
  // it holds the last reference to client object and client object holds the
  // last reference to AsyncWorkQueue, AsyncWorkQueue will be destroyed. Thus
  // when AsyncWorkQueue::_AsyncCallback() is processing the next work item (in
  // the next iteration in the for loop) and trying to get the lock (m_lock), it
  // is accessing a freed memory.
  //
  // To resolve this problem, we must increase the ref count to AsyncWorkQueue
  // to prevent it being destroyed inside the AsyncWorkQueue::_AsyncCallback(),
  // but we cannot simply call AddRef() before and Release() after
  // AsyncWorkQueue::_AsyncCallback(). The trick here is that, the
  // implementation of our IUnknown::Release() decreases the ref count *before*
  // calling destructor. So when one thread is in the destructor the ref count
  // is already zero,  AddRef() and Release() cause the ref count bump up to 1
  // and drop to 0 and eventually cause destructor be called again.
  //
  // We solve this by doing the following:
  // 1. AddRef() returning 1 means the AsyncWorkQueue is being destroyed. When
  // detected we stop processing
  //    IAsyncCallback objects.
  // 2. WaitForThreadpoolWorkCallbacks() in the destructor makes sure the memory
  // won�t be freed until all
  //    outstanding callbacks finish.

  if (pWorkQueue->AddRef() > 1) {
    pWorkQueue->_AsyncCallback();
    pWorkQueue->Release();
  }
}

VOID AsyncWorkQueue::_AsyncCallback() {
  // Crash if this is ever re-entered
  ULONG res = InterlockedCompareExchange(
      &m_processingThreadId, GetCurrentThreadId(), 0);

  // TODO: Asserts
  // ASSERT(res == 0);
  res;

  AsyncWorkItem workItem;

  // Process work items as long as they still exist in the queue
  for (;;) {
    // Attempt to pop one work item off the front of the queue. If there isn't
    // one then exit. The lock must be engaged for this entire process to ensure
    // protection of m_workItems and m_pProcessingWorkItem in tandem.
    {
      std::lock_guard<std::recursive_mutex> lock(m_lock);

      if (m_workItems.empty()) {
        m_pProcessingWorkItem = NULL;
        m_processingThreadId = 0;
        m_workSubmitted = FALSE;
        break;
      }

      workItem = m_workItems.front();
      m_workItems.pop_front();
      m_pProcessingWorkItem = &workItem;
    }

    // We DO NOT want m_lock be held when processing work item.
    HRESULT hr = workItem.Callback->ProcessWorkItem(workItem.UserData.Get());
    if (FAILED(hr)) {
      // TODO: log hr
    }
  }
}

HRESULT AsyncWorkQueue::QueueWorkItem(
    _In_ IAsyncCallback *pCallback,
    _In_opt_ IUnknown *pUserData) {
  HRESULT hr = _QueueWorkItemAt(pCallback, pUserData, InsertPosition::Back);

  return hr;
}

HRESULT AsyncWorkQueue::QueueWorkItemToFront(
    _In_ IAsyncCallback *pCallback,
    _In_opt_ IUnknown *pUserData) {
  HRESULT hr = _QueueWorkItemAt(pCallback, pUserData, InsertPosition::Front);

  return hr;
}

HRESULT AsyncWorkQueue::_QueueWorkItemAt(
    _In_ IAsyncCallback *pCallback,
    _In_opt_ IUnknown *pUserData,
    _In_ InsertPosition position) {
  // Can only add work items when the queue is properly initialized
  // TODO: Asserts
  // ASSERT(m_tpWork.get() != nullptr);

  // Construct work item structure
  AsyncWorkItem workItem;
  workItem.Callback = pCallback;
  workItem.UserData = pUserData;

  // Safely add work item to the queue
  {
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    if (m_permanentlyShutDown)
      return HRESULT_FROM_WIN32(ERROR_CANCELLED);

    try {
      if (InsertPosition::Front == position) {
        m_workItems.push_front(workItem);
      } else {
        m_workItems.push_back(workItem);
      }
    } catch (...) {
      return E_OUTOFMEMORY;
    }

    // Only 1 ThreadPoolWork item submitted at a time.
    if (!m_workSubmitted) {
      ::SubmitThreadpoolWork(m_tpWork.get());
      m_workSubmitted = TRUE;
    }
  }

  return S_OK;
}

VOID AsyncWorkQueue::CancelAllAndWait() {
  return _CancelAllAndWait(FALSE);
}

VOID AsyncWorkQueue::CancelAllAsync() {
  return _CancelAllAsync(FALSE);
}

VOID AsyncWorkQueue::ShutDown() {
  return _CancelAllAndWait(TRUE);
}

VOID AsyncWorkQueue::WaitForCallbacksToComplete() {
  if (m_tpWork.get() != nullptr) {
    ::WaitForThreadpoolWorkCallbacks(m_tpWork.get(), FALSE);
  }
}

VOID AsyncWorkQueue::_CancelAllAndWait(_In_ BOOL permanentlyShutDown) {
  // Cancel all work items
  _CancelAllAsync(permanentlyShutDown);

  // The active work item might still be running, so wait for it to complete
  if (m_tpWork.get() != nullptr) {
    // The WaitForThreadpoolWorkCallbacks function blocks the calling thread
    // until all outstanding callback functions for a work object have completed
    // execution. Passing FALSE for the 2nd parameter allows the callbacks that
    // have been queued to the thread pool to be dispatched to worker thread for
    // execution.  However since _CancelAllAsync has emptied m_workItems, no
    // workItem's ProcessWorkItem will execute, but we will reset all of our
    // state variables
    ::WaitForThreadpoolWorkCallbacks(m_tpWork.get(), FALSE);
  }
}

VOID AsyncWorkQueue::_CancelAllAsync(_In_ BOOL permanentlyShutDown) {
  std::list<AsyncWorkItem> workItems;

  {
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    if (permanentlyShutDown) {
      m_permanentlyShutDown = TRUE;
    }

    // Clear out any work items not yet processed and make a copy
    // to dispatch cancellation notification
    workItems.swap(m_workItems);

    // Cancel currently running work item
    if (m_pProcessingWorkItem != NULL) {
      HRESULT hr = m_pProcessingWorkItem->Callback->CancelWorkItem(
          m_pProcessingWorkItem->UserData.Get());
      if (FAILED(hr)) {
        // TODO: Log hr
      }
    }
  }

  // Dispatch cancellation notifications to any work items that are interested
  for (auto asyncWorkItem : workItems) {
    Microsoft::WRL::ComPtr<IAsyncCallbackCancelExtension> callbackNotification;
    HRESULT hr;
    hr = asyncWorkItem.Callback.As(&callbackNotification);
    if (SUCCEEDED(hr)) {
      callbackNotification->OnWorkItemCanceled(asyncWorkItem.UserData.Get());
    }
  }

  workItems.clear();
}

} // namespace uwp
} // namespace react
