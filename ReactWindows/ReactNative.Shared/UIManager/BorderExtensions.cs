// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
#else
using System.Windows;
using System.Windows.Controls;
#endif

namespace ReactNative.UIManager
{
    static class BorderExtensions
    {
        public static void SetBorderWidth(this BorderedCanvas borderedCanvas, int spacingType, double width)
        {
            var thickness = borderedCanvas.BorderThickness.GetUpdated(spacingType, width);
            borderedCanvas.BorderThickness = thickness;
        }

        public static void SetBorderWidth(this Border border, int spacingType, double width)
        {
            var thickness = border.BorderThickness.GetUpdated(spacingType, width);
            border.BorderThickness = thickness;
        }

        private static Thickness GetUpdated(this Thickness thickness, int spacingType, double width)
        {
            switch (spacingType)
            {
                case EdgeSpacing.Left:
                    thickness.Left = width;
                    break;
                case EdgeSpacing.Top:
                    thickness.Top = width;
                    break;
                case EdgeSpacing.Right:
                    thickness.Right = width;
                    break;
                case EdgeSpacing.Bottom:
                    thickness.Bottom = width;
                    break;
                case EdgeSpacing.All:
                    thickness = new Thickness(width);
                    break;
            }
            return thickness;
        }
    }
}
