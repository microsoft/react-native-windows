// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "unknwn.h"

namespace react { namespace uwp {

/// <summary>
/// Represents callback method for for work items processed by the AsyncWorkQueue.  Callers queuing
/// work items must provide a an IAsyncCallback implementation to process the work item along with
/// optional user data.  When the AsyncWorkQueue wishes to have the work item processed it will
/// call this interface and provide the user data.
///
/// Calls to ProcessWorkItem() will come on a thread that is owned by the work queue, and it is the
/// implementer's job to ensure its own thread safety.
/// </summary>
interface DECLSPEC_UUID("75E41B6E-935D-49b6-A8C7-71FC123EE91D")
  IAsyncCallback : IUnknown
{
  /// <summary>
  /// Invoked from AsyncWorkQueue to ask the client to process the work item.
  /// </summary>
  /// <param name="pUserData">[in] Reference to the user data</param>
  /// <returns>
  /// If ProcessWorkItem returns an error code it will be logged but execution will continue.
  /// </returns>
  STDMETHOD(ProcessWorkItem) (_In_opt_ IUnknown* pUserData) PURE;

  /// <summary>
  /// Invoked from AsyncWorkQueue to ask the client to cancel a processing work item.
  /// This can be called on any thread at any time. Implementers should strive to do minimal
  /// work in this function, just enough to signal the thread that's actually running the work
  /// item if possible. It is also possible for a work item to be canceled multiple times while
  /// it is running and implementers must handle this. If there is no way to cancel a specific
  /// work item while it is running then this function should do nothing but return S_OK.
  /// </summary>
  /// <param name="pUserData">[in] Reference to the user data</param>
  /// <returns>
  /// This should return S_OK if the work item was successfully canceled or if there is no way
  /// to interrupt its work. It should return an error only if something unexpectedly failed.
  /// The result of the error return will depend on the caller. If this function was invoked
  /// because of an external call to IAsyncWorkQueue::CancelAll*() then the error will be
  /// returned to the caller. But if it was invoked as the work queue is shutting down then
  /// the failure will be logged but execution will continue.
  /// </returns>
  STDMETHOD(CancelWorkItem) (_In_opt_ IUnknown* pUserData) PURE;
};

/// <summary>
/// Interface that can be exposed by callbacks to get notified if they are canceled without
/// ever being executed. Note that CancelWorkItem will be called for a work item that was canceled
/// during execution. If a work item implements this interface, this call will be made if that work
/// item gets canceled before it executes. Implementations should strive to do minimal work in this
/// callback - preferably just signaling or making note of the fact that the work item was never run.
/// </summary>
interface DECLSPEC_UUID("16f7789f-46bb-4124-aba3-266a56d464c2")
  IAsyncCallbackCancelExtension : public IUnknown
{
  /// <summary>
  /// Called by the work queue if a work item is canceled without every being
  /// executed.
  /// </summary>
  /// <param name="pUserData">User context passed to the work item</param>
  virtual void OnWorkItemCanceled(_In_opt_ IUnknown* pUserData) = 0;
};


/// <summary>
/// Represents a work queue that processes its work items serially on a thread other than the one
/// from which the items are queued. Individual work items consist of a callback object that
/// implements IAsyncCallback and user data typed as IUnknown since only the client will understand
/// its details.
/// </summary>
interface DECLSPEC_UUID("51AB52B1-F754-44bb-8F84-368313D9500F")
  IAsyncWorkQueue : IUnknown
{
  /// <summary>
  /// Cancel all work items. This function returns after clearing the list of work items and
  /// requesting any processing work item to cancel. It does not wait for that item to finish.
  /// </summary>
  virtual void CancelAllAsync() = 0;

  /// <summary>
  /// Cancels all work items and then waits for any running work item to complete before
  /// returning. When this method returns, subsequent attempts to queue work items will still
  /// succeed. Use ShutDown instead to cancel all items and permanently disable the work queue.
  /// </summary>
  /// <remarks>
  /// Caller should pay attention to the thread on which this function is called. If it is
  /// called on any worker thread for IAsyncWorkQueue's processing it could lead to deadlock.
  /// Here is some guidance:
  ///  a) Never call this function in IAsyncCallback::ProcessWorkItem.
  ///  b) If call this function in an object's destructor, you need to fully understand the
  ///     lifetime of the object to make sure it is never destroyed on any worker thread. For
  ///     example, if a client object holds a reference to IAsyncWorkQueue and is implemented
  ///     as a IAsyncCallback as well, calling this function in client object's destructor may
  ///     cause deadlock.
  /// </remarks>
  virtual void CancelAllAndWait() = 0;

  /// <summary>
  /// Cancels all work items and then waits for any running work item to complete before returning.
  /// When this method returns, subsequent attempts to queue work items fail.
  /// </summary>
  /// <remarks>
  /// 1. It is required to call Shutdown before destroy the IAsyncWorkQueue object.
  /// 2. Caller should pay attention to the thread on which this function is called. If it is
  ///    called on any worker thread for IAsyncWorkQueue's processing it could lead to deadlock.
  ///    Here is some guidance:
  ///    a) Never call this function in IAsyncCallback::ProcessWorkItem.
  ///    b) If call this function in an object's destructor, you need to fully understand the
  ///       lifetime of the object to make sure it is never destroyed on any worker thread. For
  ///       example, if a client object holds a reference to IAsyncWorkQueue and is implemented
  ///       as a IAsyncCallback as well, calling this function in client object's destructor may
  ///       cause deadlock.
  /// </remarks>
  virtual void ShutDown() = 0;

  /// <summary>
  /// Waits for queued callbacks to be completed
  /// </summary>
  /// <remarks>
  /// Waits for the dispatching of callbacks to complete. If new items are added to the queue
  /// while waiting, those items will also be included in the wait.
  /// </remarks>
  virtual void WaitForCallbacksToComplete() = 0;

  /// <summary>
  /// Adds a work item to the back of the queue.  These items are dequeued one by one and
  /// processed in a serial fashion.  Processing is done by invoking the callback with
  /// the associated userData.
  /// </summary>
  /// <param name="pCallback">[in] The object to call for processing the work item</param>
  /// <param name="pUserData">[in] Data to pass to the callback when processing the work item</param>
  /// <returns>
  /// HRESULT.
  /// </returns>
  virtual HRESULT QueueWorkItem(
    _In_ IAsyncCallback* pCallback,
    _In_opt_ IUnknown* pUserData) = 0;
};

/// <summary>
/// Creates an instance of IAsyncWorkQueue.
/// </summary>
/// <param name="ppAsyncWorkQueue">[out] The created instance.</param>
/// <returns>
/// HRESULT.
/// </returns>
HRESULT CreateAsyncWorkQueue(_Outptr_ IAsyncWorkQueue** ppAsyncWorkQueue);

} }
