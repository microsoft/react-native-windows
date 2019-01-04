// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.Bridge;
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
            if (DispatcherHelpers.IsOnDispatcher())
            {
                SystemNavigationManager.GetForCurrentView().BackRequested += (sender, e) => OnBackRequested(host, sender, e);
                Window.Current.CoreWindow.Dispatcher.AcceleratorKeyActivated += (sender, e) => OnAcceleratorKeyActivated(host, sender, e);
            }
        }

        private static void OnBackRequested(ReactNativeHost host, object sender, BackRequestedEventArgs e)
        {
            if (host.HasInstance)
            {
                host.ReactInstanceManager.OnBackPressed();
                e.Handled = true;
            }
        }

        private static void OnAcceleratorKeyActivated(ReactNativeHost host, CoreDispatcher sender, AcceleratorKeyEventArgs e)
        {
            if (host.HasInstance)
            {
                var reactInstanceManager = host.ReactInstanceManager;
                if (reactInstanceManager.DevSupportManager.IsEnabled)
                {
                    if (e.VirtualKey == VirtualKey.Shift)
                    {
                        s_isShiftKeyDown = IsKeyDown(e.EventType);
                    }
                    else if (e.VirtualKey == VirtualKey.Control)
                    {
                        s_isControlKeyDown = IsKeyDown(e.EventType);
                    }
                    else if (IsKeyDown(e.EventType) && s_isShiftKeyDown && e.VirtualKey == VirtualKey.F10)
                    {
                        reactInstanceManager.DevSupportManager.ShowDevOptionsDialog();
                    }
                    else if (e.EventType == CoreAcceleratorKeyEventType.KeyUp && s_isControlKeyDown && e.VirtualKey == VirtualKey.R)
                    {
                        reactInstanceManager.DevSupportManager.HandleReloadJavaScript();
                    }
                }
            }
        }

        private static bool IsKeyDown(CoreAcceleratorKeyEventType t)
        {
            return t == CoreAcceleratorKeyEventType.KeyDown || t == CoreAcceleratorKeyEventType.SystemKeyDown;
        }
    }
}
