// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
#if WINDOWS_UWP
using Windows.UI.Xaml;
#else
using System.Windows;
#endif
using static System.FormattableString;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Helpers to convert between view types.
    /// </summary>
    public static class ViewConversion
    {
        /// <summary>
        /// Gets the <see cref="DependencyObject"/> from the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <returns>The converted view.</returns>
        public static DependencyObject GetDependencyObject(object view)
        {
            if (view is DependencyObject dependencyObject)
            {
                return dependencyObject;
            }

            throw new InvalidOperationException("Cannot convert view to DependencyObject.");
        }

        /// <summary>
        /// Gets the <see cref="DependencyObject"/> from the view.
        /// </summary>
        /// <typeparamref name="T">
        /// The type of <see cref="DependencyObject"/>.
        /// </typeparamref>
        /// <param name="view">The view.</param>
        /// <returns>The converted view.</returns>
        public static T GetDependencyObject<T>(object view)
            where T : DependencyObject
        {
            if (GetDependencyObject(view) is T typedView)
            {
                return typedView;
            }

            throw new InvalidOperationException(Invariant($"Cannot convert view to '{typeof(T)}'."));
        }
    }
}
