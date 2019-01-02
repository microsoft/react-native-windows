// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using ReactNative.Tracing;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;

namespace ReactNative.UIManager
{
    /// <summary>
    /// This class acts as a buffer for command executed on
    /// <see cref="NativeViewHierarchyManager"/>. It exposes similar methods as
    /// mentioned classes but instead of executing commands immediately, it 
    /// enqueues those operations in a queue that is then flushed from 
    /// <see cref="UIManagerModule"/> once a JavaScript batch of UI operations
    /// is finished.
    /// There is one queue instance created for each top level window since
    /// each of these windows run on a different thread.
    /// </summary>
    public class UIViewOperationQueue
    {
        private readonly ReactContext _reactContext;
        private readonly ViewManagerRegistry _viewManagerRegistry;

        private class QueueInstanceInfo
        {
            public UIViewOperationQueueInstance queueInstance;
            public int rootViewCount;
        }

        // Thread safe dictionary using a single lock
        private class SimpleLockedDictionary<TKey, TValue>
        {
            private readonly object _lock = new object();
            private readonly Dictionary<TKey, TValue> _dict = new Dictionary<TKey, TValue>();

            public void Add(TKey key, TValue value)
            {
                lock(_lock)
                {
                    _dict.Add(key, value);
                }
            }

            public bool TryGetValue(TKey key, out TValue value)
            {
                lock (_lock)
                {
                    return _dict.TryGetValue(key, out value);
                }
            }

            public void RemoveList(IList<TKey> keyList)
            {
                lock (_lock)
                {
                    foreach (var key in keyList)
                    {
                        _dict.Remove(key);
                    }
                }
            }
        }

        // Maps tags to corresponding UIViewOperationQueueInstance.
        // Uses a simple serialized Dictionary that depends on a single lock (rather than one per bucket), allows fast removal of list of keys, etc.
        private readonly SimpleLockedDictionary<int, UIViewOperationQueueInstance> _reactTagToOperationQueue = new SimpleLockedDictionary<int, UIViewOperationQueueInstance>();

        // The _dispatcherToOperationQueueInfo (all entries but the main window related one) & _active combo are protected by this lock
        // This allows for consistent lifecycle OnSuspend/OnResume events to be sent to each queue instance
        private readonly object _lock = new object();

        private bool _active;

        // Maps CoreDispatcher to corresponding UIViewOperationQueueInstance + rootView accounting
        // The concurrent dictionary allows for thread safe ".Values" calls, crash-safe enumerating, etc.
        private readonly ConcurrentDictionary<CoreDispatcher, QueueInstanceInfo> _dispatcherToOperationQueueInfo = new ConcurrentDictionary<CoreDispatcher, QueueInstanceInfo>();

        /// <summary>
        /// Instantiates the <see cref="UIViewOperationQueue"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="viewManagerRegistry">
        /// The view manager registry.
        /// </param>
        public UIViewOperationQueue(ReactContext reactContext, ViewManagerRegistry viewManagerRegistry)
        {
            _reactContext = reactContext;
            _viewManagerRegistry = viewManagerRegistry;
            _active = false;

            // Corner case: UWP scenarios that start with no main window.
            // We create the UIViewOperationQueueInstance for main dispatcher thread ahead of time so animations
            // in secondary windows can work.
            var queueInfo = new QueueInstanceInfo()
            {
                queueInstance = new UIViewOperationQueueInstance(
                    _reactContext,
                    new NativeViewHierarchyManager(_viewManagerRegistry, DispatcherHelpers.MainDispatcher, OnViewsDropped),
                    ReactChoreographer.Instance)
            };

            _dispatcherToOperationQueueInfo.AddOrUpdate(DispatcherHelpers.MainDispatcher, queueInfo, (k, v) => throw new InvalidOperationException("Duplicate key"));
            MainUIViewOperationQueue = queueInfo.queueInstance;
        }

        /// <summary>
        /// The native view hierarchy manager.
        /// </summary>
        internal UIViewOperationQueueInstance MainUIViewOperationQueue { get; }

        /// <summary>
        /// Checks if the operation queue is empty.
        /// </summary>
        /// <returns>
        /// <b>true</b> if the queue is empty, <b>false</b> otherwise.
        /// </returns>
        public bool IsEmpty()
        {
            return _dispatcherToOperationQueueInfo.Values.All(qi => qi.queueInstance.IsEmpty());
        }

