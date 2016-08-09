using Windows.UI.Xaml;

namespace ReactNative.UIManager
{
    public static class ViewParentManagerExtensions
    {
        /// <summary>
        /// Remove a view from a <see cref="IViewParentManager"/>. 
        /// </summary>
        /// <param name="viewManager">The view manager.</param>
        /// <param name="parent">The parent view.</param>
        /// <param name="view">The view to remove.</param>
        public static void RemoveView(this IViewParentManager viewManager, DependencyObject parent, DependencyObject view)
        {
            for (var i = 0; i < viewManager.GetChildCount(parent); ++i)
            {
                if (viewManager.GetChildAt(parent, i) == view)
                {
                    viewManager.RemoveChildAt(parent, i);
                    break;
                }
            }
        }
    }
}
