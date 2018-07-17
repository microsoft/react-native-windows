// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge;
using System;
#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
#else
using System.Windows;
using System.Windows.Controls;
#endif

namespace ReactNative.UIManager
{
    /// <summary>
    /// allows registering for size change events. The main purpose for this class is to hide complexity of ReactRootView
    /// </summary>
    public class SizeMonitoringCanvas : Canvas
    {
        private SizeChangedEventHandler _sizeChangedEventHandler;

        /// <summary>
        /// Sets and registers the event handler responsible for monitoring
        /// size change events.
        /// </summary>
        /// <param name="sizeChangedEventHandler">The event handler.</param>
        public void SetOnSizeChangedListener(SizeChangedEventHandler sizeChangedEventHandler)
        {
            DispatcherHelpers.AssertOnDispatcher(this);

            var current = _sizeChangedEventHandler;
            if (current != null)
            {
                SizeChanged -= current;
            }

            if (sizeChangedEventHandler != null)
            {
                _sizeChangedEventHandler = sizeChangedEventHandler;
                SizeChanged += _sizeChangedEventHandler;
            }
        }

        /// <summary>
        /// Unsets the size changed event handler.
        /// </summary>
        public void RemoveSizeChanged()
        {
            DispatcherHelpers.AssertOnDispatcher(this);

            var sizeChangedEventHandler = _sizeChangedEventHandler;
            if (sizeChangedEventHandler != null)
            {
                SizeChanged -= sizeChangedEventHandler;
            }

            _sizeChangedEventHandler = null;
        }
    }
}
