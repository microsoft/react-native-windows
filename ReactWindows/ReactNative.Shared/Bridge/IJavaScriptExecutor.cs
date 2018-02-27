// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;

namespace ReactNative.Bridge
{
    /// <summary>
    /// The delagate to invoke a NativeModule synchronous hook.
    /// </summary>
    /// <param name="moduleId">The module ID.</param>
    /// <param name="methodId">The method ID.</param>
    /// <param name="arguments">The arguments.</param>
    /// <returns>The result</returns>
    public delegate JToken CallSerializableNativeHook(int moduleId, int methodId, JArray arguments);

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
        void SetGlobalVariable(string propertyName, JToken value);

        /// <summary>
        /// Runs the JavaScript at the given path.
        /// </summary>
        /// <param name="sourcePath">The source path.</param>
        /// <param name="sourceUrl">The source URL.</param>
        void RunScript(string sourcePath, string sourceUrl);

        /// <summary>
        /// Set a CallSerializableNativeHook delegate on the executor.
        /// </summary>
        /// <param name="callSerializableNativeHook">The delegate</param>
        void SetCallSerializableNativeHook(CallSerializableNativeHook callSerializableNativeHook);
    }
}
