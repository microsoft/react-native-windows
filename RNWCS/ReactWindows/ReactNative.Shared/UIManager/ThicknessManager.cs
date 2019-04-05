// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Facebook.Yoga;
#if WINDOWS_UWP
using Windows.UI.Xaml;
#else
using System.Windows;
#endif

namespace ReactNative.UIManager
{
    /// <summary>
    /// Manages thickness that can be specified either as a single value for all edges
    /// or as a separate value for each edge. Implementation is consistent with underlying
    /// UI layout engine (e.g. in handling ambiguous cases like specifying both specific edge
    /// value and the value for all edges).
    /// </summary>
    public class ThicknessManager
    {
        /*
         * This is a thin wrapper over EdgeSpacing class. We want to explictly reuse the
         * EdgeSpacing code to force consistency between Yoga layout behavior and XAML rendering
         * implementation. We also want to hide EdgeSpacing features not supported for thickness
         * like EdgeSpacing.Horizontal.
         */

        /// <summary>
        /// Thickness type that represents all edges.
        /// </summary>
        public const int All = EdgeSpacing.All;

        /// <summary>
        /// Thickness type that represents top edge.
        /// </summary>
        public const int Top = EdgeSpacing.Top;

        /// <summary>
        /// Thickness type that represents right edge.
        /// </summary>
        public const int Right = EdgeSpacing.Right;

        /// <summary>
        /// Thickness type that represents bottom edge.
        /// </summary>
        public const int Bottom = EdgeSpacing.Bottom;
        /// <summary>
        /// Thickness type that represents left edge.
        /// </summary>
        public const int Left = EdgeSpacing.Left;

        private EdgeSpacing _edgeSpacing = new EdgeSpacing();

        /// <summary>
        /// Set a thickness value.
        /// </summary>
        /// <param name="thicknessType">
        ///     One of <see cref="Top" />, <see cref="Right" />, <see cref="Bottom" />,
        ///     <see cref="Left" />, <see cref="All" />.
        /// </param>
        /// <param name="value">the value for specified thickness type.</param>
        public void Set(int thicknessType, double? value)
        {
            if (thicknessType != Top && thicknessType != Right && thicknessType != Bottom
                && thicknessType != Left && thicknessType != All)
            {
                throw new ArgumentOutOfRangeException(nameof(thicknessType));
            }
            var yogaValue = value.HasValue ? YogaValue.Point((float)value) : YogaValue.Undefined();
            _edgeSpacing.Set(thicknessType, yogaValue);
        }

        /// <summary>
        /// Get the thickness for a thickness type.
        /// This takes into account any default values that have been Set.
        /// </summary>
        /// <param name="thicknessType">
        ///     One of <see cref="Top" />, <see cref="Right" />,
        ///     <see cref="Bottom" />, <see cref="Left" />.
        /// </param>
        /// <returns>The thickness value.</returns>
        public double Get(int thicknessType)
        {
            if (thicknessType != Top && thicknessType != Right
                && thicknessType != Bottom && thicknessType != Left)
            {
                throw new ArgumentOutOfRangeException(nameof(thicknessType));
            }
            var yogaValue = _edgeSpacing.Get(thicknessType);
            return yogaValue.Unit == YogaUnit.Undefined ? default(double) : yogaValue.Value;
        }

        /// <summary>
        /// Converts this ThicknessManager to Thickness.
        /// </summary>
        /// <returns>The CornerRadius value.</returns>
        public Thickness AsThickness() => new Thickness(Get(Left), Get(Top), Get(Right), Get(Bottom));
    }
}
