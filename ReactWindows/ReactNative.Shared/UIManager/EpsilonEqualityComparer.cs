using System;
using System.Collections.Generic;

namespace ReactNative.UIManager
{
    class EpsilonEqualityComparer : IEqualityComparer<float>
    {
        private const float Epsilon = .00001f;
        private const int Decimals = 4;

        public static EpsilonEqualityComparer Instance { get; } = new EpsilonEqualityComparer();

        public bool Equals(float x, float y)
        {
            if (float.IsNaN(x) || float.IsNaN(y))
            {
                return float.IsNaN(x) && float.IsNaN(y);
            }

            return Math.Abs(x - y) < Epsilon;
        }

        public int GetHashCode(float obj)
        {
            return Math.Round(obj, Decimals).GetHashCode();
        }
    }
}
