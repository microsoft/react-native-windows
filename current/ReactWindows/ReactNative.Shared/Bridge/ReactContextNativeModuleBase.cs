// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge.Queue;
using System;

namespace ReactNative.Bridge
{
    /// <summary>
    /// Base class for React Native modules that require access to the 
    /// <see cref="ReactContext"/>.
    /// </summary>
    public abstract class ReactContextNativeModuleBase : NativeModuleBase
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
        /// <param name="actionQueue">The action queue.</param>
        protected ReactContextNativeModuleBase(ReactContext reactContext, IActionQueue actionQueue)
            : base(actionQueue)
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
        /// Instantiates the <see cref="ReactContextNativeModuleBase"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="delegateFactory">The React method delegate factory.</param>
        /// <param name="actionQueue">The action queue.</param>
        protected ReactContextNativeModuleBase(ReactContext reactContext, IReactDelegateFactory delegateFactory, IActionQueue actionQueue)
            : base(delegateFactory, actionQueue)
        {
            if (reactContext == null)
                throw new ArgumentNullException(nameof(reactContext));

            Context = reactContext;
        }

        /// <summary>
        /// The React context.
        /// </summary>
        public ReactContext Context { get; }
    }
}
