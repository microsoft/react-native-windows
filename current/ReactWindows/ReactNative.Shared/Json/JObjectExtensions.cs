// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;

namespace ReactNative.Json
{
    static class JObjectExtensions
    {
        public static bool ContainsKey(this JObject obj, string key)
        {
            return ((IDictionary<string, JToken>)obj).ContainsKey(key);
        }

        public static JToken GetProperty(this JObject obj, string key)
        {
            return obj[key];
        }

        public static object GetProperty(this JObject obj, string key, Type type)
        {
            return obj[key].ToObject(type);
        }

        public static T GetProperty<T>(this JObject obj, string key)
        {
            return obj[key].ToObject<T>();
        }

        public static bool IsNull(this JObject obj, string key)
        {
            var property = obj[key];
            return property == null
                || property.Type == JTokenType.Null
                || property.Type == JTokenType.Undefined;
        }

        public static ICollection<string> Keys(this JObject obj)
        {
            return ((IDictionary<string, JToken>)obj).Keys;
        }
    }
}
