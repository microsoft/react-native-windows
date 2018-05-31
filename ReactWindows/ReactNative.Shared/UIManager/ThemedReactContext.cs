// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge;
using System;

namespace ReactNative.UIManager
{
    /// <summary>
    /// A wrapper <see cref="ReactContext"/> that delegates lifecycle events to
    /// the original instance of <see cref="ReactContext"/>.
    /// </summary>
    public class ThemedReactContext
    {
        private readonly ReactContext _reactContext;

        /// <summary>
        /// Instantiates the <see cref="ThemedReactContext"/>.
        /// </summary>
        /// <param name="reactContext">The inner context.</param>
        public ThemedReactContext(ReactContext reactContext)
        {
            if (reactContext == null)
            {
                throw new ArgumentNullException(nameof(reactContext));   
            }

            _reactContext = reactContext;
        }

        /// <summary>
        /// Gets the instance of the <see cref="INativeModule"/> associated
        /// with the <see cref="IReactInstance"/>.
        /// </summary>
        /// <typeparam name="T">Type of native module.</typeparam>
        /// <returns>The native module instance.</returns>
        public T GetNativeModule<T>()
            where T : INativeModule
        {
            return _reactContext.GetNativeModule<T>();
        }

        /// <summary>
        /// Gets the instance of the <see cref="IJavaScriptModule"/> associated
        /// with the <see cref="IReactInstance"/>.
        /// </summary>
        /// <typeparam name="T">Type of JavaScript module.</typeparam>
        /// <returns>The JavaScript module instance.</returns>
        public T GetJavaScriptModule<T>()
            where T : IJavaScriptModule, new()
        {
            return _reactContext.GetJavaScriptModule<T>();
        }

        /// <summary>
        /// Adds a lifecycle event listener to the context.
        /// </summary>
        /// <param name="listener">The listener.</param>
        public void AddLifecycleEventListener(ILifecycleEventListener listener)
        {
            _reactContext.AddLifecycleEventListener(listener);
        }

        /// <summary>
        /// Removes a lifecycle event listener from the context.
        /// </summary>
        /// <param name="listener">The listener.</param>
        public void RemoveLifecycleEventListener(ILifecycleEventListener listener)
        {
            _reactContext.RemoveLifecycleEventListener(listener);
        }
    }
}
