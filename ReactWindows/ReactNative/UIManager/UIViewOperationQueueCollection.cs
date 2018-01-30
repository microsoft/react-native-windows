/// <summary>
/// NOTE: This file will be renamed to UIViewOperationsQueue.cs after code review.
/// </summary>

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;
using System.Collections.Generic;
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

        private readonly IDictionary<int, UIViewOperationQueueInstance> _reactTagToOperationQueue = new Dictionary<int, UIViewOperationQueueInstance>();
        private readonly IDictionary<CoreDispatcher, QueueInstanceInfo> _dispatcherToOperationQueueInfo = new Dictionary<CoreDispatcher, QueueInstanceInfo>();

        private bool _active;

        /// <summary>
        /// Instantiates the <see cref="UIViewOperationQueue"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// The native view hierarchy manager.
        /// </param>
        public UIViewOperationQueue(ReactContext reactContext, ViewManagerRegistry viewManagerRegistry)
        {
            _reactContext = reactContext;
            _viewManagerRegistry = viewManagerRegistry;
            _active = false;
        }

        /// <summary>
        /// The native view hierarchy manager.
        /// </summary>
        internal UIViewOperationQueueInstance FirstUIViewOperationQueue
        {
            get
            {
                // TODO MW: fixing this stuff
                return _mainUiViewOperationsQueueInstance;
            }
        }

        /// <summary>
        /// The native view hierarchy manager.
        /// </summary>
        internal NativeViewHierarchyManager NativeViewHierarchyManager
        {
            get
            {
                // TODO MW: fixing this stuff
                return FirstUIViewOperationQueue.NativeViewHierarchyManager;
            }
        }

        /// <summary>
        /// Checks if the operation queue is empty.
        /// </summary>
        /// <returns>
        /// <b>true</b> if the queue is empty, <b>false</b> otherwise.
        /// </returns>
        public bool IsEmpty()
        {
            // TODO MW: does it need lock?
            foreach (var queueInfo in _dispatcherToOperationQueueInfo.Values)
            {
                if (!queueInfo.queueInstance.IsEmpty())
                {
                    return false;
                }
            }

            return true;
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
            DispatcherHelpers.AssertOnDispatcher();

            // Extract dispatcher
            var currentDispatcher = rootView.Dispatcher;

            QueueInstanceInfo queueInfo;
            if (!_dispatcherToOperationQueueInfo.TryGetValue(currentDispatcher, out queueInfo))
            {
                // Queue instance doesn't exist for this dispatcher, we need to create
                queueInfo = new QueueInstanceInfo()
                {
                    queueInstance = new UIViewOperationQueueInstance(_reactContext, new NativeViewHierarchyManager(_viewManagerRegistry)),
                    rootViewCount = 1
                };

                _dispatcherToOperationQueueInfo[currentDispatcher] = queueInfo;

                if (currentDispatcher == DispatcherHelpers.MainDispatcher)
                {
                    _mainUiViewOperationsQueueInstance = queueInfo.queueInstance;
                }

                if (_active)
                {
                    DispatcherHelpers.CallOnDispatcherWithInlining<bool>(() =>
                    {
                        queueInfo.queueInstance.OnResume();
                        return true;
                    });
                }
            }
            else
            {
                queueInfo.rootViewCount++;
            }

            // Add tag
            _reactTagToOperationQueue[tag] = queueInfo.queueInstance;

            // Send forward
            queueInfo.queueInstance.AddRootView(tag, rootView, themedRootContext);
        }

        /// <summary>
        /// Enqueues an operation to remove the root view.
        /// </summary>
        /// <param name="rootViewTag">The root view tag.</param>
        public void EnqueueRemoveRootView(int rootViewTag)
        {
            UIViewOperationQueueInstance queue;
            if (!_reactTagToOperationQueue.TryGetValue(rootViewTag, out queue))
            {
                throw new InvalidOperationException("No queue for tag " + rootViewTag);
            }

            // Send forward
            queue.EnqueueRemoveRootView(rootViewTag);

            // Do some maintenance/cleanup if needed.
            // TODO MW: cleanup the core dispatcher to queue mapping
        }

        /// <summary>
        /// Enqueues an operation to dispatch a command.
        /// </summary>
        /// <param name="tag">The view tag.</param>
        /// <param name="commandId">The command identifier.</param>
        /// <param name="commandArgs">The command arguments.</param>
        public void EnqueueDispatchCommand(int tag, int commandId, JArray commandArgs)
        {
            UIViewOperationQueueInstance queue;
            if (!_reactTagToOperationQueue.TryGetValue(tag, out queue))
            {
                throw new InvalidOperationException("No queue for tag " + tag);
            }

            queue.EnqueueDispatchCommand(tag, commandId, commandArgs);
        }

        /// <summary>
        /// Enqueues an operation to update the extra data for a view.
        /// </summary>
        /// <param name="reactTag">The view tag.</param>
        /// <param name="extraData">The extra data.</param>
        public void EnqueueUpdateExtraData(int reactTag, object extraData)
        {
            UIViewOperationQueueInstance queue;
            if (!_reactTagToOperationQueue.TryGetValue(reactTag, out queue))
            {
                throw new InvalidOperationException("No queue for tag " + reactTag);
            }

            queue.EnqueueUpdateExtraData(reactTag, extraData);
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
            UIViewOperationQueueInstance queue;
            if (!_reactTagToOperationQueue.TryGetValue(tag, out queue))
            {
                throw new InvalidOperationException("No queue for tag " + tag);
            }

            queue.EnqueueShowPopupMenu(tag, items, error, success);
        }

        /// <summary>
        /// Enqueues a operation to execute a UIBlock.
        /// </summary>
        /// <param name="block">The UI block.</param>
        public void EnqueueUIBlock(IUIBlock block)
        {
            // TODO MW
        }

        /// <summary>
        /// Enqueues a operation to execute a UIBlock.
        /// </summary>
        /// <param name="block">The UI block.</param>
        public void PrependUIBlock(IUIBlock block)
        {
            // TODO MW
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
            UIViewOperationQueueInstance queue;
            if (!_reactTagToOperationQueue.TryGetValue(rootViewTag, out queue))
            {
                throw new InvalidOperationException("No queue for tag " + rootViewTag);
            }

            _reactTagToOperationQueue[viewReactTag] = queue;

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
            if (FirstUIViewOperationQueue != null)
            {
                FirstUIViewOperationQueue.EnqueueConfigureLayoutAnimation(config, success, error);
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
            UIViewOperationQueueInstance queue;
            if (!_reactTagToOperationQueue.TryGetValue(tag, out queue))
            {
                throw new InvalidOperationException("No queue for tag " + tag);
            }

            queue.EnqueueUpdateProperties(tag, className, props);
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
            UIViewOperationQueueInstance queue;
            if (!_reactTagToOperationQueue.TryGetValue(tag, out queue))
            {
                throw new InvalidOperationException("No queue for tag " + tag);
            }

            queue.EnqueueUpdateLayout(parentTag, tag, dimensions);
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
            UIViewOperationQueueInstance queue;
            if (!_reactTagToOperationQueue.TryGetValue(tag, out queue))
            {
                throw new InvalidOperationException("No queue for tag " + tag);
            }

            queue.EnqueueManageChildren(tag, indexesToRemove, viewsToAdd, tagsToDelete);
        }

        /// <summary>
        /// Enqueues an operation to set the children of a view.
        /// </summary>
        /// <param name="reactTag">The view to manage.</param>
        /// <param name="childrenTags">The children tags.</param>
        public void EnqueueSetChildren(int reactTag, int[] childrenTags)
        {
            UIViewOperationQueueInstance queue;
            if (!_reactTagToOperationQueue.TryGetValue(reactTag, out queue))
            {
                throw new InvalidOperationException("No queue for tag " + reactTag);
            }

            queue.EnqueueSetChildren(reactTag, childrenTags);
        }

        /// <summary>
        /// Enqueues an operation to measure the view.
        /// </summary>
        /// <param name="reactTag">The tag of the view to measure.</param>
        /// <param name="callback">The measurement result callback.</param>
        public void EnqueueMeasure(int reactTag, ICallback callback)
        {
            UIViewOperationQueueInstance queue;
            if (!_reactTagToOperationQueue.TryGetValue(reactTag, out queue))
            {
                throw new InvalidOperationException("No queue for tag " + reactTag);
            }

            queue.EnqueueMeasure(reactTag, callback);
        }

        /// <summary>
        /// Enqueues an operation to measure the view relative to the window.
        /// </summary>
        /// <param name="reactTag">The tag of the view to measure.</param>
        /// <param name="callback">The measurement result callback.</param>
        public void EnqueueMeasureInWindow(int reactTag, ICallback callback)
        {
            UIViewOperationQueueInstance queue;
            if (!_reactTagToOperationQueue.TryGetValue(reactTag, out queue))
            {
                throw new InvalidOperationException("No queue for tag " + reactTag);
            }

            queue.EnqueueMeasureInWindow(reactTag, callback);
        }

        /// <summary>
        /// Enqueues an operation to find a touch target.
        /// </summary>
        /// <param name="reactTag">The parent view to search from.</param>
        /// <param name="targetX">The x-coordinate of the touch event.</param>
        /// <param name="targetY">The y-coordinate of the touch event.</param>
        /// <param name="callback">The callback.</param>
        public void EnqueueFindTargetForTouch(
            int reactTag,
            double targetX,
            double targetY,
            ICallback callback)
        {
            UIViewOperationQueueInstance queue;
            if (!_reactTagToOperationQueue.TryGetValue(reactTag, out queue))
            {
                throw new InvalidOperationException("No queue for tag " + reactTag);
            }

            queue.EnqueueFindTargetForTouch(reactTag, targetX, targetY, callback);
        }

        /// <summary>
        /// Called when the host receives the suspend event.
        /// </summary>
        public async void OnSuspend()
        {
            // TODO MW
            _active = false;
            foreach (var item in _dispatcherToOperationQueueInfo)
            {
                await item.Key.RunAsync(CoreDispatcherPriority.Normal, () => { item.Value.queueInstance.OnSuspend(); }
                ).AsTask().ConfigureAwait(false);
            }
        }

        /// <summary>
        /// Called when the host receives the resume event.
        /// </summary>
        public async void OnResume()
        {
            // TODO MW
            _active = true;
            foreach (var item in _dispatcherToOperationQueueInfo)
            {
                await item.Key.RunAsync(CoreDispatcherPriority.Normal, () => {
                    item.Value.queueInstance.OnResume();
                }
                ).AsTask().ConfigureAwait(false);
            }
        }

        /// <summary>
        /// Called when the host is shutting down.
        /// </summary>
        public void OnDestroy()
        {
            // TODO MW
        }

        /// <summary>
        /// Dispatches the view updates.
        /// </summary>
        /// <param name="batchId">The batch identifier.</param>
        internal void DispatchViewUpdates(int batchId)
        {
            foreach (var queue in _dispatcherToOperationQueueInfo.Values)
            {
                queue.queueInstance.DispatchViewUpdates(batchId);
            }
        }
    }
}
