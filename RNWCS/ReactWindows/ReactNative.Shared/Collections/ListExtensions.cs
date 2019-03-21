// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;

namespace ReactNative.Collections
{
    static class ListExtensions
    {
        public static T LocalMin<T>(this IList<T> list, Func<T, double> f)
        {
            if (list.Count == 0)
            {
                throw new ArgumentOutOfRangeException(nameof(list));
            }

            var min = list[0];
            for (var i = 1; i < list.Count; ++i)
            {
                var current = list[i];
                if (f(current) > f(min))
                {
                    break; 
                }

                min = current;
            }

            return min;
        }
    }
}
