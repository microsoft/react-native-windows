// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;

namespace ReactNative.Bridge
{
    /// <summary>
    /// Extension methods for promised.
    /// </summary>
    public static class PromiseExtensions
    {
        private const string DefaultError = "EUNSPECIFIED";

        /// <summary>
        /// Report an error which wasn't caused by an exception.
        /// </summary>
        /// <param name="promise">The promise.</param>
        /// <param name="code">The error code.</param>
        /// <param name="message">The error message.</param>
        public static void Reject(this IPromise promise, string code, string message)
        {
            promise.Reject(code, message, default(Exception));
        }

        /// <summary>
        /// Report an error which wasn't caused by an exception.
        /// </summary>
        /// <param name="promise">The promise.</param>
        /// <param name="message">The error message.</param>
        /// <remarks>
        /// Using this method will pass the error code "EUNSPECIFIED".
        /// </remarks>
        public static void Reject(this IPromise promise, string message)
        {
            promise.Reject(DefaultError, message, default(Exception));
        }

        /// <summary>
        /// Report an exception.
        /// </summary>
        /// <param name="promise">The promise.</param>
        /// <param name="code">The error code.</param>
        /// <param name="exception">The exception.</param>
        public static void Reject(this IPromise promise, string code, Exception exception)
        {
            promise.Reject(code, exception.Message, exception);
        }

        /// <summary>
        /// Reject the promise with the given exception.
        /// </summary>
        /// <param name="promise">The promise.</param>
        /// <param name="exception">The exception.</param>
        /// <remarks>
        /// Using this method will pass the error code "EUNSPECIFIED".
        /// </remarks>
        public static void Reject(this IPromise promise, Exception exception)
        {
            if (exception == null)
                throw new ArgumentNullException(nameof(exception));

            promise.Reject(DefaultError, exception.Message, exception);
        }

        /// <summary>
        /// Report an exception with a custom error message.
        /// </summary>
        /// <param name="promise">The promise.</param>
        /// <param name="code">The error code.</param>
        /// <param name="message">The error message.</param>
        /// <param name="exception">The exception.</param>
        public static void Reject(this IPromise promise, string code, string message, Exception exception)
        {
            var errorData = exception?.Data;
            var userInfo = errorData != null
                ? JToken.FromObject(errorData)
                : null;
            promise.Reject(code, message, exception?.StackTrace, userInfo);
        }
    }
}