        /// <summary>
        /// Adds a root view to the hierarchy.
        /// </summary>
        /// <param name="tag">The root view tag.</param>
        /// <param name="rootView">The root view.</param>
        /// <param name="themedRootContext">The React context.</param>
        public void AddRootView(
            int tag,
            SizeMonitoringCanvas rootView,
            ThemedReactContext themedRootContext)
        {
            // This is called on layout manager thread

            // Extract dispatcher
            var rootViewDispatcher = rootView.Dispatcher;

            // _dispatcherToOperationQueueInfo contains a mapping of CoreDispatcher to <UIViewOperationQueueInstance, # of ReactRootView instances>.
            // Operation queue instances are created lazily whenever an "unknown" CoreDispatcher is detected. Each operation queue instance
            // works together with one dedicated NativeViewHierarchyManager and one ReactChoreographer.
            // One operation queue is the "main" one:
            // - is coupled with the CoreApplication.MainView dispatcher
            // - drives animations in ALL views
            if (!_dispatcherToOperationQueueInfo.TryGetValue(rootViewDispatcher, out var queueInfo))
            {
                // Queue instance doesn't exist for this dispatcher, we need to create

                // Find the CoreApplicationView associated to the new CoreDispatcher
                CoreApplicationView foundView = CoreApplication.Views.First(v => v.Dispatcher == rootViewDispatcher);

                // Create new ReactChoreographer for this view/dispatcher. It will only be used for its DispatchUICallback services
                var reactChoreographer = ReactChoreographer.CreateSecondaryInstance(foundView);

                queueInfo = new QueueInstanceInfo()
                {
                    queueInstance = new UIViewOperationQueueInstance(
                        _reactContext,
                        new NativeViewHierarchyManager(_viewManagerRegistry, rootViewDispatcher, OnViewsDropped),
                        reactChoreographer),
                    rootViewCount = 1
                };

                lock (_lock)
                {
                    // Add new tuple to map
                    _dispatcherToOperationQueueInfo.AddOrUpdate(rootViewDispatcher, queueInfo, (k, v) => throw new InvalidOperationException("Duplicate key"));

                    if (_active)
                    {
                        // Simulate an OnResume from the correct dispatcher thread
                        // (OnResume/OnSuspend/OnDestroy have this thread affinity, all other methods do enqueuings in a thread safe manner)
                        // (No inlining here so we don't hold lock during call outs. Not a big deal since inlining
                        // is only useful for main UI thread, and this code is not executed for that one)
                        DispatcherHelpers.RunOnDispatcher(rootViewDispatcher, queueInfo.queueInstance.OnResume);
                    }
                }
            }
            else
            {
                // Queue instance does exist.
                // Increment the count of root views. This is helpful for the case the count reaches 0 so we can cleanup the queue.
                queueInfo.rootViewCount++;
            }

            // Add tag
            _reactTagToOperationQueue.Add(tag, queueInfo.queueInstance);

            // Send forward
            queueInfo.queueInstance.AddRootView(tag, rootView, themedRootContext);
       }

        /// <summary>
        /// Enqueues an operation to remove the root view.
        /// </summary>
        /// <param name="rootViewTag">The root view tag.</param>
        public Task RemoveRootViewAsync(int rootViewTag)
        {
            // Called on layout manager thread

            UIViewOperationQueueInstance queue = GetQueueByTag(rootViewTag);

            // Send forward
            queue.EnqueueRemoveRootView(rootViewTag);

            // Do some maintenance/cleanup if needed.
            // Find the queue info
            var pair = _dispatcherToOperationQueueInfo.First(p => p.Value.queueInstance == queue);

            // Decrement number of root views
            pair.Value.rootViewCount--;

            if (queue != MainUIViewOperationQueue)
            {
                if (pair.Value.rootViewCount == 0)
                {
                    // We can remove this queue and then destroy
                    _dispatcherToOperationQueueInfo.TryRemove(pair.Key, out _);

                    // Simulate an OnDestroy from the correct dispatcher thread
                    // (OnResume/OnSuspend/OnDestroy have this thread affinity, all other methods do enqueuings in a thread safe manner)
                    return DispatcherHelpers.CallOnDispatcher(pair.Key, () =>
                    {
                        queue.OnDestroy();

                        return true;
                    });
                }
                else
                {
                    return Task.CompletedTask;
                }
            }
            else
            {
                return Task.CompletedTask;
            }
        }

