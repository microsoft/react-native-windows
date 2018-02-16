// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;

namespace ReactNative.Tests
{
    class MockPromise : IPromise
    {
        private const string DefaultError = "EUNSPECIFIED";

        private readonly Action<object> _resolve;
        private readonly Action<string, string, string, JToken> _reject;
        
        public MockPromise(Action<object> resolve)
            : this(resolve, (_, __, ___, ____)=> { })
        {
        }

        public MockPromise(Action<object> resolve, Action<string, string, string, JToken> reject)
        {
            _resolve = resolve;
            _reject = reject;
        }

        public void Reject(string code, string message)
        {
            Reject(code, message, default(Exception));
        }

        public void Reject(string message)
        {
            Reject(DefaultError, message, default(Exception));
        }

        public void Reject(string code, Exception e)
        {
            Reject(code, e.Message, e);
        }

        public void Reject(Exception exception)
        {
            Reject(DefaultError, exception.Message, exception);
        }

        public void Reject(string code, string message, Exception e)
        {
            var errorData = e?.Data;
            var userInfo = errorData != null
                ? JToken.FromObject(errorData)
                : null;
            Reject(code, message, e?.StackTrace, userInfo);
        }

        public void Reject(string code, string message, string stack, JToken userInfo)
        {
            _reject.Invoke(code, message, stack, userInfo);
        }

        public void Resolve(object value)
        {
            _resolve(value);
        }
    }
}
