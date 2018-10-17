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
    /// Manages corner radii that can be specified either as a single value for all corners
    /// or as a separate value for each corner. Implementation is consistent with underlying
    /// UI layout engine (e.g. in handling ambiguous cases like specifying both specific corner
    /// value and the value for all corners).
    /// </summary>
    public class CornerRadiusManager
    {
        /*
         * This is a thin wrapper over EdgeSpacing class. We want to explictly reuse the
         * EdgeSpacing code to force consistency between Yoga layout behavior and XAML rendering
         * implementation. We also want to hide EdgeSpacing features not supported for corner radius
         * like EdgeSpacing.Horizontal.
         *
         * Since EdgeSpacing constants are named against edges of a rectangle but corner radii
         * are named against corners, we use constants mapping like
         * CornerRadiusManager.TopLeft -> EdgeSpacing.Top
         * since it works like we want if only All and specific edges are used (i.e. no
         * Horizointal/Vertical are used).
         */

        /// <summary>
        /// Corner type that represents all corners.
        /// </summary>
        public const int All = EdgeSpacing.All;

        /// <summary>
        /// Corner type that represents top left corner.
        /// </summary>
        public const int TopLeft = EdgeSpacing.Top;

        /// <summary>
        /// Corner type that represents top right corner.
        /// </summary>
        public const int TopRight = EdgeSpacing.Right;

        /// <summary>
        /// Corner type that represents bottom right corner.
        /// </summary>
        public const int BottomRight = EdgeSpacing.Bottom;

        /// <summary>
        /// Corner type that represents bottom left corner.
        /// </summary>
        public const int BottomLeft = EdgeSpacing.Left;

        private EdgeSpacing _edgeSpacing = new EdgeSpacing();

        /// <summary>
        /// Set a corner radius value.
        /// </summary>
        /// <param name="cornerType">
        ///     One of <see cref="TopLeft" />, <see cref="TopRight" />, <see cref="BottomRight" />,
        ///     <see cref="BottomLeft" />, <see cref="All" />.
        /// </param>
        /// <param name="value">the value for specified corner type.</param>
        public void Set(int cornerType, double? value)
        {
            if (cornerType != TopLeft && cornerType != TopRight && cornerType != BottomRight
                && cornerType != BottomLeft && cornerType != All)
            {
                throw new ArgumentOutOfRangeException(nameof(cornerType));
            }
            var yogaValue = value.HasValue ? YogaValue.Point((float)value) : YogaValue.Undefined();
            _edgeSpacing.Set(cornerType, yogaValue);
        }

        /// <summary>
        /// Get the corner radius for a corner type.
        /// This takes into account any default values that have been Set.
        /// </summary>
        /// <param name="cornerType">
        ///     One of <see cref="TopLeft" />, <see cref="TopRight" />,
        ///     <see cref="BottomRight" />, <see cref="BottomLeft" />.
        /// </param>
        /// <returns>The radius value.</returns>
        public double Get(int cornerType)
        {
            if (cornerType != TopLeft && cornerType != TopRight
                && cornerType != BottomRight && cornerType != BottomLeft)
            {
                throw new ArgumentOutOfRangeException(nameof(cornerType));
            }
            var yogaValue = _edgeSpacing.Get(cornerType);
            return yogaValue.Unit == YogaUnit.Undefined ? default(double) : yogaValue.Value;
        }

        /// <summary>
        /// Converts this CornerRadiusManager to CornerRadius.
        /// </summary>
        /// <returns>The CornerRadius value.</returns>
        public CornerRadius AsCornerRadius() =>
            new CornerRadius(Get(TopLeft), Get(TopRight), Get(BottomRight), Get(BottomLeft));
    }
}