        /// <summary>
        /// Refreshes RTL/LTR direction on all root views.
        /// </summary>
        public void UpdateRootViewNodesDirection()
        {
            // Called on layout manager thread

            // Dispatch to all queues
            foreach (var queue in _dispatcherToOperationQueueInfo.Values)
            {
                queue.queueInstance.UpdateRootViewNodesDirection();
            }
        }

        /// <summary>
        /// Enqueues an operation to dispatch a command.
        /// </summary>
        /// <param name="tag">The view tag.</param>
        /// <param name="commandId">The command identifier.</param>
        /// <param name="commandArgs">The command arguments.</param>
        public void EnqueueDispatchCommand(int tag, int commandId, JArray commandArgs)
        {
            // Called on layout manager thread

            GetQueueByTag(tag).EnqueueDispatchCommand(tag, commandId, commandArgs);
        }

        /// <summary>
        /// Enqueues an operation to update the extra data for a view.
        /// </summary>
        /// <param name="tag">The view tag.</param>
        /// <param name="extraData">The extra data.</param>
        public void EnqueueUpdateExtraData(int tag, object extraData)
        {
            // Called on layout manager thread

            GetQueueByTag(tag).EnqueueUpdateExtraData(tag, extraData);
        }

        /// <summary>
        /// Enqueues an operation to show a popup menu.
        /// </summary>
        /// <param name="tag">The view tag.</param>
        /// <param name="items">The menu items.</param>
        /// <param name="error">Called on error.</param>
        /// <param name="success">Called on success.</param>
        public void EnqueueShowPopupMenu(int tag, string[] items, ICallback error, ICallback success)
        {
            // Called on layout manager thread

            GetQueueByTag(tag).EnqueueShowPopupMenu(tag, items, error, success);
        }

        /// <summary>
        /// Enqueues a operation to execute a UIBlock.
        /// </summary>
        /// <param name="block">The UI block.</param>
        /// <param name="tag">Optional react tag hint that triggers the choice of the dispatcher thread that executes the block .</param>
        public void EnqueueUIBlock(IUIBlock block, int? tag)
        {
            // Called on layout manager thread

            (tag.HasValue ? GetQueueByTag(tag.Value) : MainUIViewOperationQueue).EnqueueUIBlock(block);
        }

        /// <summary>
        /// Enqueues a operation to execute a UIBlock.
        /// </summary>
        /// <param name="block">The UI block.</param>
        /// <param name="tag">Optional react tag hint that triggers the choice of the dispatcher thread that executes the block .</param>
        public void PrependUIBlock(IUIBlock block, int? tag)
        {
            // Called on layout manager thread

            (tag.HasValue ? GetQueueByTag(tag.Value) : MainUIViewOperationQueue).PrependUIBlock(block);
        }

        /// <summary>
        /// Enqueues an operation to create a view.
        /// </summary>
        /// <param name="themedContext">The React context.</param>
        /// <param name="viewReactTag">The view React tag.</param>
        /// <param name="viewClassName">The view class name.</param>
        /// <param name="initialProps">The initial props.</param>
        /// <param name="rootViewTag">Root view tag.</param>
        public void EnqueueCreateView(
            ThemedReactContext themedContext,
            int viewReactTag,
            string viewClassName,
            JObject initialProps,
            int rootViewTag)
        {
            // Called on layout manager thread

            UIViewOperationQueueInstance queue = GetQueueByTag(rootViewTag);

            _reactTagToOperationQueue.Add(viewReactTag, queue);

            queue.EnqueueCreateView(themedContext, viewReactTag, viewClassName, initialProps);
        }

        /// <summary>
        /// Enqueue a configure layout animation operation.
        /// </summary>
        /// <param name="config">The configuration.</param>
        /// <param name="success">The success callback.</param>
        /// <param name="error">The error callback.</param>
        public void EnqueueConfigureLayoutAnimation(JObject config, ICallback success, ICallback error)
        {
            // Called on layout manager thread

            // We have to dispatch this to all queues. Each queue will reset the "animating layout" at the end of the current batch.
            foreach (var queue in _dispatcherToOperationQueueInfo.Values)
            {
                queue.queueInstance.EnqueueConfigureLayoutAnimation(config, success, error);
            }
        }

        /// <summary>
        /// Enqueues an operation to update the props of a view.
        /// </summary>
        /// <param name="tag">The view tag.</param>
        /// <param name="className">The class name.</param>
        /// <param name="props">The props.</param>
        public void EnqueueUpdateProps(int tag, string className, JObject props)
        {
            // Called on layout manager thread

            GetQueueByTag(tag).EnqueueUpdateProps(tag, className, props);
        }

