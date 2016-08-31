using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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

            var minIndex = 0;
            for (var i = 1; i < list.Count; ++i)
            {
                if (f(list[i]) > f(list[minIndex]))
                {
                    break; 
                }

                minIndex = i;
            }

            return list[minIndex];
        }
    }
}
