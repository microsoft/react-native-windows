using Windows.Foundation;
using Windows.UI;
using static Windows.UI.ViewManagement.StatusBar;

namespace ReactNative.Modules.StatusBar
{
    class MobileStatusBar : IStatusBar
    {
        public Color? BackgroundColor
        {
            get
            {
                return GetForCurrentView().BackgroundColor;
            }
            set
            {
                GetForCurrentView().BackgroundColor = value;
            }
        }

        public double BackgroundOpacity
        {
            get
            {
                return GetForCurrentView().BackgroundOpacity;
            }
            set
            {
                GetForCurrentView().BackgroundOpacity = value;
            }
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("AsyncUsage.CSharp.Reliability", "AvoidAsyncSuffix", Justification = "Reviewed.")]
#pragma warning disable AvoidAsyncSuffix
        public IAsyncAction HideAsync()
#pragma warning restore AvoidAsyncSuffix
        {
            return GetForCurrentView().HideAsync();
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("AsyncUsage.CSharp.Reliability", "AvoidAsyncSuffix", Justification = "Reviewed.")]
#pragma warning disable AvoidAsyncSuffix
        public IAsyncAction ShowAsync()
#pragma warning restore AvoidAsyncSuffix
        {
            return GetForCurrentView().ShowAsync();
        }
    }
}