        /// <summary>
        /// Enqueues an operation to update the layout of a view.
        /// </summary>
        /// <param name="parentTag">The parent tag.</param>
        /// <param name="tag">The view tag.</param>
        /// <param name="dimensions">The dimensions.</param>
        public void EnqueueUpdateLayout(
            int parentTag,
            int tag,
            Dimensions dimensions)
        {
            // Called on layout manager thread

            GetQueueByTag(tag).EnqueueUpdateLayout(parentTag, tag, dimensions);
        }

        /// <summary>
        /// Enqueues an operation to manage the children of a view.
        /// </summary>
        /// <param name="tag">The view to manage.</param>
        /// <param name="indexesToRemove">The indices to remove.</param>
        /// <param name="viewsToAdd">The views to add.</param>
        /// <param name="tagsToDelete">The tags to delete.</param>
        public void EnqueueManageChildren(
            int tag,
            int[] indexesToRemove,
            ViewAtIndex[] viewsToAdd,
            int[] tagsToDelete)
        {
            // Called on layout manager thread

            GetQueueByTag(tag).EnqueueManageChildren(tag, indexesToRemove, viewsToAdd, tagsToDelete);
        }

        /// <summary>
        /// Enqueues an operation to set the children of a view.
        /// </summary>
        /// <param name="tag">The view to manage.</param>
        /// <param name="childrenTags">The children tags.</param>
        public void EnqueueSetChildren(int tag, int[] childrenTags)
        {
            // Called on layout manager thread

            GetQueueByTag(tag).EnqueueSetChildren(tag, childrenTags);
        }

        /// <summary>
        /// Enqueues an operation to measure the view.
        /// </summary>
        /// <param name="tag">The tag of the view to measure.</param>
        /// <param name="callback">The measurement result callback.</param>
        public void EnqueueMeasure(int tag, ICallback callback)
        {
            // Called on layout manager thread

            UIViewOperationQueueInstance queue = GetQueueByTag(tag, true);
            if (queue == null)
            {
                // This is called bypassing the optimizer, so we need to fake a result for layout only nodes.
                callback.Invoke();
                return;
            }

            queue.EnqueueMeasure(tag, callback);
        }

        /// <summary>
        /// Enqueues an operation to measure the view relative to the window.
        /// </summary>
        /// <param name="tag">The tag of the view to measure.</param>
        /// <param name="callback">The measurement result callback.</param>
        public void EnqueueMeasureInWindow(int tag, ICallback callback)
        {
            // Called on layout manager thread

            UIViewOperationQueueInstance queue = GetQueueByTag(tag, true);
            if (queue == null)
            {
                // This is called bypassing the optimizer, so we need to fake a result for layout only nodes.
                callback.Invoke();
                return;
            }

            queue.EnqueueMeasureInWindow(tag, callback);
        }

        /// <summary>
        /// Enqueues an operation to find a touch target.
        /// </summary>
        /// <param name="tag">The parent view to search from.</param>
        /// <param name="targetX">The x-coordinate of the touch event.</param>
        /// <param name="targetY">The y-coordinate of the touch event.</param>
        /// <param name="callback">The callback.</param>
        public void EnqueueFindTargetForTouch(
            int tag,
            double targetX,
            double targetY,
            ICallback callback)
        {
            // Called on layout manager thread

            UIViewOperationQueueInstance queue = GetQueueByTag(tag, true);
            if (queue == null)
            {
                // This is called bypassing the optimizer, so we need to fake a result for layout only nodes.
                callback.Invoke();
                return;
            }

            queue.EnqueueFindTargetForTouch(tag, targetX, targetY, callback);
        }

        /// <summary>
        /// Called when the host receives the suspend event.
        /// </summary>
        public void OnSuspend()
        {
            MainUIViewOperationQueue.OnSuspend();

            lock (_lock)
            {
                _active = false;
                foreach (var pair in _dispatcherToOperationQueueInfo)
                {
                    if (pair.Key != MainUIViewOperationQueue.Dispatcher)
                    {
                        // Simulate an OnSuspend from the correct dispatcher thread
                        // (OnResume/OnSuspend/OnDestroy have this thread affinity, all other methods do enqueuings in a thread safe manner)
                        DispatcherHelpers.RunOnDispatcher(pair.Key, pair.Value.queueInstance.OnSuspend);
                    }
                }
            }
        }

