// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <wrl.h>
#include <atomic>
#include <list>
#include <memory>
#include <mutex>
#include "AsyncWorkQueue.h"

namespace react {
namespace uwp {

struct TP_WORK_DELETER {
  void operator()(TP_WORK *tpWork) {
    if (tpWork != nullptr) {
      ::CloseThreadpoolWork(tpWork);
    }
  }
};

/// <summary>
/// Implements IAsyncWorkQueue.
/// </summary>
class AsyncWorkQueue : public IAsyncWorkQueue {
 private:
  /// <summary>
  /// Represents an individual work item. For each work item queued,
  /// AsyncWorkQueue creates an AsyncWorkItem to store the information about the
  /// request.
  /// </summary>
  struct AsyncWorkItem {
    Microsoft::WRL::ComPtr<IAsyncCallback> Callback;
    Microsoft::WRL::ComPtr<IUnknown> UserData;
  };

 public:
  /// <summary>Creates an instance of IAsyncWorkQueue.</summary>
  /// <param name="ppAsyncWorkQueue">[out] The created instance.</param>
  static HRESULT CreateInstance(_Outptr_ IAsyncWorkQueue **ppAsyncWorkQueue);

  // IUnknown
  STDMETHOD_(ULONG, AddRef)();
  STDMETHOD_(ULONG, Release)();
  STDMETHOD(QueryInterface)(REFIID riid, _Outptr_ void **ppvObject);

  // IAsyncWorkQueue
  VOID CancelAllAsync();
  VOID CancelAllAndWait();
  VOID ShutDown();
  VOID WaitForCallbacksToComplete() override;

  HRESULT QueueWorkItem(
      _In_ IAsyncCallback *pCallback,
      _In_opt_ IUnknown *pUserData);

  HRESULT QueueWorkItemToFront(
      _In_ IAsyncCallback *pCallback,
      _In_opt_ IUnknown *pUserData);

 private:
  AsyncWorkQueue();

  /// <summary>Callback for thread pool. Delegates to AsyncCallBack().</summary>
  /// <param name="pInstance">
  ///   [in] A TP_CALLBACK_INSTANCE structure that defines the callback
  ///   instance. Applications do not modify the members of this structure.
  /// </param>
  /// <param name="pContext">
  ///   [in] The application-defined data, supplied as second parameter to
  ///   CreateThreadpoolWork.
  /// </param>
  /// <param name="pWork">
  ///   [in] A TP_WORK structure that defines the work object that generated the
  ///   callback.
  /// </param>
  static VOID NTAPI s_WorkCallBack(
      _In_ PTP_CALLBACK_INSTANCE pInstance,
      _In_ PVOID pContext,
      _In_ PTP_WORK pWork);

  /// <summary>Callback for threadpool. Delegated to by
  /// s_WorkCallBack().</summary>
  VOID _AsyncCallback();

  enum InsertPosition { Front, Back };

  HRESULT _QueueWorkItemAt(
      _In_ IAsyncCallback *pCallback,
      _In_opt_ IUnknown *pUserData,
      _In_ InsertPosition position);

  VOID _CancelAllAndWait(_In_ BOOL permanentlyShutDown);
  VOID _CancelAllAsync(_In_ BOOL permanentlyShutDown);

 private:
  std::atomic<ULONG> m_cRef{0};

  // m_lock protects m_workItems, m_pProcessingWorkItem, m_workSubmitted, and
  // m_permanentlyShutDown
  std::recursive_mutex m_lock;
  std::list<AsyncWorkItem> m_workItems;
  AsyncWorkItem *m_pProcessingWorkItem;
  volatile BOOL m_workSubmitted;
  BOOL m_permanentlyShutDown;
  // tlx::auto_xxx<PTP_WORK, VOID(__stdcall*)(PTP_WORK), CloseThreadpoolWork,
  // NULL> m_tpWork;
  std::unique_ptr<TP_WORK, TP_WORK_DELETER> m_tpWork;
  volatile DWORD m_processingThreadId;
};

} // namespace uwp
} // namespace react
