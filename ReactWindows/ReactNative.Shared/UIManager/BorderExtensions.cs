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
        [System.Diagnostics.CodeAnalysis.SuppressMessage("WpfAnalyzers.DependencyProperties",
        "WPF0041:Set mutable dependency properties using SetCurrentValue.",
        Justification = "Border does not contain SetCurrentValue")]
        public static void SetBorderWidth(this Border border, int spacingType, double width)
        {
            var thickness = border.BorderThickness;
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

#pragma warning disable WPF0041
            border.BorderThickness = thickness;
#pragma warning restore WPF0041
        }
    }
}
