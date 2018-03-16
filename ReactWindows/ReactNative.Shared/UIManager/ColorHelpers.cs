// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#if WINDOWS_UWP
using Windows.UI;
#else
using System.Windows.Media;
#endif

namespace ReactNative.UIManager
{
    /// <summary>
    /// Helper class for parsing color values.
    /// </summary>
    public static class ColorHelpers
    {
        /// <summary>
        /// Unsigned integer representation of transparent color.
        /// </summary>
        public const uint Transparent = 0x00FFFFFF;

        /// <summary>
        /// Parses a color from an unsigned integer.
        /// </summary>
        /// <param name="value">The unsigned integer color value.</param>
        /// <returns>The parsed color value.</returns>
        public static Color Parse(uint value)
        {
            var color = value;
            var b = (byte)color;
            color >>= 8;
            var g = (byte)color;
            color >>= 8;
            var r = (byte)color;
            color >>= 8;
            var a = (byte)color;
            return Color.FromArgb(a, r, g, b);
        }
    }
}
