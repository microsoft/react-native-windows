// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using System.Collections.Generic;
#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;
#else
using System.Windows;
using System.Windows.Documents;
using System.Windows.Media;
#endif

namespace ReactNative.UIManager
{
    /// <summary>
    /// Helper methods for root view management.
    /// </summary>
    public static class RootViewHelper
    {
        /// <summary>
        /// Returns the root view of a givenview in a React application.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <returns>The root view instance.</returns>
        public static ReactRootView GetRootView(DependencyObject view)
        {
            var current = view;
            while (true)
            {
                if (current == null)
                {
                    return null;
                }

                var rootView = current as ReactRootView;
                if (rootView != null)
                {
                    return rootView;
                }

                current = GetParent(current, true);
            }
        }

        /// <summary>
        /// Gets the hierarchy of React views from the given view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <returns>The view hierarchy.</returns>
        public static IEnumerable<DependencyObject> GetReactViewHierarchy(DependencyObject view)
        {
            var current = view;
            while (true)
            {
                if (current == null)
                {
                    yield break;
                }

                if (current is ReactRootView)
                {
                    yield break;
                }

                if (current.HasTag())
                {
                    yield return current;
                }

                current = GetParent(current, false);
            }
        }

        /// <summary>
        /// Checks if the view is part of the React hierarchy.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <returns>
        /// <code>true</code> if the view is part of the hierarchy, otherwise
        /// <code>false</code>.
        /// </returns>
        public static bool IsReactSubview(DependencyObject view)
        {
            return GetReactViewHierarchy(view).GetEnumerator().MoveNext();
        }

        private static DependencyObject GetParent(DependencyObject view, bool findRoot)
        {
            //
            // If the intent is to find the root view (usually for the purpose
            // of measurement), then we use the more robust VisualTreeHelper.
            // Otherwise, we just use the FrameworkElement.Parent property.
            // 
            // This is a bit of a hack to ensure that the SplitView (and
            // perhaps other views) continues to work as expected. In the case
            // of SplitView, if we use the VisualTreeHelper to enumerate the
            // view hierarchy on pointer pressed, it will find a React view 
            // parent that will capture the pointer and prevent the SplitView
            // from closing on click of the lightweight layer Rect (i.e., 
            // when the pointer is pressed outside the SplitView pane). 
            //

            if (!findRoot)
            {
                var frameworkElement = view as FrameworkElement;
                if (frameworkElement != null)
                {
                    return frameworkElement.Parent;
                }
#if !WINDOWS_UWP

                var frameworkContentElement = view as FrameworkContentElement;
                if (frameworkContentElement != null)
                {
                    return frameworkContentElement.Parent;
                }
#endif

                return null;
            }

            return VisualTreeHelper.GetParent(view);
        }
    }
}
