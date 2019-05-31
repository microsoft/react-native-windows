// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using System;

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
        /// Called when a FrameworkElement is created
        /// </summary>
        public static Action<FrameworkElement> ViewCreatedTestHook { get; set; } = null;
    }
}
