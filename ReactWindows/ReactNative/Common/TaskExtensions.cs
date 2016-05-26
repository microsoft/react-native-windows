using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ReactNative.Common
{
    static class TaskExtensions
    {
        static async Task<B> SelectMany<A, B>(
            this Task<A> task,
            Func<A, Task<B>> function)
        {
            A a = await task;
            return await function(a);
        }

        public static async Task<V> SelectMany<T, U, V>(this Task<T> source, Func<T, Task<U>> selector, Func<T, U, V> resultSelector)
        {
            T t = await source;
            U u = await selector(t);
            return resultSelector(t, u);
        }

        public static async Task<U> Select<T, U>(this Task<T> source, Func<T, U> selector)
        {
            T t = await source;
            return selector(t);
        }

        public static async Task<T> Where<T>(this Task<T> source, Func<T, bool> predicate)
        {
            T t = await source;
            if (!predicate(t)) throw new OperationCanceledException();
            return t;
        }
    }
}
