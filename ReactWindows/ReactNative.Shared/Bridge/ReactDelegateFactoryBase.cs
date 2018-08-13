// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;

namespace ReactNative
{
    /// <summary>
    /// Base implementation for <see cref="IReactDelegateFactory"/>.
    /// </summary>
    public abstract class ReactDelegateFactoryBase : IReactDelegateFactory
    {
        /// <summary>
        /// Discriminator for asynchronous methods.
        /// </summary>
        public const string AsyncMethodType = "async";

        /// <summary>
        /// Discriminator for synchronous methods.
        /// </summary>
        public const string SyncMethodType = "sync";

        /// <summary>
        /// Discriminator for methods with promises.
        /// </summary>
        public const string PromiseMethodType = "promise";
        
        /// <summary>
        /// Instantiates a <see cref="ReactDelegateFactoryBase"/>.
        /// </summary>
        protected ReactDelegateFactoryBase() { }

        /// <summary>
        /// Create an invocation delegate from the given method.
        /// </summary>
        /// <param name="nativeModule">The native module instance.</param>
        /// <param name="method">The method.</param>
        /// <returns>The invocation delegate.</returns>
        public abstract Func<InvokeCallback, JArray, JToken> Create(INativeModule nativeModule, MethodInfo method);

        /// <summary>
        /// Extracts the native method type from the method.
        /// </summary>
        /// <param name="method">The method.</param>
        /// <param name="attribute">The attribute.</param>
        /// <returns>The native method type.</returns>
        public string GetMethodType(MethodInfo method, ReactMethodAttribute attribute)
        {
            if (attribute.IsBlockingSynchronousMethod)
            {
                return SyncMethodType;
            }

            var parameters = method.GetParameters();
            if (parameters.Length > 0 && parameters.Last().ParameterType == typeof(IPromise))
            {
                return PromiseMethodType;
            }

            return AsyncMethodType;
        }

        /// <summary>
        /// Check that the method is valid for <see cref="ReactMethodAttribute"/>.
        /// </summary>
        /// <param name="method">The method.</param>
        /// <param name="attribute">The attribute.</param>
        public void Validate(MethodInfo method, ReactMethodAttribute attribute)
        {
            var returnType = method.ReturnType;
            if (!attribute.IsBlockingSynchronousMethod && returnType != typeof(void))
            {
                throw new NotSupportedException("Native module methods must either return void or Task.");
            }

            var parameters = method.GetParameters();
            var n = parameters.Length;
            for (var i = 0; i < n; ++i)
            {
                var parameterType = parameters[i].ParameterType;
                if (parameterType == typeof(IPromise) && i != (n - 1))
                {
                    throw new NotSupportedException("Promises are only supported as the last parameter of a native module method.");
                }
                else if (parameterType == typeof(ICallback) && i != (n - 1))
                {
                    if (i != (n - 2) || parameters[n - 1].ParameterType != typeof(ICallback))
                    {
                        throw new NotSupportedException("Callbacks are only supported in the last two positions of a native module method.");
                    }
                }
            }
        }

        /// <summary>
        /// Create a callback.
        /// </summary>
        /// <param name="callbackToken">The callback ID token.</param>
        /// <param name="invokeCallback">The invoke callback delegate.</param>
        /// <returns>The callback.</returns>
        protected static ICallback CreateCallback(JToken callbackToken, InvokeCallback invokeCallback)
        {
            var id = callbackToken.Value<int>();
            return new Callback(id, invokeCallback);
        }

        /// <summary>
        /// Create a promise.
        /// </summary>
        /// <param name="resolveToken">The resolve callback ID token.</param>
        /// <param name="rejectToken">The reject callback ID token.</param>
        /// <param name="invokeCallback">The invoke callback delegate.</param>
        /// <returns>The promise.</returns>
        protected static IPromise CreatePromise(JToken resolveToken, JToken rejectToken, InvokeCallback invokeCallback)
        {
            var resolveCallback = CreateCallback(resolveToken, invokeCallback);
            var rejectCallback = CreateCallback(rejectToken, invokeCallback);
            return new Promise(resolveCallback, rejectCallback);
        }

        class Callback : ICallback
        {
            private static readonly object[] s_empty = new object[0];

            private readonly int _id;
            private readonly InvokeCallback _invokeCallback;

            public Callback(int id, InvokeCallback invokeCallback)
            {
                _id = id;
                _invokeCallback = invokeCallback;
            }

            public void Invoke(params object[] arguments)
            {
                _invokeCallback(_id, JArray.FromObject(arguments ?? s_empty));
            }
        }
    }
}
