using Facebook.CSSLayout;
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
        public static void SetBorderWidth(this Border border, CSSSpacingType kind, double width)
        {
            var thickness = border.BorderThickness;
            switch (kind)
            {
                case CSSSpacingType.Left:
                    thickness.Left = width;
                    break;
                case CSSSpacingType.Top:
                    thickness.Top = width;
                    break;
                case CSSSpacingType.Right:
                    thickness.Right = width;
                    break;
                case CSSSpacingType.Bottom:
                    thickness.Bottom = width;
                    break;
                case CSSSpacingType.All:
                    thickness = new Thickness(width);
                    break;
            }

#pragma warning disable WPF0041
            border.BorderThickness = thickness;
#pragma warning restore WPF0041
        }
    }
}
