// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.Bridge;
using ReactNative.Modules.Core;

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
        /// <param name="initialProps">The initial properties.</param>
        /// <param name="rootViewTag">Root view tag.</param>
        public void EnqueueCreateView(
            ThemedReactContext themedContext,
            int viewReactTag,
            string viewClassName,
            ReactStylesDiffMap initialProps,
            int rootViewTag)
        {
            EnqueueCreateView(
                themedContext,
                viewReactTag,
                viewClassName,
                initialProps);
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
            DispatcherHelpers.AssertOnDispatcher();

            // First check if the view exists, as the views are created in
            // batches, and native modules attempting to synchronously interact
            // with views may attempt to update properties before the batch has
            // been processed.
            if (NativeViewHierarchyManager.ViewExists(tag))
            {
                NativeViewHierarchyManager.UpdateProperties(tag, props);
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}
