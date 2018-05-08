// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;

namespace ReactNative.Bridge
{
    class Promise : IPromise
    {
        private const string DefaultError = "EUNSPECIFIED";

        private readonly ICallback _resolve;
        private readonly ICallback _reject;

        public Promise(ICallback resolve, ICallback reject)
        {
            _resolve = resolve;
            _reject = reject;
        }

        public void Resolve(object value)
        {
            _resolve?.Invoke(value);
        }

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
