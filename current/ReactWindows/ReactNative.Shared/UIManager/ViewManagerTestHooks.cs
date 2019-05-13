// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

#if WINDOWS_UWP
using Windows.UI.Xaml;
#else
using System.Windows;
#endif

namespace ReactNative.UIManager
{
    /// <summary>
    /// Class to expose hooks for testing ViewManagers
    /// </summary>
    public static class ViewManagerTestHooks
    {
        /// <summary>
        /// Enables or disable test hooks.
        /// </summary>
        public static bool Enabled { get; set; } = false;

        /// <summary>
        /// Occurs when when View.Loaded fires.
        /// </summary>
        public static event RoutedEventHandler ViewLoaded;

        /// <summary>
        /// Invokes the ViewLoaded event handler.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The event args.</param>
        public static void OnViewLoaded(object sender, RoutedEventArgs e)
        {
            ViewLoaded?.Invoke(sender, e);
        }
    }
}
