
using ReactNative.Bridge;
using ReactNative.Modules.Core;
/// <summary>
/// NOTE: This file will be renamed to UIViewOperationsQueue.cs after code review.
/// </summary>
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
            base(reactContext, new NativeViewHierarchyManager(viewManagers, DispatcherHelpers.MainDispatcher), ReactChoreographer.Instance)
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
    }
}
