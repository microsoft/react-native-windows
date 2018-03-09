using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Bridge.Queue;
using ReactNative.Modules.Core;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
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

        private UIViewOperationQueueInstance _mainUiViewOperationsQueueInstance;

        // Maps tags to corresponding UIViewOperationQueueInstance
        private readonly IDictionary<int, UIViewOperationQueueInstance> _reactTagToOperationQueue = new Dictionary<int, UIViewOperationQueueInstance>();

        // Maps CoreDispatcher to corresponding UIViewOperationQueueInstance + rootView accounting
        private readonly IDictionary<CoreDispatcher, QueueInstanceInfo> _dispatcherToOperationQueueInfo = new Dictionary<CoreDispatcher, QueueInstanceInfo>();

        // Action queue used by UIManagerModule
        private IActionQueue _uiManagerActionQueue;

        private bool _active;

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

            _dispatcherToOperationQueueInfo.Add(DispatcherHelpers.MainDispatcher, queueInfo);
            _mainUiViewOperationsQueueInstance = queueInfo.queueInstance;
        }

        /// <summary>
        /// The native view hierarchy manager.
        /// </summary>
        internal UIViewOperationQueueInstance MainUIViewOperationQueue => _mainUiViewOperationsQueueInstance;

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
                ReactChoreographer reactChoreographer = ReactChoreographer.CreateSecondaryInstance(foundView);

                queueInfo = new QueueInstanceInfo()
                {
                    queueInstance = new UIViewOperationQueueInstance(
                        _reactContext,
                        new NativeViewHierarchyManager(_viewManagerRegistry, rootViewDispatcher, OnViewsDropped),
                        reactChoreographer),
                    rootViewCount = 1
                };

                // Add new tuple to map
                _dispatcherToOperationQueueInfo.Add(rootViewDispatcher, queueInfo);

                if (_active)
                {
                    // Simulate an OnResume from the correct dispatcher thread
                    // (OnResume/OnSuspend/OnDestroy have this thread affinity, all other methods do enqueuings in a thread safe manner)
                    DispatcherHelpers.RunOnDispatcher(rootViewDispatcher, queueInfo.queueInstance.OnResume, true); // inlining allowed
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
        public void EnqueueRemoveRootView(int rootViewTag)
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
                    _dispatcherToOperationQueueInfo.Remove(pair.Key);

                    // Simulate an OnDestroy from the correct dispatcher thread
                    // (OnResume/OnSuspend/OnDestroy have this thread affinity, all other methods do enqueuings in a thread safe manner)
                    DispatcherHelpers.RunOnDispatcher(pair.Key, queue.OnDestroy);
                }
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
        public void EnqueueUIBlock(IUIBlock block)
        {
            // Called on layout manager thread

            // Native animation module is synchronized to the main dispatcher thread.
            // Always forward to the main queue
            MainUIViewOperationQueue.EnqueueUIBlock(block);
        }

        /// <summary>
        /// Enqueues a operation to execute a UIBlock.
        /// </summary>
        /// <param name="block">The UI block.</param>
        public void PrependUIBlock(IUIBlock block)
        {
            // Called on layout manager thread

            // Native animation module is synchronized to the main dispatcher thread.
            // Always forward to the main queue
            MainUIViewOperationQueue.PrependUIBlock(block);
        }

        /// <summary>
        /// Enqueues an operation to create a view.
        /// </summary>
        /// <param name="themedContext">The React context.</param>
        /// <param name="viewReactTag">The view React tag.</param>
        /// <param name="viewClassName">The view class name.</param>
        /// <param name="initialProps">The initial properties.</param>
        /// <param name="rootViewTag">Root view tag.</param>
        public void EnqueueCreateView(
            ThemedReactContext themedContext,
            int viewReactTag,
            string viewClassName,
            ReactStylesDiffMap initialProps,
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
        /// Enqueues an operation to update the properties of a view.
        /// </summary>
        /// <param name="tag">The view tag.</param>
        /// <param name="className">The class name.</param>
        /// <param name="props">The properties.</param>
        public void EnqueueUpdateProperties(int tag, string className, ReactStylesDiffMap props)
        {
            // Called on layout manager thread

            GetQueueByTag(tag).EnqueueUpdateProperties(tag, className, props);
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
             _active = false;
            foreach (var pair in _dispatcherToOperationQueueInfo)
            {
                // Simulate an OnSuspend from the correct dispatcher thread
                // (OnResume/OnSuspend/OnDestroy have this thread affinity, all other methods do enqueuings in a thread safe manner)
                DispatcherHelpers.RunOnDispatcher(pair.Key, pair.Value.queueInstance.OnSuspend, true); // inlining allowed
            }
        }

        /// <summary>
        /// Called when the host receives the resume event.
        /// </summary>
        public void OnResume()
        {
            _active = true;
            foreach (var pair in _dispatcherToOperationQueueInfo)
            {
                // Simulate an OnResume from the correct dispatcher thread
                // (OnResume/OnSuspend/OnDestroy have this thread affinity, all other methods do enqueuings in a thread safe manner)
                DispatcherHelpers.RunOnDispatcher(pair.Key, pair.Value.queueInstance.OnResume, true); // inlining allowed
            }
        }

        /// <summary>
        /// Called when the host is shutting down.
        /// </summary>
        public void OnDestroy()
        {
            _active = false;
            foreach (var pair in _dispatcherToOperationQueueInfo)
            {
                // Simulate an OnDestroy from the correct dispatcher thread
                // (OnResume/OnSuspend/OnDestroy have this thread affinity, all other methods do enqueuings in a thread safe manner)
                DispatcherHelpers.RunOnDispatcher(pair.Key, pair.Value.queueInstance.OnDestroy, true); // inlining allowed
            }
        }

        /// <summary>
        /// Used by the native animated module to bypass the process of
        /// updating the values through the shadow view hierarchy. This method
        /// will directly update the native views, which means that updates for
        /// layout-related properties won't be handled properly.
        /// </summary>
        /// <param name="tag">The view tag.</param>
        /// <param name="props">The properties</param>
        /// <remarks>
        /// Make sure you know what you're doing before calling this method :)
        /// </remarks>
        public bool SynchronouslyUpdateViewOnDispatcherThread(int tag, ReactStylesDiffMap props)
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
                MainUIViewOperationQueue.NativeViewHierarchyManager.UpdateProperties(tag, props);
            }
            else
            {
                // Dispatch to the correct thread.
                DispatcherHelpers.RunOnDispatcher(queue.Dispatcher, CoreDispatcherPriority.High, () =>
                {
                    if (queue.NativeViewHierarchyManager.ViewExists(tag))
                    {
                        queue.NativeViewHierarchyManager.UpdateProperties(tag, props);
                    }
                    else
                    {
                        Debug.WriteLine($"View with tag {tag} not found due to race condition");
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
            //
            // We synchronize access to _reactTagToOperationQueue by using same action queue used by the other code paths.
            // Cleaning up can be done lazily, a way to mark this work unit as "Idle Scheduling" would benefit performance.
            if (_uiManagerActionQueue == null)
            {
                _uiManagerActionQueue = _reactContext.GetNativeModule<UIManagerModule>().ActionQueue;
            }

            _uiManagerActionQueue.Dispatch(() =>
            {
                foreach (var tag in tags)
                {
                    _reactTagToOperationQueue.Remove(tag);
                }
            });
        }
    }
}
