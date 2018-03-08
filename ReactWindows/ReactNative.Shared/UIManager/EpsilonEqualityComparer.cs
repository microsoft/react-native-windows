// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Facebook.Yoga;
using System;
using System.Collections.Generic;

namespace ReactNative.UIManager
{
    class EpsilonEqualityComparer : IEqualityComparer<YogaValue>
    {
        private const float Epsilon = .00001f;
        private const int Decimals = 4;

        public static EpsilonEqualityComparer Instance { get; } = new EpsilonEqualityComparer();

        public bool Equals(YogaValue x, YogaValue y)
        {
            if (x.Unit != y.Unit)
            {
                return false;
            }

            if (float.IsNaN(x.Value) || float.IsNaN(y.Value))
            {
                return float.IsNaN(x.Value) && float.IsNaN(y.Value);
            }

            return Math.Abs(x.Value - y.Value) < Epsilon;
        }

        public int GetHashCode(YogaValue obj)
        {
            return (Math.Round(obj.Value, Decimals).GetHashCode()*397) ^ (int) obj.Unit;
        }
    }
}
