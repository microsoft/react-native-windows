// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge.Queue;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace ReactNative.Bridge
{
    /// <summary>
    /// A wrapper class for native modules codegen.
    /// </summary>
    /// <typeparam name="T">Type of native module.</typeparam>
    public abstract class NativeModuleWrapperBase<T> : INativeModule, INativeModuleWrapper
        where T : INativeModule
    {
        /// <summary>
        /// Instantiates the <see cref="NativeModuleWrapperBase{T}"/>.
        /// </summary>
        /// <param name="module">The native module being wrapped.</param>
        public NativeModuleWrapperBase(T module)
        {
            if (module == null)
                throw new ArgumentNullException(nameof(module));

            Module = module;
        }

        /// <summary>
        /// The methods callable from JavaScript on this module.
        /// </summary>
        public abstract IReadOnlyDictionary<string, INativeMethod> Methods { get; }

        /// <summary>
        /// The action queue used by the native module.
        /// </summary>
        /// <remarks>
        /// Can be <code>null</code>, in which case, the call is evaluated
        /// inline on the native modules action queue.
        /// </remarks>
        public IActionQueue ActionQueue => Module.ActionQueue;

        /// <summary>
        /// Return true if you intend to override some other native module that
        /// was registered, e.g., as part of a different package (such as the
        /// core one). Trying to override without returning true from this 
        /// method is considered an error and will throw an exception during
        /// initialization. By default, all modules return false.
        /// </summary>
        public bool CanOverrideExistingModule => Module.CanOverrideExistingModule;

        /// <summary>
        /// The constants exported by this module.
        /// </summary>
        public JObject Constants => Module.Constants;

        /// <summary>
        /// The native module instance.
        /// </summary>
        public T Module { get; }

        /// <summary>
        /// The native module type.
        /// </summary>
        public Type ModuleType => typeof(T);

        /// <summary>
        /// The name of the module.
        /// </summary>
        public string Name => Module.Name;

        /// <summary>
        /// Called after the creation of a <see cref="IReactInstance"/>, in
        /// order to initialize native modules that require the React or
        /// JavaScript modules.
        /// </summary>
        public void Initialize() => Module.Initialize();

        /// <summary>
        /// Disposes the module before the <see cref="IReactInstance"/> is disposed.
        /// </summary>
        /// <returns>
        /// A task to await the dispose operation.
        /// </returns>
        public Task DisposeAsync() => Module.DisposeAsync();

        INativeModule INativeModuleWrapper.Module => Module;

        /// <summary>
        /// Convert a <see cref="JToken"/> to a derived type, with null checking.
        /// </summary>
        /// <typeparam name="TValue">Type of <see cref="JToken"/>.</typeparam>
        /// <param name="token">The JSON value.</param>
        /// <returns>The derived type value.</returns>
        protected static TValue CastJToken<TValue>(JToken token)
            where TValue : JToken
        {
            // If the object is null, or the JToken type is null or undefined (and the expected output type is not JValue), return null
            if (token == null || (typeof(TValue) != typeof(JValue) && (token.Type == JTokenType.Null || token.Type == JTokenType.Undefined)))
            {
                return null;
            }

            return (TValue)token;
        }

        /// <summary>
        /// Wrapper native method implementation.
        /// </summary>
        protected class NativeMethod : INativeMethod
        {
            private static readonly JToken s_null = JValue.CreateNull();

            private readonly Func<InvokeCallback, JArray, JToken> _func;

            /// <summary>
            /// Instantiates the <see cref="NativeMethod"/>.
            /// </summary>
            /// <param name="type">Type of native method.</param>
            /// <param name="action">Delegate to invoke the native method.</param>
            public NativeMethod(string type, Action<InvokeCallback, JArray> action)
                : this(type, (invokeCallback, args) => { action(invokeCallback, args); return s_null; })
            {
                if (action == null)
                    throw new ArgumentNullException(nameof(action));
            }

            /// <summary>
            /// Instantiates the <see cref="NativeMethod"/>.
            /// </summary>
            /// <param name="type">Type of native method.</param>
            /// <param name="func">Delegate to invoke the native method.</param>
            public NativeMethod(string type, Func<InvokeCallback, JArray, JToken> func)
            {
                if (type == null)
                    throw new ArgumentNullException(nameof(type));
                if (func == null)
                    throw new ArgumentNullException(nameof(func));
                
                Type = type;
                _func = func;
            }

            /// <summary>
            /// The type of method.
            /// </summary>
            public string Type { get; }

            /// <summary>
            /// Invoke the native method.
            /// </summary>
            /// <param name="reactInstance">The React instance.</param>
            /// <param name="arguments">The arguments.</param>
            /// <returns>The native method result.</returns>
            public JToken Invoke(InvokeCallback reactInstance, JArray arguments)
            {
                return _func(reactInstance, arguments);
            }
        }

        /// <summary>
        /// Default implementation of <see cref="ICallback"/>.
        /// </summary>
        protected class Callback : ICallback
        {
            private static readonly JArray s_empty = new JArray();

            private readonly int _id;
            private readonly InvokeCallback _invokeCallback;

            /// <summary>
            /// Instantiates the <see cref="Callback"/>.
            /// </summary>
            /// <param name="id">The callback ID.</param>
            /// <param name="invokeCallback">The React instance.</param>
            public Callback(int id, InvokeCallback invokeCallback)
            {
                if (invokeCallback == null)
                    throw new ArgumentNullException(nameof(invokeCallback));

                _id = id;
                _invokeCallback = invokeCallback;
            }

            /// <summary>
            /// Invokes the callback.
            /// </summary>
            /// <param name="arguments">The callback arguments.</param>
            public void Invoke(params object[] arguments)
            {
                _invokeCallback(_id, arguments != null ? JArray.FromObject(arguments) : s_empty);
            }
        }

        /// <summary>
        /// Default implementation of <see cref="IPromise"/>.
        /// </summary>
        protected class Promise : IPromise
        {
            private const string DefaultError = "EUNSPECIFIED";

            private readonly ICallback _resolve;
            private readonly ICallback _reject;

            /// <summary>
            /// Instantiates the <see cref="Promise"/>.
            /// </summary>
            /// <param name="resolve">The resolve callback.</param>
            /// <param name="reject">The reject callback.</param>
            public Promise(ICallback resolve, ICallback reject)
            {
                _resolve = resolve;
                _reject = reject;
            }

            /// <summary>
            /// Resolve the promise with the given value.
            /// </summary>
            /// <param name="value">The value.</param>
            public void Resolve(object value)
            {
                _resolve?.Invoke(value);
            }

            /// <summary>
            /// Report an error by explicitly specifying all of the fields of the error.
            /// </summary>
            /// <param name="code">The error code.</param>
            /// <param name="message">The error message.</param>
            /// <param name="stack">A string representing the frames on the call stack. Usually you get this off of an Exception object.</param>
            /// <param name="userInfo">User-defined information about the error. This is usually a collection of key-value pairs that provides additional error details.</param>
            public void Reject(string code, string message, string stack, JToken userInfo)
            {
                _reject?.Invoke(new JObject
                {
                    { "code", code ?? DefaultError },
                    { "message", message },
                    { "stack", stack },
                    { "userInfo", userInfo },
                });
            }
        }
    }
}
