using System.Collections.Generic;
using System.Linq;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;

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

                current = GetParent(current, false);
            }
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
                return (view as FrameworkElement)?.Parent;
            }
            else
            {
                return VisualTreeHelper.GetParent(view);
            }
        }
    }
}
