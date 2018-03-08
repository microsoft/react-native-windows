// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System.Collections.Generic;

namespace ReactNative.Collections
{
    static class JObjectExtensions
    {
        public static bool ContainsKey(this JObject obj, string key)
        {
            return ContainsKey<string, JToken>(obj, key);
        }

        private static bool ContainsKey<TKey, TValue>(
            IDictionary<TKey, TValue> d,
            TKey key)
        {
            return d.ContainsKey(key);
        }
    }
}
