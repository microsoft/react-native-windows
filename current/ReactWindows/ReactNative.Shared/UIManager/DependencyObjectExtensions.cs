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
    /// Extension methods for <see cref="DependencyObject"/>.
    /// </summary>
    public static class DependencyObjectExtensions
    {
        /// <summary>
        /// Sets the pointer events for the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="pointerEvents">The pointer events.</param>
        public static void SetPointerEvents(this DependencyObject view, PointerEvents pointerEvents)
        {
            ViewExtensions.SetPointerEvents(view, pointerEvents);
        }

        /// <summary>
        /// Gets the pointer events for the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <returns>The pointer events.</returns>
        public static PointerEvents GetPointerEvents(this DependencyObject view)
        {
            return ViewExtensions.GetPointerEvents(view);
        }

        /// <summary>
        /// Associates an implementation of IReactCompoundView with the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="compoundView">The implementation of IReactCompoundView.</param>
        public static void SetReactCompoundView(this DependencyObject view, IReactCompoundView compoundView)
        {
            ViewExtensions.SetReactCompoundView(view, compoundView);
        }

        /// <summary>
        /// Gets the implementation of IReactCompoundView associated with the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <returns>
        /// The implementation of IReactCompoundView associated with the view. Defaults to
        /// an instance of ReactDefaultCompoundView when no other implementation has been
        /// provided.
        /// </returns>
        public static IReactCompoundView GetReactCompoundView(this DependencyObject view)
        {
            return ViewExtensions.GetReactCompoundView(view);
        }

        /// <summary>
        /// Get the React tag for the view instance. 
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <returns>The React tag.</returns>
        /// <exception cref="InvalidOperationException">
        /// Thrown if tag is not available for the view.
        /// </exception>
        public static int GetTag(this DependencyObject view)
        {
            return ViewExtensions.GetTag(view);
        }

        /// <summary>
        /// Checks if a React tag is available for the view instance.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <returns>
        /// <code>true</code> if the view has a tag, <code>false</code> otherwise.
        /// </returns>
        public static bool HasTag(this DependencyObject view)
        {
            return ViewExtensions.HasTag(view);
        }

        /// <summary>
        /// Gets the <see cref="ThemedReactContext"/> associated with the view
        /// instance.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <returns>The context.</returns>
        /// <exception cref="InvalidOperationException">
        /// Thrown if context is not available for the view.
        /// </exception>
        public static ThemedReactContext GetReactContext(this DependencyObject view)
        {
            return ViewExtensions.GetReactContext(view);
        }

        internal static T As<T>(this DependencyObject view)
            where T : DependencyObject
        {
            var convertedView = view as T;
            if (convertedView == null)
            {
                throw new ArgumentOutOfRangeException(Invariant($"Child of type '{view.GetType()}' is not assignable to '{typeof(T)}'."));
            }
            return convertedView;
        }
    }
}
