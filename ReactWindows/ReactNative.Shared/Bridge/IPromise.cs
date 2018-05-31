// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;

namespace ReactNative.Bridge
{
    /// <summary>
    /// Interface that represents a JavaScript Promise which can be passed to 
    /// the native module as a method parameter.
    /// </summary>
    /// <remarks>
    /// Methods annotated with <see cref="ReactMethodAttribute"/> that use 
    /// <see cref="IPromise"/> as type of the last parameter will be marked as
    /// "remoteAsync" and will return a promise when invoked from JavaScript.
    /// </remarks>
    public interface IPromise
    {
        /// <summary>
        /// Resolve the promise with the given value.
        /// </summary>
        /// <param name="value">The value.</param>
        void Resolve(object value);

        /// <summary>
        /// Report an error by explicitly specifying all of the fields of the error.
        /// </summary>
        /// <param name="code">The error code.</param>
        /// <param name="message">The error message.</param>
        /// <param name="stack">A string representing the frames on the call stack. Usually you get this off of an Exception object.</param>
        /// <param name="userInfo">User-defined information about the error. This is usually a collection of key-value pairs that provides additional error details.</param>
        void Reject(string code, string message, string stack, JToken userInfo);
    }
}
