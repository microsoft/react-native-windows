using ReactNative.Bridge.Queue;
using System;

namespace ReactNative.Bridge
{
    /// <summary>
    /// Base class for React Native modules that require access to the 
    /// <see cref="ReactContext"/>.
    /// </summary>
    public abstract class ReactContextNativeModuleBase : NativeModuleBase, IHasActionQueue
    {
        /// <summary>
        /// Instantiates the <see cref="ReactContextNativeModuleBase"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        protected ReactContextNativeModuleBase(ReactContext reactContext)
        {
            if (reactContext == null)
                throw new ArgumentNullException(nameof(reactContext));

            Context = reactContext;
        }

        /// <summary>
        /// Instantiates the <see cref="ReactContextNativeModuleBase"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="delegateFactory">The React method delegate factory.</param>
        protected ReactContextNativeModuleBase(ReactContext reactContext, IReactDelegateFactory delegateFactory)
            : base(delegateFactory)
        {
            if (reactContext == null)
                throw new ArgumentNullException(nameof(reactContext));

            Context = reactContext;
        }

        /// <summary>
        /// The action queue used by the native module.
        /// </summary>
        /// <remarks>
        /// Defaults to the native modules action queue. If you override this getter,
        /// be sure to return the same IActionQueue instance each time this getter is
        /// accessed for a given module instance.
        /// </remarks>
        public virtual IActionQueue ActionQueue
        {
            get
            {
                return Context.IndisposableNativeModulesQueue;
            }
        }

        /// <summary>
        /// Disposes the ActionQueue.
        /// </summary>
        /// <remarks>
        /// If you override <see cref="ActionQueue" /> to return a custom action
        /// queue, you should also override this method to ensure your action
        /// queue is disposed properly. This method runs on the native modules
        /// queue.
        /// 
        /// Note that <see cref="INativeModule.OnReactInstanceDispose" />
        /// isn't the appropriate place to dispose your action queue because
        /// that means you'll dispose your action queue while running on
        /// your action queue.
        /// </remarks>
        public virtual void DisposeActionQueue()
        {
        }

        /// <summary>
        /// The React context.
        /// </summary>
        public ReactContext Context { get; }
    }
}
