using System.Collections.Generic;
using System.Linq;
using Windows.UI.Xaml;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Helper methods for root view management.
    /// </summary>
    public static class RootViewHelper
    {
        private static readonly Dictionary<DependencyObject, DependencyObject> s_parent =
            new Dictionary<DependencyObject, DependencyObject>();

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

                current = GetParent(current);
            }
        }

        /// <summary>
        /// Returns the list of pointer events views in the hierarchy, starting
        /// from the root view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <returns>The pointer events hierarchy.</returns>
        public static IList<UIElement> GetReactViewHierarchy(DependencyObject view)
        {
            return GetReactViewHierarchyCore(view).Reverse().ToList();
        }

        private static IEnumerable<UIElement> GetReactViewHierarchyCore(DependencyObject view)
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

                var uiElement = current as UIElement;
                if (uiElement != null && uiElement.HasTag())
                {
                    yield return uiElement;
                }

                current = GetParent(current);
            }
        }

        private static DependencyObject GetParent(DependencyObject current)
        {
            var mapped = default(DependencyObject);
            var frameworkElement = default(FrameworkElement);
            if (s_parent.TryGetValue(current, out mapped))
            {
                return mapped;
            }
            else if ((frameworkElement = current as FrameworkElement) != null)
            {
                return frameworkElement.Parent;
            }
            else
            {
                return null;
            }
        }

        /// <summary>
        /// Associate an element with its parent.
        /// </summary>
        /// <param name="element">The element.</param>
        /// <param name="parent">The parent.</param>
        /// <remarks>
        /// TODO: (#302) Remove this shim.
        /// </remarks>
        internal static void SetParent(this DependencyObject element, DependencyObject parent)
        {
            lock (s_parent)
            {
                s_parent.Add(element, parent);
            }
        }

        /// <summary>
        /// Unassociate a parent element.
        /// </summary>
        /// <param name="element">The element.</param>
        /// <remarks>
        /// TODO: (#302) Remove this shim.
        /// </remarks>
        internal static void RemoveParent(this DependencyObject element)
        {
            lock (s_parent)
            {
                s_parent.Remove(element);
            }
        }
    }
}