        /// <summary>
        /// Called when the host receives the resume event.
        /// </summary>
        public void OnResume()
        {
            MainUIViewOperationQueue.OnResume();

            lock (_lock)
            {
                _active = true;
                foreach (var pair in _dispatcherToOperationQueueInfo)
                {
                    if (pair.Key != MainUIViewOperationQueue.Dispatcher)
                    {
                        // Simulate an OnResume from the correct dispatcher thread
                        // (OnResume/OnSuspend/OnDestroy have this thread affinity, all other methods do enqueuings in a thread safe manner)
                        DispatcherHelpers.RunOnDispatcher(pair.Key, pair.Value.queueInstance.OnResume);
                    }
                }
            }
        }

        /// <summary>
        /// Called when the host is shutting down.
        /// </summary>
        public void OnDestroy()
        {
            MainUIViewOperationQueue.OnDestroy();

            lock (_lock)
            {
                _active = false;
                foreach (var pair in _dispatcherToOperationQueueInfo)
                {
                    if (pair.Key != MainUIViewOperationQueue.Dispatcher)
                    {
                        // Simulate an OnDestroy from the correct dispatcher thread
                        // (OnResume/OnSuspend/OnDestroy have this thread affinity, all other methods do enqueuings in a thread safe manner)
                        DispatcherHelpers.RunOnDispatcher(pair.Key, pair.Value.queueInstance.OnDestroy);
                    }
                }
            }
        }

        /// <summary>
        /// Used by the native animated module to bypass the process of
        /// updating the values through the shadow view hierarchy. This method
        /// will directly update the native views, which means that updates for
        /// layout-related props won't be handled properly.
        /// </summary>
        /// <param name="tag">The view tag.</param>
        /// <param name="props">The props.</param>
        /// <remarks>
        /// Make sure you know what you're doing before calling this method :)
        /// </remarks>
        public bool SynchronouslyUpdateViewOnDispatcherThread(int tag, JObject props)
        {
            // The native animations module is a single threaded implementation affinitized to the "main" dispatcher thread.
            // As a result all calls of this method are on main dispatcher thread.
            // Yet, for secondary views we have to dispatch to correct dispatcher as fast as possible
            DispatcherHelpers.AssertOnDispatcher();

            UIViewOperationQueueInstance queue = GetQueueByTag(tag, true);
            if (queue == null)
            {
                // Returns false as per the caller's expectation
                return false;
            }

            if (queue == MainUIViewOperationQueue)
            {
                // Main queue case. Just forward.
                if (!MainUIViewOperationQueue.NativeViewHierarchyManager.ViewExists(tag))
                {
                    return false;
                }

                MainUIViewOperationQueue.NativeViewHierarchyManager.UpdateProps(tag, props);
            }
            else
            {
                // Dispatch to the correct thread.
                DispatcherHelpers.RunOnDispatcher(queue.Dispatcher, CoreDispatcherPriority.High, () =>
                {
                    if (queue.NativeViewHierarchyManager.ViewExists(tag))
                    {
                        queue.NativeViewHierarchyManager.UpdateProps(tag, props);
                    }
                    else
                    {
                        RnLog.Warn(nameof(UIViewOperationQueue), $"View with tag '{tag}' not found due to race condition");
                    }
                });
            }
            return true;
        }

        /// <summary>
        /// Dispatches the view updates.
        /// </summary>
        /// <param name="batchId">The batch identifier.</param>
        internal void DispatchViewUpdates(int batchId)
        {
            // Called on layout manager thread

            // Dispatch to all queues
            foreach (var queue in _dispatcherToOperationQueueInfo.Values)
            {
                queue.queueInstance.DispatchViewUpdates(batchId);
            }
        }

        private UIViewOperationQueueInstance GetQueueByTag(int tag, bool dontThrow = false)
        {
            if (!_reactTagToOperationQueue.TryGetValue(tag, out var queue))
            {
                if (dontThrow)
                {
                    return null;
                }
                else
                {
                    throw new InvalidOperationException("No queue for tag " + tag);
                }
            }
            return queue;
        }

        /// <summary>
        /// Hook needed to cleanup the local state (_reactTagToOperationQueue)
        /// </summary>
        /// <param name="tags">List of deleted tags..</param>
        private void OnViewsDropped(List<int> tags)
        {
            _reactTagToOperationQueue.RemoveList(tags);
        }
    }
}
