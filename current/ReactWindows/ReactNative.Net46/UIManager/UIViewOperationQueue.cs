// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using System;
using System.Threading.Tasks;

namespace ReactNative.UIManager
{
    /// <summary>
    /// This class acts as a buffer for command executed on
    /// <see cref="NativeViewHierarchyManager"/>. It exposes similar methods as
    /// mentioned classes but instead of executing commands immediately, it 
    /// enqueues those operations in a queue that is then flushed from 
    /// <see cref="UIManagerModule"/> once a JavaScript batch of UI operations
    /// is finished.
    /// WPF implementation derives from UIViewOperationQueueInstance
    /// </summary>
    public class UIViewOperationQueue : UIViewOperationQueueInstance
    {
        /// <summary>
        /// Instantiates the <see cref="UIViewOperationQueueInstance"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="viewManagers">
        /// The view manager registry.
        /// </param>
        public UIViewOperationQueue(ReactContext reactContext, ViewManagerRegistry viewManagers) :
            base(reactContext, new NativeViewHierarchyManager(viewManagers, DispatcherHelpers.MainDispatcher, null), ReactChoreographer.Instance)
        {
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
            EnqueueCreateView(
                themedContext,
                viewReactTag,
                viewClassName,
                initialProps);
        }

        /// <summary>
        /// Enqueues an operation to remove the root view.
        /// </summary>
        /// <param name="rootViewTag">The root view tag.</param>
        public Task RemoveRootViewAsync(int rootViewTag)
        {
            EnqueueRemoveRootView(rootViewTag);

            // WPF implementation is single-threaded as far as UI dispatcher threads are concerned.
            // We can call the callback safely at this point.
            return Task.CompletedTask;
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
            DispatcherHelpers.AssertOnDispatcher();

            // First check if the view exists, as the views are created in
            // batches, and native modules attempting to synchronously interact
            // with views may attempt to update props before the batch has
            // been processed.
            if (NativeViewHierarchyManager.ViewExists(tag))
            {
                NativeViewHierarchyManager.UpdateProps(tag, props);
                return true;
            }
            else
            {
                return false;
            }
        }

        /// <summary>
        /// Enqueues a operation to execute a UIBlock.
        /// </summary>
        /// <param name="block">The UI block.</param>
        /// <param name="tag">Optional react tag hint that triggers the choice of the dispatcher thread that executes the block .</param>
        /// <remarks>
        /// <paramref name="tag"/> is not used, always runs on main dispatcher.
        /// </remarks>
        public void EnqueueUIBlock(IUIBlock block, int? tag)
        {
            // Called on layout manager thread

            EnqueueUIBlock(block);
        }

        /// <summary>
        /// Enqueues a operation to execute a UIBlock.
        /// </summary>
        /// <param name="block">The UI block.</param>
        /// <param name="tag">Optional react tag hint that triggers the choice of the dispatcher thread that executes the block .</param>
        /// <remarks>
        /// <paramref name="tag"/> is not used, always runs on main dispatcher.
        /// </remarks>
        public void PrependUIBlock(IUIBlock block, int? tag)
        {
            // Called on layout manager thread

            PrependUIBlock(block);
        }
    }
}
