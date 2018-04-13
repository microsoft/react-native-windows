// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using System;

namespace ReactNative.Bridge
{
    /// <summary>
    /// Extension methods for <see cref="IReactContext"/>.
    /// </summary>
    public static class ReactContextExtensions
    {
        /// <summary>
        /// Asserts that the current thread is on the React instance native
        /// modules queue thread.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        public static void AssertOnNativeModulesQueueThread(this IReactContext reactContext)
        {
            if (!reactContext.IsOnNativeModulesQueueThread())
            {
                throw new InvalidOperationException("Thread access assertion failed.");
            }
        }

        /// <summary>
        /// Asserts that the current thread is on the React instance JavaScript
        /// queue thread.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        public static void AssertOnJavaScriptQueueThread(this IReactContext reactContext)
        {
            if (!reactContext.IsOnJavaScriptQueueThread())
            {
                throw new InvalidOperationException("Thread access assertion failed.");
            }
        }

        /// <summary>
        /// Asserts that the current thread is on the React instance dispatcher
        /// queue thread.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        public static void AssertOnDispatcherQueueThread(this IReactContext reactContext)
        {
            if (!reactContext.IsOnDispatcherQueueThread())
            {
                throw new InvalidOperationException("Thread access assertion failed.");
            }
        }
    }
}

