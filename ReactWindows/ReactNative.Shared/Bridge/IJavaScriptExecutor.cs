// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;

namespace ReactNative.Bridge
{
    /// <summary>
    /// Interface for making JavaScript calls from native code.
    /// </summary>
    public interface IJavaScriptExecutor : IDisposable
    {
        /// <summary>
        /// Call the JavaScript method from the given module.
        /// </summary>
        /// <param name="moduleName">The module name.</param>
        /// <param name="methodName">The method name.</param>
        /// <param name="arguments">The arguments.</param>
        /// <returns>The flushed queue of native operations.</returns>
        JToken CallFunctionReturnFlushedQueue(string moduleName, string methodName, JArray arguments);

        /// <summary>
        /// Invoke the JavaScript callback.
        /// </summary>
        /// <param name="callbackId">The callback identifier.</param>
        /// <param name="arguments">The arguments.</param>
        /// <returns>The flushed queue of native operations.</returns>
        JToken InvokeCallbackAndReturnFlushedQueue(int callbackId, JArray arguments);

        /// <summary>
        /// Invoke the React 'flushedQueue' function.
        /// </summary>
        /// <returns>The flushed queue of native operations.</returns>
        JToken FlushedQueue();

        /// <summary>
        /// Sets a global variable in the JavaScript runtime.
        /// </summary>
        /// <param name="propertyName">The global variable name.</param>
        /// <param name="value">The value.</param>
        void SetGlobalVariable(string propertyName, string value);

        /// <summary>
        /// Runs the JavaScript at the given path.
        /// </summary>
        /// <param name="sourcePath">The source path.</param>
        /// <param name="sourceUrl">The source URL.</param>
        void RunScript(string sourcePath, string sourceUrl);

        /// <summary>
        /// Sets a callback for immediate native method call queue flush.
        /// </summary>
        /// <param name="flushQueueImmediate">The callback.</param>
        void SetFlushQueueImmediate(Action<JToken> flushQueueImmediate);

        /// <summary>
        /// Sets a callback for synchronous native methods.
        /// </summary>
        /// <param name="callSyncHook">The sync hook for native methods.</param>
        void SetCallSyncHook(Func<int, int, JArray, JToken> callSyncHook);
    }
}
