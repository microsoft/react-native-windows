// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.Serialization;
using static System.FormattableString;

namespace ReactNative.Reflection
{
    static class EnumHelpers
    {
        private static readonly ConcurrentDictionary<Type, IReadOnlyDictionary<string, object>> s_enumCache =
            new ConcurrentDictionary<Type, IReadOnlyDictionary<string, object>>();

        public static T Parse<T>(string value)
        {
            if (!TryParse(value, out T result))
            {
                throw new ArgumentOutOfRangeException(
                    nameof(value),
                    Invariant($"Invalid value '{value}' for type '{typeof(T)}'."));
            }
            return result;
        }

        public static T? ParseNullable<T>(string value)
            where T : struct
        {
            if (value == null)
                return null;

            return Parse<T>(value);
        }

        public static bool TryParse<T>(string value, out T result)
        {
            var lookup = s_enumCache.GetOrAdd(
                typeof(T),
                type => EnumToDictionary(type));

            if (!lookup.TryGetValue(Normalize(value), out var objectResult))
            {
                result = default(T);
                return false;
            }

            result = (T)objectResult;
            return true;
        }

        private static Dictionary<string, object> EnumToDictionary(Type type)
        {
            var result = new Dictionary<string, object>();

            var names = Enum.GetNames(type);
            var values = Enum.GetValues(type);

            for (int i = 0; i < values.Length; ++i)
            {
                string name = names[i];
                object value = values.GetValue(i);

                result.Add(Normalize(name), value);

                var enumMemberAttribute = type.GetField(name).GetCustomAttribute(typeof(EnumMemberAttribute), false);
                if (enumMemberAttribute != null)
                {
                    result.Add(Normalize(((EnumMemberAttribute)enumMemberAttribute).Value), value);
                }
            }

            return result;
        }

        private static string Normalize(string value)
        {
            return value.ToLowerInvariant().Replace("-", "");
        }
    }
}
