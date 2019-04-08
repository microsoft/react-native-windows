// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
#if WINDOWS_UWP
using Windows.Foundation;
using Windows.UI.Xaml;
#else
using System.Windows;
#endif

namespace ReactNative.UIManager
{
    /// <summary>
    /// Interface consisting of methods which are relevant to views which contain
    /// visuals that have react tags but are not rendered using UIElements.
    /// </summary>
    public interface IReactCompoundView
    {
        /// <summary>
        /// Returns the react tag rendered at point in reactView. The view
        /// is not expected to do hit testing on its UIElement descendants. Rather,
        /// this is useful for views which are composed of visuals that are associated
        /// with react tags but the visuals are not UIElements.
        /// </summary>
        /// <param name="reactView">The react view to do hit testing within.</param>
        /// <param name="point">The point to hit test in coordinates that are relative to the view.</param>
        /// <returns>The react tag rendered at point in reactView.</returns>
        int GetReactTagAtPoint(UIElement reactView, Point point);
    }
}
