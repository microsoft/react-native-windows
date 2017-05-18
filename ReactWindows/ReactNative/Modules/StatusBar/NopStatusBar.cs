using System;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.UI;

namespace ReactNative.Modules.StatusBar
{
    class NopStatusBar : IStatusBar
    {
        public Color? BackgroundColor
        {
            get;
            set;
        }

        public double BackgroundOpacity
        {
            get;
            set;
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("AsyncUsage.CSharp.Reliability", "AvoidAsyncSuffix", Justification = "Intended name.")]
#pragma warning disable AvoidAsyncSuffix
        public IAsyncAction HideAsync()
#pragma warning restore AvoidAsyncSuffix
        {
            return Task.CompletedTask.AsAsyncAction();
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("AsyncUsage.CSharp.Reliability", "AvoidAsyncSuffix", Justification = "Intended name.")]
#pragma warning disable AvoidAsyncSuffix
        public IAsyncAction ShowAsync()
#pragma warning restore AvoidAsyncSuffix
        {
            return Task.CompletedTask.AsAsyncAction();
        }
    }
}
