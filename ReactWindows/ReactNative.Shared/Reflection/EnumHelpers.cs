using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
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
            var lookup = s_enumCache.GetOrAdd(
                typeof(T),
                type => EnumToDictionary(type));

            var result = default(object);
            if (!lookup.TryGetValue(Normalize(value), out result))
            {
                throw new ArgumentOutOfRangeException(
                    nameof(value),
                    Invariant($"Invalid value '{value}' for type '{typeof(T)}'."));
            }

            return (T)result;
        }

        public static T? ParseNullable<T>(string value)
            where T : struct
        {
            if (value == null)
                return null;

            return Parse<T>(value);
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
