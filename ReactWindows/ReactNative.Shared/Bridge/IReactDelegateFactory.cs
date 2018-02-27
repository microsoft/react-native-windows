// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;
using System.Reflection;

namespace ReactNative.Bridge
{
    /// <summary>
    /// Factory interface for manufacturing method invocation delegates.
    /// </summary>
    public interface IReactDelegateFactory
    {
        /// <summary>
        /// Extracts the native method type from the method.
        /// </summary>
        /// <param name="method">The method.</param>
        /// <returns>The native method type.</returns>
        string GetMethodType(MethodInfo method);

        /// <summary>
        /// Create an invocation delegate from the given method.
        /// </summary>
        /// <param name="nativeModule">The native module instance.</param>
        /// <param name="method">The method.</param>
        /// <returns>The invocation delegate.</returns>
        Func<INativeModule, IReactInstance, JArray, JToken> Create(INativeModule nativeModule, MethodInfo method);

        /// <summary>
        /// Check that the method is valid for <see cref="ReactMethodAttribute"/>.
        /// </summary>
        /// <param name="method">The method.</param>
        void Validate(MethodInfo method);
    }
}
