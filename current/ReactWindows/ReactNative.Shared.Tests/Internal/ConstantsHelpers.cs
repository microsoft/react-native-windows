// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;

namespace ReactNative.Tests
{
    static class ConstantsHelpers
    {
        public static JObject AsMap(this object value)
        {
            return value as JObject;
        }

        public static object GetValue(this object value, string key)
        {
            var map = value.AsMap();
            if (map == null)
            {
                return null;
            }

            return map.GetValue(key);
        }

        public static JObject GetMap(this object value, string key)
        {
            return GetValue(value, key).AsMap();
        }
    }
}
