// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Collections.Generic;

namespace Microsoft.ReactNative.Managed.Linq
{
  static class ReactEnumerableExtensions
  {
    public static IEnumerable<TSource> Append<TSource>(this IEnumerable<TSource> source, TSource element)
    {
      foreach (var item in source)
      {
        yield return item;
      }

      yield return element;
    }
  }
}
