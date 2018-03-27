// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Concurrent;

namespace ReactNative.UIManager
{
    /// <summary>
    /// React Native internal settings for views.
    /// </summary>
    static class ViewExtensions
    {
        private static readonly ConcurrentDictionary<object, ViewData> s_properties =
            new ConcurrentDictionary<object, ViewData>();
        private static readonly IReactCompoundView s_defaultCompoundView = new ReactDefaultCompoundView();

        /// <summary>
        /// Sets the pointer events for the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="pointerEvents">The pointer events.</param>
        public static void SetPointerEvents(this object view, PointerEvents pointerEvents)
        {
            if (view == null)
                throw new ArgumentNullException(nameof(view));

            s_properties.GetOrAdd(view, (v) => new ViewData()).PointerEvents = pointerEvents;
        }

        /// <summary>
        /// Gets the pointer events for the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <returns>The pointer events.</returns>
        public static PointerEvents GetPointerEvents(this object view)
        {
            if (view == null)
                throw new ArgumentNullException(nameof(view));

            if (!s_properties.TryGetValue(view, out var elementData) || !elementData.PointerEvents.HasValue)
            {
                return PointerEvents.Auto;
            }

            return elementData.PointerEvents.Value;
        }

        /// <summary>
        /// Associates an implementation of IReactCompoundView with the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="compoundView">The implementation of IReactCompoundView.</param>
        public static void SetReactCompoundView(this object view, IReactCompoundView compoundView)
        {
            if (view == null)
                throw new ArgumentNullException(nameof(view));

            s_properties.GetOrAdd(view, (v) => new ViewData()).CompoundView = compoundView;
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
        public static IReactCompoundView GetReactCompoundView(this object view)
        {
            if (view == null)
                throw new ArgumentNullException(nameof(view));

            if (s_properties.TryGetValue(view, out var elementData))
            {
                var compoundView = elementData.CompoundView;
                if (compoundView != null)
                {
                    return compoundView;
                }
            }

            return s_defaultCompoundView;
        }

        internal static void SetTag(this object view, int tag)
        {
            if (view == null)
                throw new ArgumentNullException(nameof(view));

            s_properties.GetOrAdd(view, (v) => new ViewData()).Tag = tag;
        }

        /// <summary>
        /// Get the React tag for the view instance. 
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <returns>The React tag.</returns>
        /// <exception cref="InvalidOperationException">
        /// Thrown if tag is not available for the view.
        /// </exception>
        public static int GetTag(this object view)
        {
            if (view == null)
                throw new ArgumentNullException(nameof(view));

            if (!s_properties.TryGetValue(view, out var elementData) || !elementData.Tag.HasValue)
            {
                throw new InvalidOperationException("Could not get tag for view.");
            }

            return elementData.Tag.Value;
        }

        /// <summary>
        /// Checks if a React tag is available for the view instance.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <returns>
        /// <code>true</code> if the view has a tag, <code>false</code> otherwise.
        /// </returns>
        public static bool HasTag(this object view)
        {
            if (view == null)
                throw new ArgumentNullException(nameof(view));

            return s_properties.TryGetValue(view, out var elementData) && elementData.Tag.HasValue;
        }

        internal static void SetReactContext(this object view, ThemedReactContext context)
        {
            if (view == null)
                throw new ArgumentNullException(nameof(view));

            s_properties.GetOrAdd(view, (v) => new ViewData()).Context = context;
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
        public static ThemedReactContext GetReactContext(this object view)
        {
            if (view == null)
                throw new ArgumentNullException(nameof(view));

            if (!s_properties.TryGetValue(view, out var elementData))
            {
                throw new InvalidOperationException("Could not get React context for view.");
            }

            return elementData.Context;
        }

        internal static void ClearData(this object view)
        {
            s_properties.TryRemove(view, out _);
        }

        class ViewData
        {
            public ThemedReactContext Context { get; set; }

            public PointerEvents? PointerEvents { get; set; }

            public int? Tag { get; set; }

            public IReactCompoundView CompoundView { get; set; }
        }
    }
}
