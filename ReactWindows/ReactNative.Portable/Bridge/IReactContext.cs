using System;

namespace ReactNative.Bridge
{
    public interface IReactContext
    {
        /// <summary>
        /// The React instance associated with the context.
        /// </summary>
        IReactInstance ReactInstance { get; }

        /// <summary>
        /// Checks if the context has an active <see cref="IReactInstance"/>.
        /// </summary>
        bool HasActiveReactInstance { get; }

        /// <summary>
        /// The exception handler for native module calls.
        /// </summary>
        Action<Exception> NativeModuleCallExceptionHandler { get; set; }

        /// <summary>
        /// Gets the instance of the <see cref="IJavaScriptModule"/> associated
        /// with the <see cref="IReactInstance"/>.
        /// </summary>
        /// <typeparam name="T">Type of JavaScript module.</typeparam>
        /// <returns>The JavaScript module instance.</returns>
        T GetJavaScriptModule<T>()
            where T : IJavaScriptModule;

        /// <summary>
        /// Gets the instance of the <see cref="INativeModule"/> associated
        /// with the <see cref="IReactInstance"/>.
        /// </summary>
        /// <typeparam name="T">Type of native module.</typeparam>
        /// <returns>The native module instance.</returns>
        T GetNativeModule<T>()
            where T : INativeModule;

        /// <summary>
        /// Adds a lifecycle event listener to the context.
        /// </summary>
        /// <param name="listener">The listener.</param>
        void AddLifecycleEventListener(ILifecycleEventListener listener);

        /// <summary>
        /// Removes a lifecycle event listener from the context.
        /// </summary>
        /// <param name="listener">The listener.</param>
        void RemoveLifecycleEventListener(ILifecycleEventListener listener);

        /// <summary>
        /// Called by the host when the application suspends.
        /// </summary>
        void OnSuspend();

        /// <summary>
        /// Called by the host when the application resumes.
        /// </summary>
        void OnResume();

        /// <summary>
        /// Called by the host when the application shuts down.
        /// </summary>
        void Dispose();

        /// <summary>
        /// Checks if the current thread is on the React instance dispatcher
        /// queue thread.
        /// </summary>
        /// <returns>
        /// <b>true</b> if the call is from the dispatcher queue thread,
        ///  <b>false</b> otherwise.
        /// </returns>
        bool IsOnDispatcherQueueThread();

        /// <summary>
        /// Asserts that the current thread is on the React instance dispatcher
        /// queue thread.
        /// </summary>
        void AssertOnDispatcherQueueThread();

        /// <summary>
        /// Enqueues an action on the dispatcher queue thread.
        /// </summary>
        /// <param name="action">The action.</param>
        void RunOnDispatcherQueueThread(Action action);

        /// <summary>
        /// Checks if the current thread is on the React instance layout
        /// queue thread.
        /// </summary>
        /// <returns>
        /// <b>true</b> if the call is from the layout queue thread,
        ///  <b>false</b> otherwise.
        /// </returns>
        bool IsOnLayoutQueueThread();

        /// <summary>
        /// Asserts that the current thread is on the React instance layout
        /// queue thread.
        /// </summary>
        void AssertOnLayoutQueueThread();

        /// <summary>
        /// Enqueues an action on the layout queue thread.
        /// </summary>
        /// <param name="action">The action.</param>
        void RunOnLayoutQueueThread(Action action);

        /// <summary>
        /// Checks if the current thread is on the React instance
        /// JavaScript queue thread.
        /// </summary>
        /// <returns>
        /// <b>true</b> if the call is from the JavaScript queue thread,
        /// <b>false</b> otherwise.
        /// </returns>
        bool IsOnJavaScriptQueueThread();

        /// <summary>
        /// Asserts that the current thread is on the React instance
        /// JavaScript queue thread.
        /// </summary>
        void AssertOnJavaScriptQueueThread();

        /// <summary>
        /// Enqueues an action on the JavaScript queue thread.
        /// </summary>
        /// <param name="action">The action.</param>
        void RunOnJavaScriptQueueThread(Action action);

        /// <summary>
        /// Checks if the current thread is on the React instance native 
        /// modules queue thread.
        /// </summary>
        /// <returns>
        /// <b>true</b> if the call is from the native modules queue thread,
        /// <b>false</b> otherwise.
        /// </returns>
        bool IsOnNativeModulesQueueThread();

        /// <summary>
        /// Asserts that the current thread is on the React instance native
        /// modules queue thread.
        /// </summary>
        void AssertOnNativeModulesQueueThread();

        /// <summary>
        /// Enqueues an action on the native modules queue thread.
        /// </summary>
        /// <param name="action">The action.</param>
        void RunOnNativeModulesQueueThread(Action action);

        /// <summary>
        /// Passes the exception to the current 
        /// <see cref="ReactContext.NativeModuleCallExceptionHandler"/>, if set, otherwise
        /// rethrows.
        /// </summary>
        /// <param name="exception"></param>
        void HandleException(Exception exception);
    }
}
