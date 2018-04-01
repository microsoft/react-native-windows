using System;

namespace ReactNative.Modules.Core
{
    public interface IReactChoreographer
    {
        /// <summary>
        /// For use by <see cref="UIManager.UIManagerModule"/>. 
        /// </summary>
        event EventHandler<IMutableFrameEventArgs> DispatchUICallback;

        /// <summary>
        /// For use by <see cref="Animated.NativeAnimatedModule"/>. 
        /// </summary>
        event EventHandler<IMutableFrameEventArgs> NativeAnimatedCallback;

        /// <summary>
        /// For events that make JavaScript do things.
        /// </summary>
        event EventHandler<IMutableFrameEventArgs> JavaScriptEventsCallback;

        /// <summary>
        /// Event used to trigger the idle callback. Called after all UI work has been
        /// dispatched to JavaScript.
        /// </summary>
        event EventHandler<IMutableFrameEventArgs> IdleCallback;

        /// <summary>
        /// Activate the callback for the given key.
        /// </summary>
        /// <param name="callbackKey">The callback key.</param>
        void ActivateCallback(string callbackKey);

        /// <summary>
        /// Deactivate the callback for the given key.
        /// </summary>
        /// <param name="callbackKey">The callback key.</param>
        void DeactivateCallback(string callbackKey);
    }
}
