// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge.Queue;
using System.Collections.Generic;

namespace ReactNative.Bridge
{
    /// <summary>
    /// A native module whose API can be provided to JavaScript React
    /// instances. 
    /// </summary>
    /// <remarks>
    /// <see cref="INativeModule"/>s whose implementation is written in C#
    /// should extend <see cref="NativeModuleBase"/> or
    /// <see cref="ReactContextNativeModuleBase"/>.
    /// </remarks>
    public interface INativeModule : IAsyncDisposable
    {
        /// <summary>
        /// The action queue used by the native module.
        /// </summary>
        /// <remarks>
        /// Can be <code>null</code>, in which case, the call is evaluated
        /// inline on the native modules action queue.
        /// </remarks>
        IActionQueue ActionQueue { get; }

        /// <summary>
        /// Return true if you intend to override some other native module that
        /// was registered, e.g., as part of a different package (such as the
        /// core one). Trying to override without returning true from this 
        /// method is considered an error and will throw an exception during
        /// initialization. By default, all modules return false.
        /// </summary>
        bool CanOverrideExistingModule { get; }

        /// <summary>
        /// The constants exported by this module.
        /// </summary>
        JObject Constants { get; }

        /// <summary>
        /// The methods callable from JavaScript on this module.
        /// </summary>
        IReadOnlyDictionary<string, INativeMethod> Methods { get; }

        /// <summary>
        /// The name of the module.
        /// </summary>
        /// <remarks>
        /// This will be the name used to <code>require()</code> this module
        /// from JavaScript.
        /// </remarks>
        string Name { get; }

        /// <summary>
        /// Called after the creation of a <see cref="IReactInstance"/>, in
        /// order to initialize native modules that require the React or
        /// JavaScript modules.
        /// </summary>
        void Initialize();
    }
}
