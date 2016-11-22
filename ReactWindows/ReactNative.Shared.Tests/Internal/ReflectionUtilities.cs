using System;
using System.Collections.Generic;
using System.Reflection;

namespace ReactNative.Shared.Tests.Internal
{
    public static class ReflectionUtilities
    {
        public static Tuple<bool, List<string>> PublicInstancePropertiesEqual<T>(T self, T to, params string[] ignore) where T : class
        {
            var result = false;
            var differencesList = new List<string>();

            if (self != null && to != null)
            {
                var type = typeof(T);
                var ignoreList = new List<string>(ignore);
                foreach (var pi in type.GetProperties(System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.Instance))
                {
                    if (!ignoreList.Contains(pi.Name))
                    {
                        var selfValue = type.GetProperty(pi.Name).GetValue(self, null);
                        var toValue = type.GetProperty(pi.Name).GetValue(to, null);

                        if (selfValue != toValue && (selfValue == null || !selfValue.Equals(toValue)))
                        {
                            differencesList.Add(pi.Name);
                        }
                    }
                }

                result = (differencesList.Count == 0);
            }
            else
            {
                result = (self == to);
            }

            return new Tuple<bool, List<string>>(result, differencesList);
        }
    }
}
