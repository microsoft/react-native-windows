using System.Threading;
using Windows.System;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;

namespace ReactNative
{
    static class ReactRootViewExtensions
    {
        private static bool s_isShiftKeyDown;
        private static bool s_isControlKeyDown;

        public static void OnCreate(this ReactRootView rootView, ReactNativeHost host)
        {
            rootView.Background = (Brush)Application.Current.Resources["ApplicationPageBackgroundThemeBrush"];
            SystemNavigationManager.GetForCurrentView().BackRequested += (sender, e) => OnBackRequested(host, sender, e);
            Window.Current.CoreWindow.Dispatcher.AcceleratorKeyActivated += (sender, e) => OnAcceleratorKeyActivated(host, sender, e);
        }

        private static void OnBackRequested(ReactNativeHost host, object sender, BackRequestedEventArgs e)
        {
            if (host.HasInstance)
            {
                host.ReactInstanceManager.OnBackPressed();
                e.Handled = true;
            }
        }

        private static async void OnAcceleratorKeyActivated(ReactNativeHost host, CoreDispatcher sender, AcceleratorKeyEventArgs e)
        {
            if (host.HasInstance)
            {
                var reactInstanceManager = host.ReactInstanceManager;
                if (reactInstanceManager.DevSupportManager.IsEnabled)
                {
                    if (e.VirtualKey == VirtualKey.Shift)
                    {
                        s_isShiftKeyDown = e.EventType == CoreAcceleratorKeyEventType.KeyDown;
                    }
                    else if (e.VirtualKey == VirtualKey.Control)
                    {
                        s_isControlKeyDown = e.EventType == CoreAcceleratorKeyEventType.KeyDown;
                    }
                    else if ((s_isShiftKeyDown && e.VirtualKey == VirtualKey.F10) ||
                              (e.EventType == CoreAcceleratorKeyEventType.KeyDown && e.VirtualKey == VirtualKey.Menu))
                    {
                        reactInstanceManager.DevSupportManager.ShowDevOptionsDialog();
                    }
                    else if (e.EventType == CoreAcceleratorKeyEventType.KeyUp && s_isControlKeyDown && e.VirtualKey == VirtualKey.R)
                    {
                        await reactInstanceManager.DevSupportManager.CreateReactContextFromPackagerAsync(CancellationToken.None).ConfigureAwait(false);
                    }
                }
            }
        }
    }
}
